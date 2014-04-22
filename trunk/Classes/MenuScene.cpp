﻿#include "MenuScene.h"
#include "MainGameScene.h"
#include "ScoreScene.h"
#include "AudioManager.h"
#include "DataManager.h"
#include "WaitForLifeDialog.h"
#include "QuitDialog.h"
#include <time.h>
#include "TextLoader.h"
#include "LogOutDialog.h"
#include "TryPlayDialog.h"
#include "NotLoggedInMenuScene.h"

USING_NS_CC;
USING_NS_CC_EXT;


CCScene* MenuScene::scene()
{
    CCScene *scene = CCScene::create();
    MenuScene *layer = MenuScene::create();
    scene->addChild(layer);
    return scene;
}

bool MenuScene::init()
{
	DataManager::sharedDataManager()->RefreshPlayerLife();
	GameClientManager::sharedGameClientManager()->setDelegate(this);

	//////////////////////////////////////////////////////////////////////////

    if ( !CCLayerColor::initWithColor(G_MENU_BG_COLOR) )
    {
        return false;
    }

	m_isShowDialog = false;
	this->setKeypadEnabled(true);

	//////////////////////////////////////////////////////////////////////////
	m_isLoggedIn = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	EziSocialObject::sharedObject()->setFacebookDelegate(this);
	if(EziSocialObject::sharedObject()->isFacebookSessionActive()) //logged in state
	{
		m_isLoggedIn = true;
	}
    else
    {
        m_isLoggedIn = false;
    }
#endif

    /////////////////////////////
	
	string lang = DataManager::sharedDataManager()->GetLanguage();
	if (lang.compare("English") == 0)
	{
		m_bg = CCSprite::create(G_MENU_BG_EN);
	}
	else
	{
		m_bg = CCSprite::create(G_MENU_BG_VN);
	}	
	
	m_bg->setAnchorPoint(G_MENU_BG_ANCHORPOINT);
	m_bg->setPosition(G_MENU_BG_POS);
	this->addChild(m_bg, G_MENU_BG_Z);
	
	initLifeIcon();	

	//////////////////////////////////////////////////////////////////////////

	//
	CCString* s = CCString::createWithFormat("v%d", G_VERSION);
	CCLabelTTF* labelVersion = CCLabelTTF::create(s->getCString(), G_FONT_NORMAL, G_MENU_VERSION_TEXT_SIZE);
	labelVersion->setColor(G_MENU_VERSION_TEXT_COLOR);
	labelVersion->setPosition(G_MENU_VERSION_TEXT_POS);
	this->addChild(labelVersion);

    //
    //new button border
    //
    m_sprNewButtonBorderOut = CCSprite::create("new_button_border_out.png");
    m_sprNewButtonBorderIn = CCSprite::create("new_button_border_in.png");
    
    m_sprNewButtonBorderOut->setPosition(G_MENU_NEW_BUTTON_POS);
    m_sprNewButtonBorderIn->setPosition(G_MENU_NEW_BUTTON_POS);
    
    this->addChild(m_sprNewButtonBorderOut);
    this->addChild(m_sprNewButtonBorderIn);
    
    
    //
    //disable play button animation in local mode
	//
	if (lang.compare("English") == 0)
	{
		m_playItem = CCMenuItemImage::create(
			G_MENU_NEW_BUTTON_SPR_NORMAL_EN,
			G_MENU_NEW_BUTTON_SPR_PRESS_EN,
			this,
			menu_selector(MenuScene::playCallback));
	} 
	else
	{
		m_playItem = CCMenuItemImage::create(
			G_MENU_NEW_BUTTON_SPR_NORMAL_VN,
			G_MENU_NEW_BUTTON_SPR_PRESS_VN,
			this,
			menu_selector(MenuScene::playCallback));
	}

	m_playItem->setPosition(G_MENU_NEW_BUTTON_POS);
    
	m_scoreItem = CCMenuItemImage::create(
		"score_button.png",
		"score_button_press.png",
		this,
		menu_selector(MenuScene::scoreCallback));
	m_scoreItem->setPosition(ccp(315, 1280-1176));

    //
	//rate
    //
	CCMenuItemImage *itRate = CCMenuItemImage::create(
		"rate.png",
		"rateDown.png",
		this,
		menu_selector(MenuScene::rateCallback));
	itRate->setPosition(ccp(502, 1280-1180));

    //
	//exit
    //
	CCMenuItemImage *itExit = CCMenuItemImage::create(
		"exit.png",
		"exitDown.png",
		this,
		menu_selector(MenuScene::exitCallback));
	itExit->setPosition(ccp(692, 1280-1181));


	
	//////////////////////////////////////////////////////////////////////////
	//Setting Bar

	m_sprSettingBar = CCSprite::create("setting_bar.png");
	m_sprSettingBar->setAnchorPoint(ccp(0.5f, 0.0f));
	m_sprSettingBar->setPosition(ccp(108, 1280-1245));
	m_sprSettingBar->setVisible(false);


	m_itShowCharge = CCMenuItemImage::create("charge.png", "charge_down.png", "charge_down.png", this, menu_selector(MenuScene::showChargeCallback));
	m_itShowCharge->setPosition(ccp(70, m_sprSettingBar->getContentSize().height - 65 + 121));


	CCMenuItem* facebookOn = CCMenuItemImage::create("facebook_in.png", NULL, NULL);
	CCMenuItem* facebookOff = CCMenuItemImage::create("facebook_out.png", NULL, NULL);
	m_facebookItem = CCMenuItemToggle::createWithTarget(this,  menu_selector(MenuScene::facebookCallback), facebookOn, facebookOff, NULL);
	m_facebookItem->setPosition(ccp(70, m_sprSettingBar->getContentSize().height-65));
	if (m_isLoggedIn)
	{
		m_facebookItem->setSelectedIndex(1);
	} 
	else
	{
		m_facebookItem->setSelectedIndex(0);
	}

	CCMenuItem* soundOn = CCMenuItemImage::create("sound_on.png", NULL, NULL);
	CCMenuItem* soundOff = CCMenuItemImage::create("sound_off.png", NULL, NULL);
	m_soundItem = CCMenuItemToggle::createWithTarget(this,  menu_selector(MenuScene::soundCallback), soundOn, soundOff, NULL);
	m_soundItem->setPosition(ccp(81, m_sprSettingBar->getContentSize().height-181));
	if(AudioManager::sharedAudioManager()->IsEnableBackground())
	{
		m_soundItem->setSelectedIndex(0);
	}
	else
	{
		m_soundItem->setSelectedIndex(1);
	}

	CCMenu* settingMenu = CCMenu::create(m_facebookItem, m_soundItem, m_itShowCharge, NULL);
	settingMenu->setPosition(CCPointZero);
	
	m_sprSettingBar->addChild(settingMenu);

	this->addChild(m_sprSettingBar, 2);

	CCMenuItem* settingOn = CCMenuItemImage::create("setting.png", NULL, NULL);
	CCMenuItem* settingOff = CCMenuItemImage::create("settingDown.png", NULL, NULL);
	m_settingItem = CCMenuItemToggle::createWithTarget(this,  menu_selector(MenuScene::settingCallback), settingOn, settingOff, NULL);
	m_settingItem->setPosition(ccp(112, 1280-1178));

	CCMenu* menuForSetting = CCMenu::create(m_settingItem, NULL);
	menuForSetting->setPosition(CCPointZero);
	this->addChild(menuForSetting, 3);
	
	//setting bar
	//////////////////////////////////////////////////////////////////////////
	//Language bar

	m_sprLanguageBar = CCSprite::create("language_bar.png");
	m_sprLanguageBar->setAnchorPoint(ccp(0.5f, 0.0f));
	m_sprLanguageBar->setPosition(ccp(688, 1280-1253));
	m_sprLanguageBar->setVisible(false);

	m_englishItem = CCMenuItemImage::create("flag_english.png", "flag_english_down.png", "flag_english_down.png", this, menu_selector(MenuScene::englishCallback));
	m_englishItem->setPosition(ccp(71, m_sprLanguageBar->getContentSize().height-53)); //(703, 1280-947));

	m_vietnamItem = CCMenuItemImage::create("flag_vietnam.png", "flag_vietnam_down.png", "flag_vietnam_down.png", this, menu_selector(MenuScene::vietnamCallback));
	m_vietnamItem->setPosition(ccp(71, m_sprLanguageBar->getContentSize().height-53)); //703, 1280-1065));

	CCMenu* languageMenu = CCMenu::create(m_englishItem, m_vietnamItem, NULL);
	languageMenu->setPosition(CCPointZero);

	m_sprLanguageBar->addChild(languageMenu);

	this->addChild(m_sprLanguageBar, 2);

	//check current language before set
	if (lang.compare("English") == 0)
	{
		m_englishItem->setVisible(false);
		m_languageItem = CCMenuItemImage::create("flag_english.png", "flag_english_down.png", this, menu_selector(MenuScene::languageCallback));
	} 
	else
	{
		m_vietnamItem->setVisible(false);
		m_languageItem = CCMenuItemImage::create("flag_vietnam.png", "flag_vietnam_down.png", this, menu_selector(MenuScene::languageCallback));
	}
	m_languageItem->setPosition(ccp(690, 1280-1202));

	CCMenu* menuForLanguage = CCMenu::create(m_languageItem, NULL);
	menuForLanguage->setPosition(CCPointZero);
	this->addChild(menuForLanguage, 3);


	//Language bar
	//////////////////////////////////////////////////////////////////////////

    m_menu = CCMenu::create(m_playItem, m_scoreItem, itRate, NULL);
    m_menu->setPosition(CCPointZero);
    this->addChild(m_menu, 1);



	//check if life = 0 to show

	int life = DataManager::sharedDataManager()->GetLastPlayerLife();
	//CCLOG("MenuScene: Last life = %d", life);
	if (life < G_MAX_PLAYER_LIFE) //start counter when not full of life
	{
		initTimer();
	}

	if (DataManager::sharedDataManager()->GetRegistrationId().length() == 0)
	{
		GetRegistrationId();
	}


	STOP_BACKGROUND_MUSIC;
    return true;
}

void MenuScene::initLifeIcon()
{
	float w = CCSprite::create("oil.png")->getContentSize().width;
	float x = (800 - w * 5)/2 + w/2;
	float y = 1280 - 445;

	m_arrSprLife = new CCArray();
	m_arrSprLife->retain();

	//life icon
	for (int i = 0; i < G_MAX_PLAYER_LIFE; ++i)
	{
		CCSprite* _sprBlur = CCSprite::create("oil.png");
		_sprBlur->setOpacity(50);
		_sprBlur->setPosition(ccp(x + i * w, y));
		this->addChild(_sprBlur, 1); //////////////////////////////////new ////////////////////////////////////////

		CCSprite* _spr = CCSprite::create("oil.png");
		_spr->setPosition(ccp(x + i * w, y));
		_spr->setVisible(false);
		this->addChild(_spr, 1.1); ////////////////////////////////////new //////////////////////////////////////
		
		m_arrSprLife->addObject(_spr);
	}

	int life = DataManager::sharedDataManager()->GetLastPlayerLife();
	for (int i = 0; i < life; ++i)
	{
		CCSprite* _spr = (CCSprite*) m_arrSprLife->objectAtIndex(i);
		_spr->setVisible(true);
	}
}

void MenuScene::refreshLifeIcon()
{
	for (int i = 0; i < G_MAX_PLAYER_LIFE; ++i)
	{
		CCSprite* _spr = (CCSprite*) m_arrSprLife->objectAtIndex(i);
		_spr->setVisible(false);
	}

	////////////////////////////////////////////////////////////////////////////////////////////

	int life = DataManager::sharedDataManager()->GetLastPlayerLife();
	for (int i = 0; i < life; ++i)
	{
		CCSprite* _spr = (CCSprite*) m_arrSprLife->objectAtIndex(i);
		_spr->setVisible(true);
	}
}

void MenuScene::playStartAnimation(int lastLife)
{
    CCSprite* spr = (CCSprite*)m_arrSprLife->objectAtIndex(lastLife - 1);
	CCSequence* seq = CCSequence::create(
        CCMoveTo::create(0.6f, ccp(spr->getPositionX(), 1280 + 200)),
		CCCallFunc::create(this, callfunc_selector(MenuScene::gotoMainGame)),
		NULL);
	spr->runAction(seq);
}

void MenuScene::gotoMainGame()
{
    if (m_isLoggedIn == false) {
        TryPlayDialog* trydialog = TryPlayDialog::create();
        this->addChild(trydialog, 10);
        this->onShowDialog();
    } else {
        GameClientManager::sharedGameClientManager()->setDelegate(NULL);
        
        CCScene *pScene = CCTransitionFade::create(0.5, MainGameScene::scene());
        CCDirector::sharedDirector()->replaceScene(pScene);
    }
}

void MenuScene::playCallback(CCObject* pSender)
{
    if (m_isLoggedIn == true) {
        CCLOG("ENEMY DOWN");
        PLAY_ENEMY1_DOWN_EFFECT;
    } else {
        CCLOG("GUN RELOAD");
        PLAY_GUN_RELOAD_EFFECT;
    }

    //
    //animation
    //
    
    m_sprNewButtonBorderOut->runAction(
        CCSequence::createWithTwoActions(
            CCRotateTo::create(0.15f, 40),
            CCRotateTo::create(0.15f, 0)
        ));
    m_sprNewButtonBorderIn->runAction(
        CCSequence::createWithTwoActions(
            CCRotateTo::create(0.15f, -40),
            CCRotateTo::create(0.15f, 0)
        ));
    
	this->setKeypadEnabled(false);
	this->setTouchEnabled(false);

	//check if last_player_life > 0
	int lastLife = DataManager::sharedDataManager()->GetLastPlayerLife();

	//CCLOG("GOTO PLAY: Lastlife: %d", lastLife);

	if (lastLife > 0) //enough life -> go play
	{
        playStartAnimation(lastLife);
        m_playItem->selected();
	}
	else //not enough life -> request life
	{
		WaitForLifeDialog* dialog = WaitForLifeDialog::create((float)G_PLAYER_TIME_TO_REVIVE);
		this->addChild(dialog, 100);
		this->setTouchEnabled(false);
		this->onShowDialog();
	}
}

void MenuScene::scoreCallback( CCObject* pSender )
{
	PLAY_BUTTON_EFFECT;

	GameClientManager::sharedGameClientManager()->setDelegate(NULL);

	CCScene *pScene = CCTransitionFade::create(0.5, ScoreScene::scene());
	CCDirector::sharedDirector()->replaceScene(pScene);
}

void MenuScene::keyBackClicked()
{
	exitCallback(NULL);
}

void MenuScene::onRateCompleted( CCNode *sender, void *data )
{
	if (data != NULL)
	{
		CCDictionary *convertedData = (CCDictionary *)data;
		CCString* s = (CCString*)convertedData->objectForKey("isSuccess");
		if (s->boolValue())
		{
			//CCLOG("CPP Rate Completed: TRUE");
			CCString* s = (CCString*)convertedData->objectForKey("responseType");
			//CCLOG("%s", s->getCString());
			if (s->compare("RATE") == 0)
			{
				CCMessageBox(TXT("menu_thank_rate"), TXT("menu_thank_caption"));
			}
		} 
		else
		{
			//CCLOG("CPP Rate Completed: FALSE");
		}

		NDKHelper::RemoveSelector("MENU", "onRateCompleted");
	}
}

void MenuScene::onShowDialog()
{
	m_menu->setEnabled(false);
	this->setKeypadEnabled(false);
}

void MenuScene::onCloseDialog()
{
	DataManager::sharedDataManager()->RefreshPlayerLife();
	initLifeIcon();
	
	m_menu->setEnabled(true);
	this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
}

void MenuScene::onCompletedWaiting()
{
	//get revive_life
	tm* lasttm = DataManager::sharedDataManager()->GetLastDeadTime();
	time_t lastTime = mktime(lasttm);
	time_t curTime = time(NULL);
	double seconds = difftime(curTime, lastTime);

	int lastLife = (int)(seconds / G_PLAYER_TIME_TO_REVIVE);
	lastLife = (lastLife > G_MAX_PLAYER_LIFE) ? G_MAX_PLAYER_LIFE : lastLife;

	//CCLOG("Revive Last life: %d", lastLife);

	if (lastLife > 0)
	{
		DataManager::sharedDataManager()->SetLastPlayerLife(lastLife);
		initLifeIcon();
	}
	else
	{
		//CCLOG("Your code is failed!, F**k the coder!");
	}
}

void MenuScene::soundCallback( CCObject* pSender )
{
	if(AudioManager::sharedAudioManager()->IsEnableBackground())
	{
		PLAY_BUTTON_EFFECT;

		AudioManager::sharedAudioManager()->SetEnableBackground(false);
		AudioManager::sharedAudioManager()->SetEnableEffect(false);

		//
		AudioManager::sharedAudioManager()->StopBackground();
	}
	else
	{
		AudioManager::sharedAudioManager()->SetEnableBackground(true);
		AudioManager::sharedAudioManager()->SetEnableEffect(true);
	}

	//m_sprSettingBar->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCHide::create()));
}

void MenuScene::rateCallback( CCObject* pSender )
{
	NDKHelper::AddSelector("MENU",
		"onRateCompleted",
		callfuncND_selector(MenuScene::onRateCompleted),
		this);

	CCDictionary* prms = CCDictionary::create();
	prms->setObject(CCString::create(TXT("game_name")), "game_name");
	prms->setObject(CCString::create(TXT("game_package")), "game_package");
	prms->setObject(CCString::create(TXT("rate_msg")), "msg");
	prms->setObject(CCString::create(TXT("rate_rate")), "rate");
	prms->setObject(CCString::create(""), "later");
	prms->setObject(CCString::create(TXT("rate_no")), "no");

	SendMessageWithParams(string("Rate"), prms);
}

void MenuScene::settingCallback( CCObject* pSender )
{
	if (m_sprSettingBar->isVisible())
	{
		m_sprSettingBar->setVisible(false);
	} 
	else
	{
		m_sprSettingBar->setVisible(true);
	}
}

void MenuScene::facebookCallback( CCObject* pSender )
{
	PLAY_BUTTON_EFFECT;

	if (m_isLoggedIn == true)
	{
		LogOutDialog* dialog = LogOutDialog::create();
		this->addChild(dialog, 10);
		this->onShowDialog();
	}
	else
	{
		facebookLogInOut();
	}

	/////////////refresh view ///////////////
	if (m_isLoggedIn)
	{
		m_facebookItem->setSelectedIndex(1);
	} 
	else
	{
		m_facebookItem->setSelectedIndex(0);
	}
}

void MenuScene::facebookLogInOut()
{
	if (m_isLoggedIn)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		EziSocialObject::sharedObject()->perfromLogoutFromFacebook();
#endif
	} 
	else
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		bool needPublicPermission = true;
		EziSocialObject::sharedObject()->performLoginUsingFacebook(needPublicPermission); // Pass true if you need publish permission also
        this->retain();
        
        CCLOG("-------------- RETAIN MENU SCENE");
#endif
	}
}

void MenuScene::exitCallback( CCObject* pSender )
{
	QuitDialog* dialog = QuitDialog::create();
    dialog->setYesNoDialogParent(this);
	this->addChild(dialog, 10);
	this->onShowDialog();
}

void MenuScene::ScheduleTick( float dt )
{
	tm* lasttm = DataManager::sharedDataManager()->GetLastDeadTime();
	int diff = (int)difftime(time(NULL), mktime(lasttm));

	m_waitTime = 1000 * G_PLAYER_TIME_TO_REVIVE - diff;
	m_waitTime %= G_PLAYER_TIME_TO_REVIVE;

	int mins = m_waitTime / 60;
	int seconds = m_waitTime % 60;

	if (seconds == 0)
	{
		DataManager::sharedDataManager()->RefreshPlayerLife();
		refreshLifeIcon();

		int life = DataManager::sharedDataManager()->GetLastPlayerLife();
		//CCLOG("REFRESH LIFE ICON, Life = %d", life);
		if (life >= G_MAX_PLAYER_LIFE)
		{
			this->unschedule(schedule_selector(MenuScene::ScheduleTick));
			m_lbTime->setVisible(false);
			return;
		}
	}	

	CCString* s;
	if (seconds >= 10)
	{
		s = CCString::createWithFormat("0%d:%d", mins, seconds);
	}
	else
	{
		s = CCString::createWithFormat("0%d:0%d", mins, seconds);
	}

	m_lbTime->setString(s->getCString());
}

void MenuScene::initTimer()
{
	//CCLOG("Initing... timer! ...");

	DataManager::sharedDataManager()->GetLastPlayerLife();
	tm* lasttm = DataManager::sharedDataManager()->GetLastDeadTime();
	int diff = (int)difftime(time(NULL), mktime(lasttm));

	m_waitTime = 1000 * G_PLAYER_TIME_TO_REVIVE - diff;
	m_waitTime %= G_PLAYER_TIME_TO_REVIVE;


	if (m_waitTime < 0)
	{
		DataManager::sharedDataManager()->SetLastPlayerLife(G_MAX_PLAYER_LIFE);
		return;
	}
	
	int mins = m_waitTime / 60;
	int seconds = m_waitTime % 60;

	CCString* s;
	if (seconds >= 10)
	{
		s = CCString::createWithFormat("0%d:%d", mins, seconds);
	} 
	else
	{
		s = CCString::createWithFormat("0%d:0%d", mins, seconds);
	}
	
	m_lbTime = CCLabelTTF::create(s->getCString(), "Roboto-Medium", 48);
	m_lbTime->setPosition(ccp(400, 1280-531));
	m_lbTime->setColor(ccc3(56, 56, 56));
	this->addChild(m_lbTime);

	this->schedule(schedule_selector(MenuScene::ScheduleTick), 1);
}

//////////////////////////////////////////////////////////////////////////

void MenuScene::GetRegistrationId()
{
	//CCLOG("GetRegistrationId");
	NDKHelper::AddSelector("MENU",
		"onGetRegistrationIdCompleted",
		callfuncND_selector(MenuScene::onGetRegistrationIdCompleted),
		this);
	SendMessageWithParams(string("GetRegistrationId"), NULL);
}

void MenuScene::onGetRegistrationIdCompleted( CCNode *sender, void *data )
{
	//CCLOG("onGetRegistrationIdCompleted");
	if (data != NULL)
	{
		CCDictionary *convertedData = (CCDictionary *)data;
		CCString* s = (CCString*)convertedData->objectForKey("isSuccess");
		if (s->boolValue())
		{
			//CCLOG("CPP RegistrationId Completed: TRUE");
			CCString* s = (CCString*)convertedData->objectForKey("registrationId");
			CCLOG("CPP: RegistrationId: %s", s->getCString());

			DataManager::sharedDataManager()->SetRegistrationId(s->getCString());

			//send to server if fbId != NULL
			string fbid = DataManager::sharedDataManager()->GetFbID();
			if (fbid.compare("NULL") != 0)
			{
				GameClientManager::sharedGameClientManager()->sendDeviceProfile(
				DataManager::sharedDataManager()->GetFbID(),
				string(""),
				s->getCString(),
				string(""),
				string("")
				);
			}			
		} 
		else
		{
			//CCLOG("CPP RegistrationId Completed: FALSE");
		}

		NDKHelper::RemoveSelector("MENU", "onGetRegistrationIdCompleted");
	}
}

//////////////////////////////////////////////////////////////////////////


//END MY FUNCTION

//when did Logged In  || Logged Out
void MenuScene::fbSessionCallback(int responseCode, const char *responseMessage)
{
	//CCLOG("fbSessionCallback");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (responseCode == EziSocialWrapperNS::RESPONSE_CODE::FB_LOGIN_SUCCESSFUL)
	{
		//CCLOG("fbSessionCallback: SUCCESSFUL");
		m_isLoggedIn = true;

		//auto get profile, info
	}
	else
	{
		//CCLOG("fbSessionCallback: FAILED");
		m_isLoggedIn = false;
        
        
        //
        //Clear data
        //
        //set FbId = "NULL";
        DataManager::sharedDataManager()->ClearFbProfileID();
        
        //clear highscores
        DataManager::sharedDataManager()->SetHigherFriends(NULL);
        
        //go to NotLoggedInMenuSene
        GameClientManager::sharedGameClientManager()->setDelegate(NULL);
        
        CCScene *pScene = CCTransitionFade::create(0.5, NotLoggedInMenuScene::scene());
        CCDirector::sharedDirector()->replaceScene(pScene);
	}

	/////////////refresh view ///////////////
	if (m_isLoggedIn == true)
	{
		m_facebookItem->setSelectedIndex(1);
        //m_scoreItem->setVisible(true);
        //m_scoreItem->runAction(CCFadeIn::create(0.5f));
	}
	else
	{
		m_facebookItem->setSelectedIndex(0);
        //m_scoreItem->runAction(CCFadeOut::create(0.5f));
	}
    
    
    //
    //start button
    //
    string lang = DataManager::sharedDataManager()->GetLanguage();
    string en_start_press = string(G_MENU_NEW_BUTTON_SPR_PRESS_EN);
    string vn_start_press = string(G_MENU_NEW_BUTTON_SPR_PRESS_VN);
    if (m_isLoggedIn == false)
    {
        en_start_press = string(G_MENU_NEW_BUTTON_SPR_NORMAL_EN);
        vn_start_press = string(G_MENU_NEW_BUTTON_SPR_NORMAL_VN);
    }
    
    if (lang.compare("English") == 0)
    {
        CCSprite* selected_image = CCSprite::create(en_start_press.c_str());
        m_playItem->setSelectedImage(selected_image);
    }
    else
    {
        CCSprite* selected_image = CCSprite::create(vn_start_press.c_str());
        m_playItem->setSelectedImage(selected_image);
    }

#endif
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS) // fbUserDetailCallback

void MenuScene::fbUserDetailCallback( int responseCode, const char* responseMessage, EziFacebookUser* fbUser )
{
	//CCLOG("fbUserDetailCallback");
	if (fbUser != NULL)
	{
		//CCLOG("fbUserDetailCallback: user != NULL");
		EziSocialObject::sharedObject()->setCurrentFacebookUser(fbUser);

		//save data
		std::string firstname = fbUser->getFirstName(); //getFirstName //getFullName
		std::string userName = fbUser->getUserName();
		std::string profileID = fbUser->getProfileID();
		std::string fullName = fbUser->getFullName(); //getEmailID
		std::string emailID = fbUser->getEmailID();

		DataManager::sharedDataManager()->SetName(firstname.c_str());
		DataManager::sharedDataManager()->SetFbProfileID(profileID.c_str());
		DataManager::sharedDataManager()->SetFbUserName(userName.c_str());
		DataManager::sharedDataManager()->SetFbFullName(fullName.c_str());
		DataManager::sharedDataManager()->SetFbEmail(emailID.c_str());

		sendUserProfileToServer(profileID, fullName, emailID);

		getFacebookFriends();

		EziSocialObject::sharedObject()->getProfilePicForID(this, fbUser->getProfileID(), G_AVATAR_SIZE, G_AVATAR_SIZE, false);
	}
}
#endif

void MenuScene::fbUserPhotoCallback(const char *userPhotoPath, const char* fbID)
{
	//CCLOG("fbUserPhotoCallback");
	std::string sid = std::string(fbID);

	if ((strcmp(userPhotoPath, "") != 0))
	{
		DataManager::sharedDataManager()->SetPhotoPath(userPhotoPath);
	}
}

void MenuScene::sendUserProfileToServer(string fbId, string fbName, string email)
{
	//CCLOG("sendUserProfileToServer");
	GameClientManager::sharedGameClientManager()->sendPlayerFbProfile(fbId, fbName, email, string(G_APP_ID));
	
	//send device token to server
	string fbid = DataManager::sharedDataManager()->GetRegistrationId();
	if (fbid.length() > 0)
	{
		GameClientManager::sharedGameClientManager()->sendDeviceProfile(
			DataManager::sharedDataManager()->GetFbID(),
			string(""),
			DataManager::sharedDataManager()->GetRegistrationId(),
			string(""),
			string("")
			);
	}
}

void MenuScene::getFacebookFriends()
{
	//CCLOG("getFacebookFriends");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ScoreScene::s_beginFriendInd = 0;
	ScoreScene::s_endFriendInd = G_NUMBER_FRIEND_TO_GET - 1;
	EziSocialObject::sharedObject()->getFriends(EziSocialWrapperNS::FB_FRIEND_SEARCH::ALL_FRIENDS, ScoreScene::s_beginFriendInd, ScoreScene::s_endFriendInd);

	DataManager::sharedDataManager()->SetTimeRefreshFriendNow();
#endif
}

void MenuScene::fbFriendsCallback( int responseCode, const char* responseMessage, cocos2d::CCArray* friends )
{
	//CCLOG("fbFriendsCallback");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	int count = friends->count();
    CCLOG("GOT >>>>>>>>>>>>>> %d friends", count);
	if (count > 0)
	{
		CCArray* arrFriends = new CCArray();
		arrFriends->retain();

		CCObject* it;
		CCARRAY_FOREACH(friends, it)
		{
			EziFacebookFriend* fbFriend = dynamic_cast<EziFacebookFriend*>(it);
			if (NULL != it)
			{
				string fbId = fbFriend->getFBID();
				string fbName = fbFriend->getName();
				FacebookAccount* acc = new FacebookAccount(fbId, fbName, string(), -1);

				arrFriends->addObject(acc);
			}
		}

		GameClientManager::sharedGameClientManager()->sendFriendList(DataManager::sharedDataManager()->GetFbID(), arrFriends);		

		ScoreScene::s_beginFriendInd += G_NUMBER_FRIEND_TO_GET;
		ScoreScene::s_endFriendInd += G_NUMBER_FRIEND_TO_GET;
		EziSocialObject::sharedObject()->getFriends(EziSocialWrapperNS::FB_FRIEND_SEARCH::ALL_FRIENDS, ScoreScene::s_beginFriendInd, ScoreScene::s_endFriendInd);
	} else {
        //end of friends
        this->release();
        CCLOG("RELEASE MENU SCENE --------------");
    }
#endif
}

void MenuScene::languageCallback( CCObject* pSender )
{
	if (m_sprLanguageBar->isVisible())
	{
		m_sprLanguageBar->setVisible(false);
	} 
	else
	{
		m_sprLanguageBar->setVisible(true);
	}
}

void MenuScene::englishCallback( CCObject* pSender )
{
	m_englishItem->setVisible(false);
	m_vietnamItem->setVisible(true);

    
    CCSprite* normalImage = (CCSprite*)m_englishItem->getNormalImage();
    CCSprite* newNormalImage = CCSprite::createWithTexture(normalImage->getTexture());
    
    CCSprite* selectedImage = (CCSprite*)m_englishItem->getSelectedImage();
    CCSprite* newSelectedImage = CCSprite::createWithTexture(selectedImage->getTexture());
    
	m_languageItem->setNormalImage(newNormalImage);
	m_languageItem->setSelectedImage(newSelectedImage);
    
	m_sprLanguageBar->setVisible(false);
	DataManager::sharedDataManager()->SetLanguage("English");
	TextLoader::shareTextLoader()->setCurrentLanguage(LANGUAGE_ENGLISH);

	//refresh UI
	refreshLanguageUI();
}

void MenuScene::vietnamCallback( CCObject* pSender )
{
	m_englishItem->setVisible(true);
	m_vietnamItem->setVisible(false);

    CCSprite* normalImage = (CCSprite*)m_vietnamItem->getNormalImage();
    CCSprite* newNormalImage = CCSprite::createWithTexture(normalImage->getTexture());
    
    CCSprite* selectedImage = (CCSprite*)m_vietnamItem->getSelectedImage();
    CCSprite* newSelectedImage = CCSprite::createWithTexture(selectedImage->getTexture());
    
	m_languageItem->setNormalImage(newNormalImage);
	m_languageItem->setSelectedImage(newSelectedImage);

	m_sprLanguageBar->setVisible(false);
	DataManager::sharedDataManager()->SetLanguage("Vietnamese");
	TextLoader::shareTextLoader()->setCurrentLanguage(LANGUAGE_VIETNAMESE);

	//refresh UI
	refreshLanguageUI();
}

void MenuScene::refreshLanguageUI()
{
	string lang = DataManager::sharedDataManager()->GetLanguage();

	if (lang.compare("English") == 0)
	{
		//bg
		m_bg->setTexture(CCSprite::create(G_MENU_BG_EN)->getTexture());

		//start
		m_playItem->setNormalImage(CCSprite::create(G_MENU_NEW_BUTTON_SPR_NORMAL_EN));
		m_playItem->setSelectedImage(CCSprite::create(G_MENU_NEW_BUTTON_SPR_PRESS_EN));
	}
	else
	{
		//bg
        m_bg->setTexture(CCSprite::create(G_MENU_BG_VN)->getTexture());

		//start
		m_playItem->setNormalImage(CCSprite::create(G_MENU_NEW_BUTTON_SPR_NORMAL_VN));
		m_playItem->setSelectedImage(CCSprite::create(G_MENU_NEW_BUTTON_SPR_PRESS_VN));
	}
}

void MenuScene::onPushNotification( CCNode *sender, void *data )
{
	if (data != NULL)
	{
		CCDictionary *convertedData = (CCDictionary *)data;
		CCString* s = (CCString*)convertedData->objectForKey("isSuccess");
		if (s->boolValue())
		{
			//CCLOG("CPP onPushNotification Completed: TRUE");
			CCMessageBox("Push notification Success!", "Kaka");
		} 
		else
		{
			//CCLOG("CPP onPushNotification Completed: FALSE");
		}

		//NDKHelper::RemoveSelector("MENU", "onPushNotification");
	}
}

void MenuScene::showChargeCallback( CCObject* pSender )
{
	NDKHelper::AddSelector("MENU",
		"onShowChargeCompleted",
		callfuncND_selector(MenuScene::onShowChargeCompleted),
		this);

	CCDictionary* prms = CCDictionary::create();
	prms->setObject(CCString::create(TXT("charge_caption")), "charge_caption");
	prms->setObject(CCString::create(TXT("charge_close")), "charge_close");
	prms->setObject(CCString::create(TXT("charge_link")), "charge_link");

	SendMessageWithParams(string("ShowChargeWebView"), prms);
}

void MenuScene::onShowChargeCompleted( CCNode *sender, void *data )
{
	//CCLOG("CPP: onShowChargeCompleted");
	//m_sprSettingBar->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCHide::create()));
}

void MenuScene::getUserProfile()
{
	string fbId = DataManager::sharedDataManager()->GetFbID();

	if (fbId.compare("NULL") != 0)
	{
		GameClientManager::sharedGameClientManager()->getPlayerFbProfile(fbId);
	}
	else
	{
		CCLOG("fbId == NULL, could not connect server to get diamond");
	}
}

void MenuScene::onGetPlayerFbProfileCompleted( bool isSuccess, FacebookAccount* acc )
{
	if (isSuccess)
	{
		CCLOG("DIAMOND: %d", acc->m_coin);
		DataManager::sharedDataManager()->SetDiamon(acc->m_coin);
	}
	else
	{
		CCMessageBox(TXT("menu_error_server"), TXT("menu_error_caption"));
		this->disableMoneytize();
	}
}

void MenuScene::onSendPlayerFbProfileCompleted( bool isSuccess )
{
	if (isSuccess)
	{
		//get User Profile
		this->getUserProfile();
	}
	else
	{
		//failed to connect server
		this->disableMoneytize();
	}
}

void MenuScene::disableMoneytize()
{
	////iOS
	//CCLOG("DISABLE MONEYTIZE...");
	//DataManager::sharedDataManager()->SetIsMoneytize(false);
	////remove chargeTutorial
	//m_itShowCharge->setVisible(false);

	//Android, Windows Phone
	CCLOG("SET DIAMOND = DEFAULT...");
	DataManager::sharedDataManager()->SetDiamon(G_DEFAULT_DIAMON);
}

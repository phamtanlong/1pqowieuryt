#ifndef __SCORE_SCENE_H__
#define __SCORE_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CustomTableViewCell.h"
#include "GameClientManager.h"
#include "GameClientDelegate.h"
#include "GameClientObjects.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "EziSocialObject.h"
#include "EziSocialDelegate.h"
#include "EziFacebookFriend.h"
#include "EziFBIncomingRequestManager.h"
#include "EziFBIncomingRequest.h"
#endif

#include <algorithm>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class ScoreScene : public cocos2d::CCLayer, 
	public GameClientDelegate,
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	public EziFacebookDelegate,
#endif
	public cocos2d::extension::CCTableViewDataSource, 
	public cocos2d::extension::CCTableViewDelegate
{
public:
	virtual bool init();
	CREATE_FUNC(ScoreScene);
	static CCScene* scene() {
		CCScene *scene = CCScene::create();
		scene->addChild(ScoreScene::create());
		return scene;
	}

	void itXephangCallback(CCObject* pSender);
	void itQuatangCallback(CCObject* pSender);
	void itFbLogOutCallback(CCObject* pSender);
	void itFbLogInCallback(CCObject* pSender);
	void itMenuCallback(CCObject* pSender);
	virtual void keyBackClicked();
	void itAddFriendCallback(CCObject* pSender);
	void itGetBoomCallback(CCObject* pSender);
	void itGetBoomNowCallback(CCObject* pSender);
	void itSendLifeCallback(CCObject* pSender);

	static int CompareEziFriendScore(const CCObject* p1, const CCObject* p2)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			EziFacebookFriend* f1 = (EziFacebookFriend*)(p1);
			EziFacebookFriend* f2 = (EziFacebookFriend*)(p2);

			return (f1->getScore() > f2->getScore());
		#else
			return true;
		#endif
	}
	static void SortEziFriendScoreList(CCArray* arr)
	{
		std::sort(arr->data->arr, 
			arr->data->arr + arr->data->num, CompareEziFriendScore);
	}

	//table delegate
	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view) {};
	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view) {};
	virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
	virtual cocos2d::CCSize tableCellSizeForIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);
	//table delegate

	CCTableViewCell* getTableCellXepHangAtIndex(CCTableView *table, unsigned int idx);
	CCTableViewCell* getTableCellQuatangAtIndex(CCTableView *table, unsigned int idx);
	
	void refreshView();
	void refreshUserDetail();
	void scheduleTimer(float dt);

public:
	static int s_beginFriendInd;
	static int s_endFriendInd;

	bool m_isLoggedIn;
	bool m_isXepHangView;
	int m_tableXepHangSize;
	int m_tableQuatangSize;

	//UI
	CCMenuItemToggle* m_itXephangToggle;
	CCMenuItemToggle* m_itQuatangToggle;
	CCMenuItemImage* m_itFbLogInItem;

	CCLabelTTF* m_lbInviteQuatang;
	CCLabelTTF* m_lbInvite;
	CCSprite* m_sprWaiting;
	CCLabelTTF* m_lbLostConnection;
	CCLabelTTF* m_lbName;
	CCLabelTTF* m_lbScore;
	CCSprite* m_sprCell;

	CCTableView* m_tableXephang;
	CCTableView* m_tableQuatang;

	CCArray* m_arrHighScores;
	CCArray* m_arrRequests;

	CustomTableViewCell* m_friendCell;

	// Facebook //=========================================
	
	CCSprite* m_userAvatar;
	CCSprite* m_sprLife;
	CCSprite* m_sprBoom;
	CCSprite* m_sprDiamond;
	CCLabelTTF* m_lbLife;
	CCLabelTTF* m_lbBoom;
	CCLabelTTF* m_lbDiamond;
	CCArray* m_friendList;
	
	void checkRefreshFriendList();
	void submitScore();
	void syncScore();
	void getHighScores();
	void getFacebookFriends();
	virtual void onGetFriendListCompleted(bool isSuccess, CCArray* arrFriends);
	void sendUserProfileToServer(string fbId, string fbName, string email);
	virtual void onSendPlayerFbProfileCompleted( bool isSuccess ){ CCLOG("onSendPlayerFbProfileCompleted"); if(isSuccess) CCLOG("TRUE"); else CCLOG("FALSE"); }
	void postMessageToLoser(std::string loserName, std::string loserUserName, int yourScore);
	virtual void onSendFriendListCompleted( bool isSuccess ){ CCLOG("onSendFriendListCompleted"); if(isSuccess) CCLOG("TRUE"); else CCLOG("FALSE"); }
	virtual void onGetScoreCompleted( bool isSuccess, int score, std::string time );

	//Ezibyte
	virtual void fbFriendsCallback(int responseCode, const char* responseMessage, cocos2d::CCArray* friends);
	virtual void fbMessageCallback(int responseCode, const char* responseMessage);
	virtual void fbSessionCallback(int responseCode, const char* responseMessage);
	virtual void fbUserPhotoCallback(const char *userPhotoPath, const char* fbID);
	virtual void fbSendRequestCallback(int responseCode, const char* responseMessage, cocos2d::CCArray* friendsGotRequests);
	virtual void fbIncomingRequestCallback(int responseCode, const char* responseMessage, int totalIncomingRequests);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	virtual void fbUserDetailCallback(int responseCode, const char* responseMessage, EziFacebookUser* fbUser);
#endif
	// Facebook //=========================================
};

#endif // __SCORE_SCENE_H__
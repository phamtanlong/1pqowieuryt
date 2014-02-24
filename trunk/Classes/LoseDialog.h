#ifndef __LOSE_DIALOG_H__
#define __LOSE_DIALOG_H__

#include "cocos2d.h"
USING_NS_CC;

class LoseDialog : public cocos2d::CCLayer
{
public:
	LoseDialog(int score, int killedEnemies):CCLayer(){
		this->m_score = score;
		this->m_killedEnemies = killedEnemies;
	};
    virtual bool init();
	static LoseDialog* create(int score, int killedEnemies) {
		LoseDialog* dig = new LoseDialog(score, killedEnemies);
		dig->init();
		dig->autorelease();
		return dig;
	}

private:
	int m_score;
	int m_killedEnemies;

	//particle
	CCParticleSystem*    m_emitter;

public:
	void menuCallBack(CCObject* pSender);
	void reviveCallBack(CCObject* pSender);

private:
	void breakRecord();
	void initParticle();
};

#endif // __LOSE_DIALOG_H__

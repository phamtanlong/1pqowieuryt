#include "CustomTableViewCell.h"

USING_NS_CC;

CustomTableViewCell::CustomTableViewCell()
{
	//get boom
	m_lastTimeGetBoom = 0;
	m_lbGetBoomTimer = NULL;
	m_lbGetBoom = NULL;
	m_itGetBoom = NULL;
    m_sprWait = NULL;

	//send life
	m_lastTimeSendLife = NULL;
	m_lbSendLifeTimer = NULL;
	m_lbSendLife = NULL;
	m_itSendLife = NULL;
    
}
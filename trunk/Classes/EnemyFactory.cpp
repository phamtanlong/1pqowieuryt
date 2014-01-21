#include "cocos2d.h"
#include "EnemyFactory.h"
#include "Enemy.h"
#include "Global.h"
#include "ObjectLayer.h"

USING_NS_CC;

#define MAX_ENEMY_BIG	1
#define MAX_ENEMY_MED	3

static float getVy(int id) {
	static float vy[NUM_ENEMY_TYPE] = {
		-0.35f,
		-0.42f,
		-0.49f,
		-0.56f,
		-0.63f,
		-0.7f,

		-0.3f,
		-0.36f,
		-0.42f,
		-0.48f,
		-0.54f,

		-0.12f,
		-0.2f,
		-0.24f,
		-0.28f
	};

	return vy[id];
}

EnemyFactory::EnemyFactory()
{
	m_smallCounter = 0;
	m_bigCounter = 0;

	m_currentLevel = (*LevelLoader::shareLevelLoader()->GetValueLowerThan(0)).Clone();
	m_savedLevel = m_currentLevel.Clone();
}

void EnemyFactory::update( float delta, int score )
{
	m_smallCounter += delta;

	if (m_smallCounter >= m_currentLevel.m_smallDelay) // generate in pack
	{
		m_smallCounter = 0;

		//generate in pack

		int rd = (int)(CCRANDOM_0_1() * NUM_ENEMY_TYPE); //0-14

		for (int i = 0; i < NUM_ENEMY_TYPE; ++i) //0-14
		{
			int id = rd + i;
			if (id >= NUM_ENEMY_TYPE)
			{
				id -= NUM_ENEMY_TYPE;
			}

			int val = m_currentLevel.m_arrEnemy[id];
			Enemy* enemy;
			
			if (val > 0)
			{
				if (id < ENEMY_SMALL_TYPE_1_6)
				{
					enemy = Enemy::create(1, id, m_currentLevel.m_hp1, getVy(id));
				}
				else if (id < ENEMY_SMALL_TYPE_2_5)
				{
					if (Enemy::S_NUMBER_MED < MAX_ENEMY_MED)
					{
						enemy = Enemy::create(2, id, m_currentLevel.m_hp2, getVy(id));
					}
					else
					{
						continue;
					}
				}
				else
				{
					if (Enemy::S_NUMBER_BIG < MAX_ENEMY_BIG)
					{
						CCLOG("YES");
						enemy = Enemy::create(3, id, m_currentLevel.m_hp3, getVy(id));
					}
					else
					{
						CCLOG("NO");
						continue;
					}
				}

				m_currentLevel.m_arrEnemy[id]--;
				ObjectLayer* parent = (ObjectLayer*) this->getParent();
				parent->AddEmemy(enemy);

				break;
			}
		}
	}

	//check if reset pack
	int sum = 0;
	for (int i = 0; i < NUM_ENEMY_TYPE; ++i)
	{
		sum += m_currentLevel.m_arrEnemy[i];
	}

	if (sum == 0)
	{
		m_smallCounter = - m_currentLevel.m_bigDelay;
		m_currentLevel = m_savedLevel.Clone();
	}

	//check if need new pack

	if (score > m_currentLevel.m_score)
	{
		LevelData* newLd = LevelLoader::shareLevelLoader()->GetValueLowerThan(score);

		if (newLd != NULL && (*newLd).m_score > m_currentLevel.m_score)
		{
			m_currentLevel = (*newLd).Clone();
			m_savedLevel = (*newLd).Clone();
		}
	}
}

void EnemyFactory::Reset()
{
	m_smallCounter = 0;
	m_bigCounter = 0;

	m_currentLevel = (*LevelLoader::shareLevelLoader()->GetValueLowerThan(0)).Clone();
	m_savedLevel = m_currentLevel.Clone();
}

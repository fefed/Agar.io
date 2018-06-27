#pragma once

#include "cocos2d.h"
#include<vector>
#include "Client.h"

USING_NS_CC;
using namespace std;

class GameOl : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene(Client* client);

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	//menu item callback
	//void menuBackCallback(cocos2d::Ref* pSender);

	void menuPauseSceneCallback(cocos2d::Ref*pSender);

	//touch one by one event callback
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);

	//right mouse
	void mouseUp(Event* event);


	//move the view using schedule
	virtual void spriteFollowedView(float dt);
	Vec2 viewOffset;

	//change the view size using schedule
	virtual void viewFollowingPlayerScale(float dt);
	float viewScale = 1.0;// > 1.0

	//calculate center of the player's sprites using schedule
	void calCenter(float dt);
	Vec2 playerCenter;

	//used for initialize the view position
	//bool initViewPosition = true;


	//refresh playerScale
	void refreshPlayerScale(int plusOrMinus);

	//control player scale when it is too large
	void tooLargeScaleControl(float dt);


	//create little particles
	void createLittleParticles(int amount);
	void createParticlesByTime(float dt);

	//enable player to swallow particles using physics engine contact
	bool contactBegin(PhysicsContact& contact);


	void update(float dt);

	void sendInitPos(float dt);

	string formatPos(int pos)
	{
		string strPos;
		if (pos >= 0)
			strPos = "+";
		else
			strPos = "-";

		pos = abs(pos);
		if (pos >= 1000)
			strPos += to_string(pos);
		else if (pos >= 100)
			strPos += ("0" + to_string(pos));
		else if (pos >= 10)
			strPos += ("00" + to_string(pos));
		else
			strPos += ("000" + to_string(pos));

		strPos += "||";
		return strPos;
	}

	int getPosFromFmt(string order)
	{
		int result = 0;
		result += 1000 * (order[4] - '0');
		result += 100 * (order[5] - '0');
		result += 10 * (order[6] - '0');
		result += 1 * (order[7] - '0');
		if (order[3] = '-')
			result = -result;
		return result;
	}

	void moveMsg5(string& msg, int x, int y)
	{
		if (x > 0)
		{
			if (y > 0)
				msg += "1";
			else
				msg += "2";
		}
		else
		{
			if (y > 0)
				msg += "3";
			else
				msg += "4";
		}
	}

	vector<Sprite*> vecPlayerSprite;
	vector<Sprite*> aryMultiPlayerSprite[8];

	// implement the "static create()" method manually
	CREATE_FUNC(GameOl);
};

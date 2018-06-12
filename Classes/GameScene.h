#pragma once

#include "cocos2d.h"
#include<vector>

USING_NS_CC;
using namespace std;

class Game : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

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


	//refresh playerScale
	void refreshPlayerScale(int plusOrMinus);

	//control player scale when it is too large
	void tooLargeScaleControl(float dt);


	//create little particles
	void createLittleParticles(int amount);
	void createParticlesByTime(float dt);

	//enable player to swallow particles using physics engine contact
	bool contactBegin(PhysicsContact& contact);
	

	vector<Sprite*> vecPlayerSprite;	
	
	// implement the "static create()" method manually
	CREATE_FUNC(Game);
};

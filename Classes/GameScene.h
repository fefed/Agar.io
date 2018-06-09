#pragma once

#include "cocos2d.h"

USING_NS_CC;

class Game : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	//menu item callback
	void menuBackCallback(cocos2d::Ref* pSender);

	//touch one by one event callback
	bool touchBegan(Touch* touch, Event* event);
	void touchMoved(Touch* touch, Event* event);
	void touchEnded(Touch* touch, Event* event);

	//old version
	//move the view
	//void setViewPointCenter(Sprite* player, int kind_of_move_action, int if_x_is_minus, int if_y_is_minus);

	//move the view using schedule
	virtual void spriteFollowedView(float dt);

	//change the view size using schedule
	virtual void viewFollowingPlayerScale(float dt);

	//create little particles
	void createLittleParticles(int amount);

	bool contactBegin(PhysicsContact& contact);

	float playerScale = 1.0;
	float viewScale = 1.0;// > 1.0
	//int particlesEaten = 0;

	//refresh playerScale
	void refreshPlayerScale(int plusOrMinus);

	//control player scale when it is too large
	void tooLargeScaleControl(float dt);

	void createParticlesByTime(float dt);

	// implement the "static create()" method manually
	CREATE_FUNC(Game);
};

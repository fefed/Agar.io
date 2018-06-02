#pragma once

#include "cocos2d.h"

USING_NS_CC;

class Game : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();

	//menu item callback
	void menuBackCallback(cocos2d::Ref* pSender);

	//touch one by one event callback
	bool touchBegan(Touch* touch, Event* event);
	void touchMoved(Touch* touch, Event* event);
	void touchEnded(Touch* touch, Event* event);

	// implement the "static create()" method manually
	CREATE_FUNC(Game);
};

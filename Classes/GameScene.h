#pragma once

#include "cocos2d.h"

class Game : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	//menu item callback
	void menuBackCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(Game);
};

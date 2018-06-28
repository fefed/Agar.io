#pragma once

#include "cocos2d.h"

class End : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	//menu item callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(End);
};

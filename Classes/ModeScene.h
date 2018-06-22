#pragma once

#include "cocos2d.h"

class Mode : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	//menu item callback
	void menuBackCallback(cocos2d::Ref* pSender);

	void menuItemSingleCallback(cocos2d::Ref* pSender);
	void menuItemFreeCallback(cocos2d::Ref* pSender);
	void menuItemTeamCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(Mode);
};

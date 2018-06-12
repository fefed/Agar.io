#pragma once

#include "cocos2d.h"

class PauseScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// back to the game
	void menuContinueCallback(cocos2d::Ref* pSender);

	//back to the origional scene
	void menuOverGameCallback(cocos2d::Ref* pSender);

	//control the music
	void menuMusicCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(PauseScene);
};


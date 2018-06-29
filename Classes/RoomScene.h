#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class Room : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onExit();

	//menu item callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	//void menuItemReadyCallback(cocos2d::Ref* pSender);

	void menuItemCreateCallback(cocos2d::Ref* pSender);

	//void menuItemJoinCallback(cocos2d::Ref* pSender);

	void menuItemStartCallback(cocos2d::Ref* pSender);

	void search(float dt);
	void update(float dt);
	void startCheck(float dt);
	//void close(float dt);


	void textFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);

	void menuItemSendCallback(cocos2d::Ref* pSender);

	void handleMsg(std::string temp);

	std::string _chatString;
	cocos2d::ui::TextField* _chatWindow;
	int msgCnt = 0;

	// implement the "static create()" method manually
	CREATE_FUNC(Room);
};

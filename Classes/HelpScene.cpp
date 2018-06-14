#include "HelpScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Help::createScene()
{
	return Help::create();
}

bool Help::init()
{
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//background
	Sprite* backGround = Sprite::create("secondMenu/bgNoTitle.png");
	backGround->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(backGround, 0);

	//auto label = Label::createWithSystemFont("How To Play\nYou can control the ball\nby clicking left mouse button\nPress the space to pause", "Arial", 56);
	//label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	//this->addChild(label, 1);

	auto Introduction = Sprite::create("helpScene/helpScene.png");
	Introduction->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(Introduction, 1);

	//back button
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Help::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	Menu* mn = Menu::create(backMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	return true;
}

void Help::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

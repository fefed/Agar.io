#include "ModeScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "RoomScene.h"

USING_NS_CC;

Scene* Mode::createScene()
{
	return Mode::create();
}

bool Mode::init()
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

	//back button
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Mode::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38


	//single button 
	//size 350*200
	Sprite* singleSpriteNormal = Sprite::create("secondMenu/single.png");
	Sprite* singleSpriteSelected = Sprite::create("secondMenu/singleC.png");//C for chosen

	MenuItemSprite* singleMenuItem = MenuItemSprite::create(singleSpriteNormal, singleSpriteSelected,
		CC_CALLBACK_1(Mode::menuItemSingleCallback, this));
	singleMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200)));

	//free button
	MenuItemImage* freeMenuItem = MenuItemImage::create("secondMenu/free.png", "secondMenu/freeC.png",
		CC_CALLBACK_1(Mode::menuItemFreeCallback, this));
	freeMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2)));

	//team button
	MenuItemImage* teamMenuItem = MenuItemImage::create("secondMenu/team.png", "secondMenu/teamC.png",
		CC_CALLBACK_1(Mode::menuItemTeamCallback, this));
	teamMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 200)));


	Menu* mn = Menu::create(backMenuItem, singleMenuItem, freeMenuItem, teamMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	return true;
}

void Mode::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Mode::menuItemSingleCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);
}

void Mode::menuItemFreeCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();

	auto sc = Room::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->pushScene(reScene);
}

void Mode::menuItemTeamCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();

	auto sc = Room::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->pushScene(reScene);
}

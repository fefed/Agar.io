#include "OptionsScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Options::createScene()
{
	return Options::create();
}

bool Options::init()
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

	//sound toggle button
	auto soundOnMenuItem = MenuItemImage::create("secondMenu/soundOn.png", "secondMenu/soundOn.png");
	auto soundOffMenuItem = MenuItemImage::create("secondMenu/soundOff.png", "secondMenu/soundOff.png");
	auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Options::menuSoundToggleCallback, this),
		soundOnMenuItem, soundOffMenuItem, NULL);
	soundToggleMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2)));

	//back button
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Options::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	Menu* mn = Menu::create(soundToggleMenuItem, backMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	return true;
}

void Options::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

//audio control not completed...
void Options::menuSoundToggleCallback(cocos2d::Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	log("Touch Sound Toggle Menu Item %p", item);
}
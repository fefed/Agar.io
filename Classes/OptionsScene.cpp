#include "OptionsScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;
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
	//judge if the bgm is playing and creat the matched scene
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying() == true)
	{
		auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Options::menuSoundToggleCallback, this),
			soundOnMenuItem, soundOffMenuItem, NULL);
		soundToggleMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2)));
		auto menu = Menu::create(soundToggleMenuItem, NULL);
		menu->setPosition(Vec2::ZERO);
		this->addChild(menu, 0);
	}
	else
	{
		auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Options::menuSoundToggleCallback, this),
			soundOffMenuItem, soundOnMenuItem, NULL);
		soundToggleMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width / 2, visibleSize.height / 2)));
		auto menu1 = Menu::create(soundToggleMenuItem, NULL);
		menu1->setPosition(Vec2::ZERO);
		this->addChild(menu1, 0);
	}
	//back button
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
	    CC_CALLBACK_1(Options::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38
	auto menu2 = Menu::create(backMenuItem, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 0);
	return true;
}

void Options::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Options::menuSoundToggleCallback(cocos2d::Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying() == true)
	{
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
	else
	{
		SimpleAudioEngine::getInstance()->playBackgroundMusic("music/teat2.mp3", true);

	}
	log("Touch Sound Toggle Menu Item %p", item);
}
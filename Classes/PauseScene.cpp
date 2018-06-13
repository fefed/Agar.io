#include "PauseScene.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;
Scene* PauseScene::createScene()
{
	return PauseScene::create();
}

bool PauseScene::init()
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

	//dismiss dispatchers
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();


	//sound toggle button
	auto soundOnMenuItem = MenuItemImage::create("secondMenu/soundOn.png", "secondMenu/soundOn.png");
	auto soundOffMenuItem = MenuItemImage::create("secondMenu/soundOff.png", "secondMenu/soundOff.png");

	//judge if the bgm is playing and creat the matched scene
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying() == true)
	{
		auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(PauseScene::menuMusicCallback, this),
			soundOnMenuItem, soundOffMenuItem, NULL);
		soundToggleMenuItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 200));
		auto menu = Menu::create(soundToggleMenuItem, NULL);
		menu->setPosition(Vec2::ZERO);
		this->addChild(menu, 0);
	}
	else
	{
		auto soundToggleMenuItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(PauseScene::menuMusicCallback, this),
			soundOffMenuItem, soundOnMenuItem, NULL);
		soundToggleMenuItem->setPosition(Vec2(visibleSize.width / 2, 200 + visibleSize.height / 2));
		auto menu1 = Menu::create(soundToggleMenuItem, NULL);
		menu1->setPosition(Vec2::ZERO);
		this->addChild(menu1, 0);
	}


	MenuItemFont::setFontName("Times New Roman");
	MenuItemFont::setFontSize(86);

	//back to the game
	auto ContinueItem = MenuItemFont::create("Continue", CC_CALLBACK_1(PauseScene::menuContinueCallback, this));
	ContinueItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	//over the game
	auto EndItem = MenuItemFont::create("Over", CC_CALLBACK_1(PauseScene::menuOverGameCallback, this));
	EndItem->setPosition(Vec2(visibleSize.width / 2, (visibleSize.height / 2) - 200));

	auto mn = Menu::create(ContinueItem, EndItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	return true;
}

// back to the game
void PauseScene::menuContinueCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

// control the music
void PauseScene::menuMusicCallback(cocos2d::Ref* pSender)
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

// end the game
void PauseScene::menuOverGameCallback(cocos2d::Ref*pSender)
{
	Director::getInstance()->popScene();

	auto sc = HelloWorld::create();
	Director::getInstance()->replaceScene(sc);
}
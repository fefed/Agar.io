#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Game::createScene()
{
	return Game::create();
}

bool Game::init()
{
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backGround = LayerColor::create(Color4B(100, 100, 100, 255));
	this->addChild(backGround);

	//back button for the convenience of testing
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Game::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	Menu* mn = Menu::create(backMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	auto playerPInfo = AutoPolygon::generatePolygon("game/pinkPlayer.png");
	auto player = Sprite::create(playerPInfo);
	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(player);

	//to be compeleted...

	return true;
}

void Game::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

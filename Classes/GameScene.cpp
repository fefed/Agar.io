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

	//tile map
	auto backGround = Sprite::create("game/bgTile.png", Rect(0, 0, visibleSize.width, visibleSize.height));
	Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	backGround->getTexture()->setTexParameters(tp);
	backGround->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	this->addChild(backGround, 0);

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

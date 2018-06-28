#include "EndScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* End::createScene()
{
	return End::create();
}

bool End::init()
{
	if (!Scene::init())
	{
		return false;
	}

	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//background
	Sprite* backGround = Sprite::create("game/end.png");
	backGround->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(backGround, 0);


	//exit
	auto closeItem = MenuItemImage::create(
		"startScreen/exitNormal.png",
		"startScreen/exitChosen.png",
		CC_CALLBACK_1(End::menuCloseCallback, this));

	float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
	float y = origin.y + closeItem->getContentSize().height / 2;
	closeItem->setPosition(Vec2(x, y));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);


	return true;
}

void End::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "startScreen/exitNormal.png",
                                           "startScreen/exitChosen.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'startScreen/exitNormal.png' and 'startScreen/exitChosen.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

	//background
	auto backGround = Sprite::create("startScreen/bg.png");
	backGround->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(backGround, 0);

	//start button 
	//size 250*80
	Sprite* startSpriteNormal = Sprite::create("startScreen/startNormal.png");
	Sprite* startSpriteSelected = Sprite::create("startScreen/startChosen.png");

	MenuItemSprite* startMenuItem = MenuItemSprite::create(startSpriteNormal, startSpriteSelected, 
		CC_CALLBACK_1(HelloWorld::menuItemStartCallback, this));
	startMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(512, 384)));
	
	//Help button
	MenuItemImage* helpMenuItem = MenuItemImage::create("startScreen/helpNormal.png", "startScreen/helpChosen.png", 
		CC_CALLBACK_1(HelloWorld::menuItemHelpCallback, this));
	helpMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(512, 384 + 120)));

	//Options button
	MenuItemImage* optionsMenuItem = MenuItemImage::create("startScreen/optionsNormal.png", "startScreen/optionsChose.png", //Chose instead of Chosen for a strange bug
		CC_CALLBACK_1(HelloWorld::menuItemOptionsCallback, this));
	optionsMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(512, 384 + 2 * 120)));

	Menu* mu = Menu::create(startMenuItem, helpMenuItem, optionsMenuItem, NULL);
	mu->setPosition(Vec2::ZERO);
	this->addChild(mu);

   /* auto label = Label::createWithTTF("Ball Ball Big Fight", "fonts/Arial.ttf", 48);
    if (label == nullptr)
    {
        problemLoading("'fonts/Arial.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - 2 * label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }



    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }*/
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::menuItemStartCallback(Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	log("Touch Start Menu Item %p", item);
}

void HelloWorld::menuItemHelpCallback(Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	log("Touch Help Menu Item %p", item);
}

void HelloWorld::menuItemOptionsCallback(Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	log("Touch Options Menu Item %p", item);
}

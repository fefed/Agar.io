#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include <time.h>

#define PLAYER_SPRITE_TAG 0
#define MOVE_ACTION_1 1
#define MOVE_ACTION_2 2
#define MOVE_ACTION_3 3
#define SPEED 300000
#define MAP_WIDTH_TIMES 10
#define MAP_HEIGHT_TIMES 10
#define INIT_PARTICLE_NUM 5000

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
	auto backGround = Sprite::create("game/bgTile.png", Rect(0, 0, 
		MAP_WIDTH_TIMES * visibleSize.width, MAP_HEIGHT_TIMES * visibleSize.height));//map size
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

	//create player sprite using polygen sprite
	auto playerPInfo = AutoPolygon::generatePolygon("game/pinkPlayer.png");
	auto player = Sprite::create(playerPInfo);
	player->setTag(PLAYER_SPRITE_TAG);
	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(player, 2);

	//create little particles when initialize
	for (int amount = INIT_PARTICLE_NUM; amount > 0; amount--)
		createLittleParticle();

	//test sign for (0,0)
	auto test = Sprite::create("HelloWorld.png");
	test->setPosition(Vec2(0, 0));
	this->addChild(test, 0);

	//to be compeleted...

	return true;
}

void Game::menuBackCallback(cocos2d::Ref* pSender)
{
	//back to menu for the convenience of test
	Director::getInstance()->popScene();//but not work now
}


void Game::onEnter()
{
	Scene::onEnter();
	log("GameScene onEnter");

	auto listener = EventListenerTouchOneByOne::create();

	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Game::touchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Game::touchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Game::touchEnded, this);

	EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, getChildByTag(PLAYER_SPRITE_TAG));
}


//move by touch

int previous_if_x_is_minus, previous_if_y_is_minus, previous_kind_of_move_action;//record previous action types
clock_t moveStartTime = clock();//record time to control the interval of calling the function touchMoved

bool Game::touchBegan(Touch* touch, Event* event)
{
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	Vec2 locationInNode = target->convertToNodeSpaceAR(touch->getLocation());

	//8 directions instead of any directions to move smoothly
	float tan = abs(locationInNode.y / locationInNode.x);
	previous_if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;//the first direction type, so we use previous_... in file scope
	previous_if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;//to compare with the next

	auto move1 = MoveBy::create(1000, Vec2(0, SPEED * previous_if_y_is_minus));//1000 seconds until touchMoved or touchEnded
	auto move2 = MoveBy::create(1000, Vec2(0.707 * SPEED * previous_if_x_is_minus, 0.707 * SPEED * previous_if_y_is_minus));
	auto move3 = MoveBy::create(1000, Vec2(SPEED * previous_if_x_is_minus, 0));
	move1->setTag(MOVE_ACTION_1);//set tag for actions to stop them in the following function
	move2->setTag(MOVE_ACTION_2);
	move3->setTag(MOVE_ACTION_3);

	if (tan > 2.4142)//tan 77.5
	{
		target->runAction(move1);
		previous_kind_of_move_action = 1;
	}
	else if (tan > 0.4142)//tan 22.5
	{
		target->runAction(move2);
		previous_kind_of_move_action = 2;
	}
	else
	{
		target->runAction(move3);
		previous_kind_of_move_action = 3;
	}

	//old version
	//get sprite position to move the view
	//setViewPointCenter(target, previous_kind_of_move_action, previous_if_x_is_minus, previous_if_y_is_minus);

	//call function using schedule
	this->schedule(schedule_selector(Game::spriteFollowingView), 1.0 / 60.0);

	return true;
}

void Game::touchMoved(Touch * touch, Event * event)
{
	//Function calling inteval(ms)
	//if the interval is too short, funtion calling too frequent, the sprite cannot move or change directions smoothly
	if (clock() - moveStartTime > 100)
	{
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		Vec2 locationInNode = target->convertToNodeSpaceAR(touch->getLocation());

		float tan = abs(locationInNode.y / locationInNode.x);
		int if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;//maybe a new diretion, so we use them without previous_...
		int if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;

		bool if_move_action_is_same = true;
		int kind_of_move_action;

		if ((if_x_is_minus == previous_if_x_is_minus) && (if_y_is_minus == previous_if_y_is_minus))
		{
			if (tan > 2.4142)
			{
				if (previous_kind_of_move_action == 1);//the direction is the same as it is previously, so we do nothing
				else
				{
					if_move_action_is_same = false;
					kind_of_move_action = 1;
				}
			}
			else if (tan > 0.4142)
			{
				if (previous_kind_of_move_action == 2);
				else
				{
					if_move_action_is_same = false;
					kind_of_move_action = 2;
				}
			}
			else
			{
				if (previous_kind_of_move_action == 3);
				else
				{
					if_move_action_is_same = false;
					kind_of_move_action = 3;
				}
			}
		}
		else
		{
			if_move_action_is_same = false;

			if (tan > 2.4142)
				kind_of_move_action = 1;
			else if (tan > 0.4142)
				kind_of_move_action = 2;
			else
				kind_of_move_action = 3;
		}

		if (!if_move_action_is_same)
		{
			auto move1 = MoveBy::create(1000, Vec2(0, SPEED * if_y_is_minus));
			auto move2 = MoveBy::create(1000, Vec2(0.707 * SPEED * if_x_is_minus, 0.707 * SPEED * if_y_is_minus));
			auto move3 = MoveBy::create(1000, Vec2(SPEED * if_x_is_minus, 0));
			move1->setTag(MOVE_ACTION_1);//tag for stopping
			move2->setTag(MOVE_ACTION_2);
			move3->setTag(MOVE_ACTION_3);

			target->stopActionByTag(previous_kind_of_move_action);//since direction should change, the previous action stops
			previous_kind_of_move_action = kind_of_move_action;//new direction, record this action type to compare with the next
			if (previous_kind_of_move_action == 1)
				target->runAction(move1);
			else if (previous_kind_of_move_action == 2)
				target->runAction(move2);
			else
				target->runAction(move3);
		}

		//record time the function ends to control the intervals between two callings
		moveStartTime = clock();
	}
}

void Game::touchEnded(Touch * touch, Event * event)
{
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	target->stopActionByTag(previous_kind_of_move_action);//when touch ended, just stop
	unschedule(schedule_selector(Game::spriteFollowingView));
}


//move the view using schedule
void Game::spriteFollowingView(float dt)
{
	Sprite* player = (Sprite*)getChildByTag(PLAYER_SPRITE_TAG);
	Vec2 position = player->getPosition();
	Size visibleSize = Director::getInstance()->getVisibleSize();

	int x = MAX(position.x, (1 - MAP_WIDTH_TIMES) / 2 * visibleSize.width);
	int y = MAX(position.y, (1 - MAP_HEIGHT_TIMES) / 2 * visibleSize.height);
	x = MIN(x, (MAP_WIDTH_TIMES - 0.5) * visibleSize.width);
	y = MIN(y, (MAP_HEIGHT_TIMES - 0.5) * visibleSize.height);
	
	Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);
	Vec2 pointB = Vec2(x, y);
	log("target position (%f, %f)", pointB.x, pointB.y);

	Vec2 offset = pointA - pointB;

	log("offset (%f, %f)", offset.x, offset.y);
	this->setPosition(offset);
}


//Create little particles
void Game::createLittleParticle()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto particlePInfo = AutoPolygon::generatePolygon("game/littleParticle.png");
	auto littleParticle = Sprite::create(particlePInfo);

	float posX = (MAP_WIDTH_TIMES * visibleSize.width - 2 * 200) * CCRANDOM_0_1() - MAP_WIDTH_TIMES * visibleSize.width / 2;
	float posY = (MAP_HEIGHT_TIMES * visibleSize.height - 2 * 100) * CCRANDOM_0_1() - MAP_HEIGHT_TIMES * visibleSize.height / 2;
	littleParticle->setPosition(Vec2(posX, posY));
	
	int colorType = (int)(6 * CCRANDOM_0_1()) % 6;
	switch (colorType)
	{
	case 0:
		littleParticle->setColor(Color3B(255, 247, 153));//yellow preset
		break;
	case 1:
		littleParticle->setColor(Color3B(230, 18, 0));//red preset
		break;
	case 2:
		littleParticle->setColor(Color3B(153, 195, 31));//green preset
		break;
	case 3:
		littleParticle->setColor(Color3B(0, 183, 238));//blue preset
		break;
	case 4:
		littleParticle->setColor(Color3B(228, 0, 127));//purple preset
		break;
	case 5:
		littleParticle->setColor(Color3B(255, 255, 255));//white preset
		break;
	}

	this->addChild(littleParticle, 1);
}

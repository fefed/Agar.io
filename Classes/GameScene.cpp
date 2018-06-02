#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include <time.h>

#define PLAYER_SPRITE_TAG 0
#define MOVE_ACTION_1 1
#define MOVE_ACTION_2 2
#define MOVE_ACTION_3 3
#define SPEED 300000

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
	auto backGround = Sprite::create("game/bgTile.png", Rect(0, 0, 10 * visibleSize.width, 10 * visibleSize.height));
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

	//player sprite
	auto playerPInfo = AutoPolygon::generatePolygon("game/pinkPlayer.png");
	auto player = Sprite::create(playerPInfo);
	player->setTag(PLAYER_SPRITE_TAG);
	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(player);

	//to be compeleted...

	return true;
}

void Game::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
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

int previous_if_x_is_minus, previous_if_y_is_minus, previous_kind_of_move_action;
clock_t startTime = clock();

bool Game::touchBegan(Touch* touch, Event* event)
{
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());

	float tan = abs(locationInNode.y / locationInNode.x);
	previous_if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;
	previous_if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;

	auto move1 = MoveBy::create(1000, Vec2(0, SPEED * previous_if_y_is_minus));
	auto move2 = MoveBy::create(1000, Vec2(SPEED * previous_if_x_is_minus, SPEED * previous_if_y_is_minus));
	auto move3 = MoveBy::create(1000, Vec2(SPEED * previous_if_x_is_minus, 0));
	move1->setTag(MOVE_ACTION_1);
	move2->setTag(MOVE_ACTION_2);
	move3->setTag(MOVE_ACTION_3);

	if (tan > 2.4142)
	{
		target->runAction(move1);
		previous_kind_of_move_action = 1;
	}
	else if (tan > 0.4142)
	{
		target->runAction(move2);
		previous_kind_of_move_action = 2;
	}
	else
	{
		target->runAction(move3);
		previous_kind_of_move_action = 3;
	}

	/*float distance = sqrt(locationInNode.x * locationInNode.x + locationInNode.y * locationInNode.y);
	auto move = MoveBy::create(1.0 / 60.0, Vec2(SPEED * locationInNode.x / distance, SPEED * locationInNode.y / distance));
	auto delay = DelayTime::create(0.03);

	while (true)
	{
		target->runAction(Sequence::create(move, delay, nullptr));
		Sleep(3);
	}*/

	return true;
}

void Game::touchMoved(Touch * touch, Event * event)
{
	if (clock() - startTime > 200)//Function calling inteval
	{
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());

		float tan = abs(locationInNode.y / locationInNode.x);
		int if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;
		int if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;

		bool if_move_action_is_same = true;
		int kind_of_move_action;

		if ((if_x_is_minus == previous_if_x_is_minus) && (if_y_is_minus == previous_if_y_is_minus))
		{
			if (tan > 2.4142)
			{
				if (previous_kind_of_move_action == 1);
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
			auto move2 = MoveBy::create(1000, Vec2(SPEED * if_x_is_minus, SPEED * if_y_is_minus));
			auto move3 = MoveBy::create(1000, Vec2(SPEED * if_x_is_minus, 0));
			move1->setTag(MOVE_ACTION_1);
			move2->setTag(MOVE_ACTION_2);
			move3->setTag(MOVE_ACTION_3);

			target->stopActionByTag(previous_kind_of_move_action);
			previous_kind_of_move_action = kind_of_move_action;
			if (previous_kind_of_move_action == 1)
				target->runAction(move1);
			else if (previous_kind_of_move_action == 2)
				target->runAction(move2);
			else
				target->runAction(move3);
		}

		startTime = clock();

		/*float tan = locationInNode.y / locationInNode.x;
		if (tan >= 0 && tan <= 0.414)
		{
			target->runAction
		}
		auto move = Place::create(target->convertToNodeSpace(Vec2(SPEED * locationInNode.x / distance, SPEED * locationInNode.y / distance)));
		auto delay = DelayTime::create(0.03);

		while (true)
		{
			target->runAction(Sequence::create(move, delay, nullptr));
			Sleep(3);
		}*/

		/*Action* move = MoveBy::create(sqrt(locationInNode.x * locationInNode.x + locationInNode.y * locationInNode.y) / 1000,
			Vec2(locationInNode.x, locationInNode.y));
		move->setTag(MOVE_ACTION_TAG);
		target->stopActionByTag(MOVE_ACTION_TAG);
		target->runAction(move);*/
	}
}

void Game::touchEnded(Touch * touch, Event * event)
{
	auto target = static_cast<Sprite*>(event->getCurrentTarget());
	Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
	target->stopActionByTag(previous_kind_of_move_action);
}

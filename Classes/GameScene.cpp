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
#define INIT_PARTICLE_NUM 2000
#define LITTLE_PARTICLE_TAG 4
#define EDGE_NODE_TAG 5
#define CONTACT_TAG 6

USING_NS_CC;

Scene* Game::createScene()
{
	return Game::create();
}

bool Game::init()
{
	//initialize using physics engine
	if (!Scene::initWithPhysics())
	{
		return false;
	}

	//Game::getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	//Game::getPhysicsWorld()->setAutoStep(false);
	//Game::getPhysicsWorld()->step(1.0 / 180.0);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//world edge collision
	Size worldSize(MAP_WIDTH_TIMES * visibleSize.width, MAP_HEIGHT_TIMES * visibleSize.height);
	auto worldBody = PhysicsBody::createEdgeBox(worldSize, PHYSICSBODY_MATERIAL_DEFAULT, 50.0);
	worldBody->setDynamic(false);

	auto edgeNode = Node::create();
	edgeNode->setTag(EDGE_NODE_TAG);
	edgeNode->setPosition(Vec2(0, 0));
	edgeNode->setPhysicsBody(worldBody);
	this->addChild(edgeNode);


	//tile map
	auto backGround = Sprite::create("game/greyTile.png", Rect(0, 0,
		(MAP_WIDTH_TIMES + 4) * visibleSize.width, (MAP_HEIGHT_TIMES + 4) * visibleSize.height));//map size, +4 for avoiding the black background edge bug
	Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	backGround->getTexture()->setTexParameters(tp);
	backGround->setPosition(origin.x, origin.y);
	this->addChild(backGround, 0);


	//back button for the convenience of testing
	//not work now
	/*auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Game::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	Menu* mn = Menu::create(backMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);*/


	//create player sprite using polygen sprite
	//auto playerPInfo = AutoPolygon::generatePolygon("game/player50x50.png");
	//auto player = Sprite::create(playerPInfo);
	auto player = Sprite::create("game/player50x50.png");
	player->setTag(PLAYER_SPRITE_TAG);

	auto playerBody = PhysicsBody::createCircle(player->getContentSize().width / 4);
	playerBody->setGravityEnable(false);
	playerBody->setContactTestBitmask(0x03);//0011
	playerBody->setCollisionBitmask(0x01);
	player->setPhysicsBody(playerBody);

	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));//player initial position, to be completed
	this->addChild(player, 2);

	vecPlayerSprite.push_back(player);


	//create little particles when initialize
	createLittleParticles(INIT_PARTICLE_NUM);

	//test sign for (0,0)
	auto test = Sprite::create("HelloWorld.png");
	test->setPosition(Vec2(0, 0));
	this->addChild(test, 0);

	return true;
}

/*void Game::menuBackCallback(cocos2d::Ref* pSender)
{
	//back to menu for the convenience of test
	Director::getInstance()->popScene();//but not work now
}*/


void Game::onEnter()
{
	Scene::onEnter();
	log("GameScene onEnter");

	
	//Touch listener
	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(Game::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Game::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Game::onTouchEnded, this);

	EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, getChildByTag(PLAYER_SPRITE_TAG));//? 


	//contact listener in physics engine
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Game::contactBegin, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(contactListener, 1);


	//Mouse listener
	//release right mouse button to divide
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseUp = CC_CALLBACK_1(Game::mouseUp, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);


	//call function that changes view size using schedule
	this->schedule(schedule_selector(Game::viewFollowingPlayerScale), 1.0 / 60.0);
	
	//call function that contorls player scale when it is too large
	this->schedule(schedule_selector(Game::tooLargeScaleControl), 6.0 / 60.0);

	//call function to create little particles
	this->schedule(schedule_selector(Game::createParticlesByTime), 2.0);

	//call function to calculate center
	this->schedule(schedule_selector(Game::calCenter), 1.0 / 60.0);
}

void Game::onExit()
{
	Scene::onExit();
	log("GameScene onExit");

	unschedule(schedule_selector(Game::tooLargeScaleControl));
	unschedule(schedule_selector(Game::createParticlesByTime));
	unschedule(schedule_selector(Game::calCenter));
	
	//dismiss dispatchers?
}


//move by touch
int previous_if_x_is_minus, previous_if_y_is_minus, previous_kind_of_move_action;//record previous action types
clock_t moveStartTime = clock();//record time to control the interval of calling the function touchMoved
bool if_is_moving = false;

bool Game::onTouchBegan(Touch * touch, Event * event)
{
	Vec2 locationInNode = touch->getLocation() - viewOffset - playerCenter;
	//log("touch %f %f", touch->getLocation().x, touch->getLocation().y);
	//log("player %f %f", playerCenter.x, playerCenter.y);
	//log("offset %f %f", locationInNode.x, locationInNode.y);

	//8 directions instead of any directions to move smoothly
	float tan = abs(locationInNode.y / locationInNode.x);
	previous_if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;//the first direction type, so we use previous_... in file scope
	previous_if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;//to compare with the next

	for (int i = 0; i < vecPlayerSprite.size(); i++)
	{
		auto move1 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),//1000 seconds until touchMoved or touchEnded
			Vec2(0, SPEED * previous_if_y_is_minus));
		auto move2 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
			Vec2(0.707 * SPEED * previous_if_x_is_minus, 0.707 * SPEED * previous_if_y_is_minus));
		auto move3 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
			Vec2(SPEED * previous_if_x_is_minus, 0));
		move1->setTag(MOVE_ACTION_1);//set tag for actions to stop them in the following function
		move2->setTag(MOVE_ACTION_2);
		move3->setTag(MOVE_ACTION_3);

		if (tan > 2.4142)//tan 77.5
		{
			vecPlayerSprite[i]->runAction(move1);
			previous_kind_of_move_action = 1;
		}
		else if (tan > 0.4142)//tan 22.5
		{
			vecPlayerSprite[i]->runAction(move2);
			previous_kind_of_move_action = 2;
		}
		else
		{
			vecPlayerSprite[i]->runAction(move3);
			previous_kind_of_move_action = 3;
		}
	}

	//call function that changes view position using schedule
	this->schedule(schedule_selector(Game::spriteFollowedView), 1.0 / 60.0);
	
	if_is_moving = true;

	return true;
}

void Game::onTouchMoved(Touch * touch, Event * event)
{	
	//Function calling inteval(ms)
	//if the interval is too short, funtion calling too frequent, the sprite cannot move or change directions smoothly
	if (clock() - moveStartTime > 100)
	{
		Vec2 locationInNode = touch->getLocation() - viewOffset - playerCenter;
		//log("touch location: (%f,%f)", touch->getLocation().x, touch->getLocation().y);
		//log("player center: (%f,%f)", playerCenter.x, playerCenter.y);
		//log("offset: (%f,%f)", locationInNode.x, locationInNode.y);
		//log("%f %f", locationInNode.x, locationInNode.y);

		float tan = abs(locationInNode.y / locationInNode.x);
		int if_x_is_minus = (locationInNode.x > 0) ? 1 : -1;//maybe a new diretion, so we use them without previous_...
		int if_y_is_minus = (locationInNode.y > 0) ? 1 : -1;
		//log("tan: %f, x: %d, y: %d", tan, if_x_is_minus, if_y_is_minus);

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
			for (int i = 0; i < vecPlayerSprite.size(); i++)
			{
				auto move1 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
					Vec2(0, SPEED * if_y_is_minus));
				auto move2 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
					Vec2(0.707 * SPEED * if_x_is_minus, 0.707 * SPEED * if_y_is_minus));
				auto move3 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
					Vec2(SPEED * if_x_is_minus, 0));
				move1->setTag(MOVE_ACTION_1);//tag for stopping
				move2->setTag(MOVE_ACTION_2);
				move3->setTag(MOVE_ACTION_3);
			
				vecPlayerSprite[i]->stopAllActions();
				//vecPlayerSprite[i]->stopActionByTag(previous_kind_of_move_action);//since direction should change, the previous action stops
				previous_kind_of_move_action = kind_of_move_action;//new direction, record this action type to compare with the next
				previous_if_x_is_minus = if_x_is_minus;
				previous_if_y_is_minus = if_y_is_minus;

				if (previous_kind_of_move_action == 1)
					vecPlayerSprite[i]->runAction(move1);
				else if (previous_kind_of_move_action == 2)
					vecPlayerSprite[i]->runAction(move2);
				else
					vecPlayerSprite[i]->runAction(move3);
				//log("move%d", kind_of_move_action);
			}
		}

		//record time the function ends to control the intervals between two callings
		moveStartTime = clock();
	}
}

void Game::onTouchEnded(Touch * touch, Event * event)
{
	for (int i = 0; i < vecPlayerSprite.size(); i++)
	{
		vecPlayerSprite[i]->stopAllActions();
		//vecPlayerSprite[i]->stopActionByTag(previous_kind_of_move_action);

		//when touch ended, stop slowly
		//not completed...
		/*ActionInterval* move;
		if (previous_kind_of_move_action == 1)
		move = MoveBy::create(0.5, Vec2(0, 2000 * SPEED * previous_if_y_is_minus));
		else if (previous_kind_of_move_action == 2)
		move = MoveBy::create(0.5, Vec2(2000 * 0.707 * SPEED * previous_if_x_is_minus, 2000 * 0.707 * SPEED * previous_if_y_is_minus));
		else
		move = MoveBy::create(0.5, Vec2(2000 * SPEED * previous_if_x_is_minus, 0));

		target->runAction(EaseOut::create(move, 3));*/

		unschedule(schedule_selector(Game::spriteFollowedView));

		if_is_moving = false;
	}
}


//when release right mouse button, divide
void Game::mouseUp(Event* event)
{
	EventMouse* e = (EventMouse*)event;
	if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
	{
		log("Mouse up");
		int playerSpriteNum = vecPlayerSprite.size();
		for (int i = 0; i < playerSpriteNum; i++)
		{
			if (vecPlayerSprite[i]->getScale() > 1.5) 
			{
				vecPlayerSprite[i]->setScale(0.6 * vecPlayerSprite[i]->getScale());//0.707? 

				auto player = Sprite::create("game/player50x50.png");
				player->setColor(Color3B(0, 0, 0));
				player->setTag(PLAYER_SPRITE_TAG);

				auto playerBody = PhysicsBody::createCircle(player->getContentSize().width / 4);
				playerBody->setGravityEnable(false);
				playerBody->setContactTestBitmask(0x03);//0011
				playerBody->setCollisionBitmask(0x01);
				player->setPhysicsBody(playerBody);

				player->setScale(vecPlayerSprite[i]->getScale());
				this->addChild(player);

				//set the new sprite's position
				//and make it move(if others are moving)
				//and adjust the speed of the original sprite
				log("moving%d", if_is_moving);
				//if_is_moving = false;
				if (previous_kind_of_move_action == 1)
				{
					player->setPosition(Vec2(vecPlayerSprite[i]->getPosition().x,
						vecPlayerSprite[i]->getPosition().y 
						+ 50 * vecPlayerSprite[i]->getScale() * previous_if_y_is_minus));//50 maybe need to be changed
					if (if_is_moving)
					{
						//vecPlayerSprite[i]->stopAllActions();

						//needs to be changed

						auto move1 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
							Vec2(0, SPEED * previous_if_y_is_minus));
						move1->setTag(MOVE_ACTION_1);

						vecPlayerSprite[i]->stopAllActions();
						vecPlayerSprite[i]->runAction(move1);

						player->runAction(move1->clone());
						log("move1 x%d y%d", previous_if_x_is_minus, previous_if_y_is_minus);
					}
				}
				else if (previous_kind_of_move_action == 2)
				{
					player->setPosition(Vec2(vecPlayerSprite[i]->getPosition().x 
						+ 50 * vecPlayerSprite[i]->getScale() * previous_if_x_is_minus,
						vecPlayerSprite[i]->getPosition().y 
						+ 50 * vecPlayerSprite[i]->getScale()* previous_if_y_is_minus));
					if (if_is_moving)
					{
						//vecPlayerSprite[i]->stopAllActions();

						auto move2 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
							Vec2(0.707 * SPEED * previous_if_x_is_minus, 0.707 * SPEED * previous_if_y_is_minus));
						move2->setTag(MOVE_ACTION_2);

						vecPlayerSprite[i]->stopAllActions();
						vecPlayerSprite[i]->runAction(move2);

						player->runAction(move2->clone());
						log("move2 x%d y%d", previous_if_x_is_minus, previous_if_y_is_minus);
					}
				}
				else
				{
					player->setPosition(Vec2(vecPlayerSprite[i]->getPosition().x 
						+ 50 * vecPlayerSprite[i]->getScale() * previous_if_x_is_minus,
						vecPlayerSprite[i]->getPosition().y));
					if (if_is_moving)
					{
						//vecPlayerSprite[i]->stopAllActions();

						auto move3 = MoveBy::create(1000 * vecPlayerSprite[i]->getScale(),
							Vec2(SPEED * previous_if_x_is_minus, 0));
						move3->setTag(MOVE_ACTION_3);

						vecPlayerSprite[i]->stopAllActions();
						vecPlayerSprite[i]->runAction(move3);

						player->runAction(move3->clone());
						log("move3 x%d y%d", previous_if_x_is_minus, previous_if_y_is_minus);
					}
				}
				
				//log("original %f %f", vecPlayerSprite[i]->getPosition().x, vecPlayerSprite[i]->getPosition().y);
				//log("x %d, y %d, move %d", previous_if_x_is_minus, previous_if_y_is_minus, previous_kind_of_move_action);

				vecPlayerSprite.push_back(player);
			}
		}
	}
}


//move the view using schedule
void Game::spriteFollowedView(float dt)
{
	Vec2 position = playerCenter;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//log("sprite position (%f, %f)", position.x, position.y);

	position.x /= viewScale;
	position.y /= viewScale;
	visibleSize.width /= viewScale;
	visibleSize.height /= viewScale;

	//view center position control near the edges
	int x = (position.x < 0) ? MAX(position.x, -(MAP_WIDTH_TIMES / 2 - 0.5 * viewScale) * visibleSize.width) : position.x;
	int y = (position.y < 0) ? MAX(position.y, -(MAP_HEIGHT_TIMES / 2 - 0.5 * viewScale) * visibleSize.height) : position.y;
	x = MIN(x, (MAP_WIDTH_TIMES / 2 - 0.5 * viewScale) * visibleSize.width);
	y = MIN(y, (MAP_HEIGHT_TIMES / 2 - 0.5 * viewScale) * visibleSize.height);

	Vec2 pointA = Vec2(visibleSize.width / 2, visibleSize.height / 2);//linked to player born location, not completed
	Vec2 pointB = Vec2(x, y);
	//log("viewScale: %f", viewScale);
	//log("target position (%f, %f)", pointB.x, pointB.y);

	viewOffset = pointA - pointB;

	//log("offset (%f, %f)", offset.x, offset.y);
	this->setPosition(viewOffset);
}

//change the view size using schedule
void Game::viewFollowingPlayerScale(float dt)
{
	int playerSpriteNum = vecPlayerSprite.size();
	float playerScale = vecPlayerSprite[0]->getScale();
	for (int i = 1; i < playerSpriteNum; i++)
	{
		if (vecPlayerSprite[i]->getScale() > playerScale)
			playerScale = vecPlayerSprite[i]->getScale();
	}
	playerScale *= pow((float)playerSpriteNum, 3);//math function needs to be changed


	Game::viewScale = pow(0.95 + playerSpriteNum * 0.05, 1.0 / 4.0);
	this->setScale(1.0 / viewScale);
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto node = this->getChildByTag(EDGE_NODE_TAG);
	node->setScale(1.0 / viewScale);
	//log("visibleSize (%f, %f)", visibleSize.width, visibleSize.height);
}

void Game::calCenter(float dt)
{
	int spriteNum = vecPlayerSprite.size();
	float x = 0.0, y = 0.0;
	for (int i = 0; i < spriteNum; i++)
	{
		x += vecPlayerSprite[i]->getPosition().x;
		y += vecPlayerSprite[i]->getPosition().y;
	}
	playerCenter.x = x / spriteNum;
	playerCenter.y = y / spriteNum;
}


//refresh playerScale
//now only suitable for one sprite
void Game::refreshPlayerScale(int plusOrMinus)
{
	auto player = (Sprite*)getChildByTag(CONTACT_TAG);
	float playerScale = player->getScale();

	if (playerScale < 4.18 || plusOrMinus < 0)
	{
		//particlesEaten += plusOrMinus;

		//this function: when playerScale is near 4.1799-4.1800, it will hardly increase
		//when playerScale >= 4.1800(through eating others), this function will make playerScale decrease
		//so when playerScale >= 4.18, just make it unable to increase through eating little particles,
		//but decrease with time passing by through calling tooLargeScaleControl
		playerScale = pow(playerScale + 0.03, 1.0 / 1.005);
		player->setScale(playerScale);//player scale changes
	}
}

//control player scale when it is too large using schedule
void Game::tooLargeScaleControl(float dt)
{
	//if player scale too large, decrease with time passing
	//refer to function refreshPlayerScale()
	for (int i = 0; i < vecPlayerSprite.size(); i++)
	{
		float playerScale = vecPlayerSprite[i]->getScale();
		if (playerScale >= 4.8)
		{
			refreshPlayerScale(-1);
			log("scale decrease");
		}
	}
}


//Create little particles
void Game::createLittleParticles(int particleAmount)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//using SpriteBatchNode to optimize
	//but this can be replaced by normal sprite::create() because it is automatically applied after cocos2dx 3.0 beta
	SpriteBatchNode* batchNode = SpriteBatchNode::create("game/littleParticle.png", particleAmount);
	this->addChild(batchNode);

	for (int amount = 0; amount < particleAmount; amount++)
	{
		Sprite* littleParticle = Sprite::createWithTexture(batchNode->getTexture());
		littleParticle->setTag(LITTLE_PARTICLE_TAG);

		Size particleSize(20, 20);
		auto particleBody = PhysicsBody::createBox(particleSize);
		particleBody->setGravityEnable(false);
		particleBody->setContactTestBitmask(0x07);//0111
		particleBody->setCollisionBitmask(0x00);
		littleParticle->setPhysicsBody(particleBody);

		float posX = (MAP_WIDTH_TIMES * visibleSize.width - 2 * 100) * CCRANDOM_0_1()//100 pixels reserved for each edge
			- (MAP_WIDTH_TIMES * visibleSize.width / 2 - 100);
		float posY = (MAP_HEIGHT_TIMES * visibleSize.height - 2 * 100) * CCRANDOM_0_1()//100 pixels reserved for each edge
			- (MAP_HEIGHT_TIMES * visibleSize.height / 2 - 100);
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
}

void Game::createParticlesByTime(float dt)
{
	createLittleParticles(20);
}


bool Game::contactBegin(PhysicsContact& contact)
{
	auto player = (Sprite*)contact.getShapeA()->getBody()->getNode();
	auto littleParticle = (Sprite*)contact.getShapeB()->getBody()->getNode();

	if (player && littleParticle && player->getTag() == PLAYER_SPRITE_TAG && littleParticle->getTag() == LITTLE_PARTICLE_TAG)
	{
		player->setTag(CONTACT_TAG);
		refreshPlayerScale(1);
		player->setTag(PLAYER_SPRITE_TAG);
		this->removeChild(littleParticle);//little particle swallowed
		//log("player scale: %f", playerScale);
	}

	//log("onContactBegin");
	return true;
}

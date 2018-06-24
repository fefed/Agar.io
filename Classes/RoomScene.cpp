#include "RoomScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Client.h"

#define START_FORBIDDEN_TAG 101

USING_NS_CC;

std::string playerPic[8] = { "room/p1.png" , "room/p2.png" ,"room/p3.png" ,"room/p4.png" ,
"room/p5.png","room/p6.png","room/p7.png","room/p8.png" };

Scene* Room::createScene()
{
	return Room::create();
}

bool Room::init()
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


	//exit button
	auto closeItem = MenuItemImage::create(
		"startScreen/exitNormal.png",
		"startScreen/exitChosen.png",
		CC_CALLBACK_1(Room::menuCloseCallback, this));
	closeItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	//ready toggle
	/*auto readyOn = MenuItemImage::create("room/readyOn.png", "room/readyOn.png");
	auto readyOff = MenuItemImage::create("room/readyOff_2.png", "room/readyOff_2.png");

	auto readyToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Room::menuItemReadyCallback, this),
		readyOff, readyOn, NULL);
	readyToggle->setPosition(Director::getInstance()->convertToGL(Vec2(200, 3 * 120)));*/

	//forbidden start button
	auto startForbiddenSprite = Sprite::create("room/startForbidden.png");
	startForbiddenSprite->setPosition(Director::getInstance()->convertToGL(Vec2(250, 300)));
	startForbiddenSprite->setTag(START_FORBIDDEN_TAG);
	this->addChild(startForbiddenSprite, 1);

	//create or join room button
	Sprite* createSpriteNormal = Sprite::create("room/createOrJoin.png");
	Sprite* createSpriteSelected = Sprite::create("room/createOrJoinC.png");//C for chosen

	MenuItemSprite* createMenuItem = MenuItemSprite::create(createSpriteNormal, createSpriteSelected,
		CC_CALLBACK_1(Room::menuItemCreateCallback, this));
	createMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(250, 150)));


	Menu* mn = Menu::create(closeItem, createMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	return true;
}


Client* client;
//replace by exit?
void Room::menuCloseCallback(cocos2d::Ref* pSender)
{
	client->close();
	Director::getInstance()->popScene();
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

//replace by start?
void Room::menuItemStartCallback(Ref* pSender)
{
	log("start button triggered");
	/*Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);*/
}


//do not create client repeatedly
//remote: client create(1) local(2)
//withserver true (local true)
//roomscene 2 (2 for room owner, 1 for others)

bool init_client = true;
bool room_owner = false;

void Room::menuItemCreateCallback(Ref* pSender)
{
	/*Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);*/

	if (init_client)
	{
		auto connectingSprite = Sprite::create("room/connecting.png");
		connectingSprite->setPosition(Director::getInstance()->convertToGL(Vec2(250, 150)));
		this->addChild(connectingSprite);

		//auto server = LocalServer::create();
		//this->addChild(server);
		client = Client::create(1);
		this->addChild(client);
		client->_filter_mode = false;
		//client->sensitive_word = _playerName;
		client->_with_server = false;
		//client->_with_server = true;
		init_client = false;

		this->schedule(schedule_selector(Room::search), 20.0 / 60.0);
	}

	/*auto server = LocalServer::create();
	this->addChild(server);
	auto client = Client::create(2);
	this->addChild(client);
	client->_filter_mode = true;
	//client->sensitive_word = _playerName;
	client->_with_server = true;*/
}


int try_time = 0;
bool if_found = false;
bool init_create_room = true;

void Room::search(float dt)
{
	if ((!if_found) && (try_time < 5) && (client->_search_finished))
	{
		try_time++;
		client->sendMessage(QUERY_FOR_ROOM, "|||||||||||");

		std::string temp = client->executeOrder();
		//log("%c", temp[0]);
		if (temp != "no")
		{
			if (temp[0] == ANSWER_FOR_ROOM[0])
			{
				if_found = true;
				if (temp == "bfull") 
				{
					auto roomFullSprite = Sprite::create("room/roomFull.png");
					roomFullSprite->setPosition(Director::getInstance()->convertToGL(Vec2(250, 150)));
					this->addChild(roomFullSprite);

					unschedule(schedule_selector(Room::search));
				}
				else
				{
					this->schedule(schedule_selector(Room::update), 1.0 / 60.0);

					auto roomJoinedSprite = Sprite::create("room/roomJoined.png");
					roomJoinedSprite->setPosition(Director::getInstance()->convertToGL(Vec2(250, 150)));
					this->addChild(roomJoinedSprite);

					unschedule(schedule_selector(Room::search));
				}
			}
		}
	}

	else if ((!if_found) && (try_time > 4) && (init_create_room))
	{
		init_create_room = false;
		room_owner = true;
		this->schedule(schedule_selector(Room::update), 1.0 / 60.0);

		auto roomCreatedSprite = Sprite::create("room/roomCreated.png");
		roomCreatedSprite->setPosition(Director::getInstance()->convertToGL(Vec2(250, 150)));
		this->addChild(roomCreatedSprite);

		auto startForbiddenSprite = this->getChildByTag(START_FORBIDDEN_TAG);
		startForbiddenSprite->setVisible(false);


		auto startSpriteNormal = Sprite::create("room/purpleStart.png");
		auto startSpriteChosen = Sprite::create("room/purpleStartC.png");

		MenuItemSprite* startMenuItem = MenuItemSprite::create(startSpriteNormal, startSpriteChosen,
			CC_CALLBACK_1(Room::menuItemStartCallback, this));
		startMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(250, 300)));

		Menu* mn = Menu::create(startMenuItem, NULL);
		mn->setPosition(Vec2::ZERO);
		this->addChild(mn, 0);


		unschedule(schedule_selector(Room::search));
	}
}


bool init_in_room = true;
int player_count = 0;

void Room::update(float dt)
{
	if (room_owner)
	{
		if (init_in_room)
		{
			auto playerSign = Sprite::create(playerPic[0]);
			playerSign->setPosition(Vec2(700, 100));
			this->addChild(playerSign);
			player_count++;
			init_in_room = false;
		}

		std::string temp = client->executeOrder();
		if (temp != "no") 
		{
			if (temp[0] == QUERY_FOR_ROOM[0])
			{
				if (player_count < 8)
				{
					char buffer[2];
					buffer[0] = player_count + 1 + '0';
					buffer[1] = '\0';
					std::string msg(buffer);
					client->sendMessage(ANSWER_FOR_ROOM, msg);
				}
				else
				{
					client->sendMessage(ANSWER_FOR_ROOM, "full");
				}
			}

			if (temp[0] == QUERY_FOR_PLAYERS_IN_ROOM[0])
			{
				player_count++;
				char buffer[2];
				buffer[0] = player_count + '0';
				buffer[1] = '\0';
				std::string msg(buffer);
				client->sendMessage(ANSWER_FOR_PLAYERS_IN_ROOM, msg);

				auto playerSign = Sprite::create(playerPic[player_count - 1]);
				playerSign->setPosition(Vec2(700, 20 + 80 * player_count));
				this->addChild(playerSign);
			}
		}
	}

	else
	{
		if (init_in_room)
		{
			init_in_room = false;
			client->sendMessage(QUERY_FOR_PLAYERS_IN_ROOM, "whoin?");
		}

		std::string temp = client->executeOrder();
		if (temp != "no")
		{
			if (temp[0] == ANSWER_FOR_PLAYERS_IN_ROOM[0])
			{
				player_count = temp[1] - '0';
				Sprite* playerSign;
				for (int i = 0; i < player_count; i++)
				{
					playerSign = Sprite::create(playerPic[i]);
					playerSign->setPosition(Vec2(700, 100 + 80 * i));
					this->addChild(playerSign);
				}
			}
		}
	}

}

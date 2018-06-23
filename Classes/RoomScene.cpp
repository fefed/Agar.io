#include "RoomScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Server.h"
#include "Client.h"

USING_NS_CC;

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

	//back button
	auto backMenuItem = MenuItemImage::create("secondMenu/backNorma.png", "secondMenu/backChosen.png",//Norma instead of Normal for a strange bug
		CC_CALLBACK_1(Room::menuBackCallback, this));
	backMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(visibleSize.width - 121 / 2, visibleSize.height - 38 / 2)));//Size 121*38

	//ready toggle
	auto readyOn = MenuItemImage::create("room/readyOn.png", "room/readyOn.png");
	auto readyOff = MenuItemImage::create("room/readyOff_2.png", "room/readyOff_2.png");

	auto readyToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(Room::menuItemReadyCallback, this),
		readyOff, readyOn, NULL);
	readyToggle->setPosition(Director::getInstance()->convertToGL(Vec2(200, 3 * 120)));

	//create room button
	Sprite* createSpriteNormal = Sprite::create("room/create.png");
	Sprite* createSpriteSelected = Sprite::create("room/createC.png");//C for chosen

	MenuItemSprite* createMenuItem = MenuItemSprite::create(createSpriteNormal, createSpriteSelected,
		CC_CALLBACK_1(Room::menuItemCreateCallback, this));
	createMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(200, 120)));

	//join room button
	Sprite* joinSpriteNormal = Sprite::create("room/join.png");
	Sprite* joinSpriteSelected = Sprite::create("room/joinC.png");//C for chosen

	MenuItemSprite* joinMenuItem = MenuItemSprite::create(joinSpriteNormal, joinSpriteSelected,
		CC_CALLBACK_1(Room::menuItemJoinCallback, this));
	joinMenuItem->setPosition(Director::getInstance()->convertToGL(Vec2(200, 2 * 120)));

	Menu* mn = Menu::create(backMenuItem, readyToggle, createMenuItem, joinMenuItem, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn, 0);

	return true;
}

void Room::menuBackCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

void Room::menuItemReadyCallback(Ref* pSender)
{
	/*Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);*/
}

void Room::menuItemCreateCallback(Ref* pSender)
{
	/*Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);*/

	//auto server = LocalServer::create();
	//this->addChild(server);
	auto client = Client::create(1);
	this->addChild(client);
	client->_filter_mode = true;
	//client->sensitive_word = _playerName;
	client->_with_server = false;
	//client->_with_server = true;

	/*auto server = LocalServer::create();
	this->addChild(server);
	auto client = Client::create(2);
	this->addChild(client);
	client->_filter_mode = true;
	//client->sensitive_word = _playerName;
	client->_with_server = true;*/
}

//do not create client repeatedly
//remote: client create(1) local(2)
//withserver true (local true)
//roomscene 2 (2 for room owner, 1 for others)

void Room::menuItemJoinCallback(Ref* pSender)
{
	/*Director::getInstance()->popScene();

	auto sc = Game::createScene();
	auto reScene = TransitionCrossFade::create(0.5f, sc);
	Director::getInstance()->replaceScene(reScene);*/

	auto client = Client::create(1);
	client->_with_server = false;
}

void Room::update(float dt)
{
}

/*#define CLIENT_MODE 1
#define SERVER_MODE 2

bool    finish_create;
int     player_count;
int     current_count;
int     loop_you_in;
bool    if_initial;
bool    if_self_joined;

static int      roomMode;
static Client*  client = nullptr;

static int loop;

//std::vector<PlayerData> player_list;
//std::map<int, Button*>  chatMessage;

static std::string player_name;

void Room::update(float delta)
{

	if (player_count == 1) {
		_room_ptr->setVisible(true);
	}

	loop++;
	if (loop / 100 > 1) {
		loop = 0;
		system("ping -c 1 255.255.255.255");
	}


	if (roomMode == CLIENT_MODE && if_initial == false && player_count != 0)
	{
		if_initial = true;
		client->sendMessage(QUERY_FOR_PLAYERS_IN_ROOM, "whoin?");
		player_count--;
	}

	if (current_count != player_count && roomMode == SERVER_MODE) {
		current_count = player_count;
		auto board = Button::create("PlayerBar.png", "PlayerBar.png");
		player_list.push_back(*_owner_player_data);

		std::string show_string;
		show_string.append(_owner_player_data->player_name);
		//                show_string.append("1# : ");
		//                show_string[0] = temp[1];
		//                show_string.append(player.player_name + "   Role: ");
		//                show_string.append(player.player_role);

		auto serial_num = Sprite::create("1.png");
		board->addChild(serial_num);
		serial_num->setPosition(Vec2(serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

		auto chat_bar = Button::create("ChatBar.png", "ChatBar.png");
		serial_num->addChild(chat_bar);
		chat_bar->setPosition(Vec2(serial_num->getContentSize().width * 2.5, serial_num->getContentSize().height * 1.3));
		chatMessage[1] = chat_bar;
		chat_bar->setVisible(false);


		auto player_icon = Sprite::create(StringUtils::format("%s.png", _owner_player_data->player_role.c_str()));
		board->addChild(player_icon);
		player_icon->setPosition(Vec2(board->getContentSize().width - serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

		board->setTitleText(show_string);

		board->setTitleFontSize(30);
		_room_ptr->addChild(board);
		board->setPosition(Vec2(
			_room_ptr->getContentSize().width / 2,
			(_room_ptr->getContentSize().height - 90) / 6 * (6 - player_count)));
	}

	std::string temp = client->executeOrder();

	if (temp != "no") {

		//        std::cout << "room: " << "\t" <<temp << std::endl;

		if (temp[0] == QUERY_FOR_ROOM[0] && roomMode == SERVER_MODE) {
			client->sendMessage(ANSWER_FOR_ROOM, _owner_player_name);
		}

		if (temp[0] == QUERY_FOR_PLAYERS_IN_ROOM[0] && roomMode == SERVER_MODE)
		{
			std::string players_in_room;
			std::string number;
			number.append("0");
			number[0] += _selectLevelIndex;
			client->sendMessage(MAP_SELECT, number);
			for (int i = 0; i < player_list.size(); i++) {
				char buffer[2];
				buffer[0] = i + 1 + '0';
				buffer[1] = '\0';
				std::string msg(buffer);
				msg.append(player_list.at(i).player_role);
				msg.append("|");
				msg.append(player_list.at(i).player_name);
				client->sendMessage(ANSWER_FOR_PLAYERS_IN_ROOM, msg);
			}
		}

		if (temp[0] == ANSWER_FOR_PLAYERS_IN_ROOM[0] && roomMode == CLIENT_MODE)
		{
			std::string players_in_room;
			int i = player_list.size();
			if (player_list.size() == 0)
			{
				player_count++;
				int i = temp.find('|');
				std::string role = temp.substr(2, i - 2);
				std::string player_name = temp.substr(i + 1, temp.size() - i - 1);

				PlayerData player(player_name, role, temp[1] - '0');
				player_list.push_back(player);
				auto board = Button::create("PlayerBar.png", "PlayerBar.png");
				std::string show_string;
				show_string.append(player.player_name);
				//                show_string.append("1# : ");
				//                show_string[0] = temp[1];
				//                show_string.append(player.player_name + "   Role: ");
				//                show_string.append(player.player_role);

				if (temp[1] > '4') {
					temp[1] = '4';
				}

				auto serial_num = Sprite::create(StringUtils::format("%c.png", temp[1]));
				board->addChild(serial_num);
				serial_num->setPosition(Vec2(serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

				auto chat_bar = Button::create("ChatBar.png", "ChatBar.png");
				serial_num->addChild(chat_bar);
				chatMessage[temp[1] - '0'] = chat_bar;
				chat_bar->setPosition(Vec2(serial_num->getContentSize().width * 2.5, serial_num->getContentSize().height * 1.3));
				chat_bar->setVisible(false);


				auto player_icon = Sprite::create(StringUtils::format("%s.png", player.player_role.c_str()));
				board->addChild(player_icon);
				player_icon->setPosition(Vec2(board->getContentSize().width - serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

				board->setTitleText(show_string);
				board->setTitleFontSize(30);
				_room_ptr->addChild(board);
				board->setPosition(Vec2(
					_room_ptr->getContentSize().width / 2,
					(_room_ptr->getContentSize().height - 90) / 6 * (6 - player_count)));
			}
			else {
				bool if_exist = false;
				int i = temp.find('|');
				std::string role = temp.substr(2, i - 2);
				std::string player_name = temp.substr(i + 1, temp.size() - i - 1);
				for (int i = 0; i < player_list.size(); i++)
				{
					if (player_name == player_list.at(i).player_name) {
						if_exist = true;
					}
				}
				if (!if_exist)
				{
					int i = temp.find('|');
					std::string role = temp.substr(2, i - 2);
					std::string player_name = temp.substr(i + 1, temp.size() - i - 1);

					player_count++;
					PlayerData player(player_name, role, temp[1] - '0');
					player_list.push_back(player);
					auto board = Button::create("PlayerBar.png", "PlayerBar.png");
					std::string show_string;
					show_string.append(player.player_name);
					//                    show_string.append("1# : ");
					//                    show_string[0] = temp[1];
					//                    show_string.append(player.player_name + "   Role: ");
					//                    show_string.append(player.player_role);

					if (temp[1] > '4') {
						temp[1] = '4';
					}

					auto serial_num = Sprite::create(StringUtils::format("%c.png", temp[1]));
					board->addChild(serial_num);
					serial_num->setPosition(Vec2(serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

					auto chat_bar = Button::create("ChatBar.png", "ChatBar.png");

					serial_num->addChild(chat_bar);
					chat_bar->setPosition(Vec2(serial_num->getContentSize().width * 2.5, serial_num->getContentSize().height * 1.3));
					chat_bar->setVisible(false);
					chatMessage[temp[1] - '0'] = chat_bar;
					auto player_icon = Sprite::create(StringUtils::format("%s.png", player.player_role.c_str()));
					board->addChild(player_icon);
					player_icon->setPosition(Vec2(board->getContentSize().width - serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

					board->setTitleText(show_string);
					board->setTitleFontSize(30);
					_room_ptr->addChild(board);
					board->setPosition(Vec2(
						_room_ptr->getContentSize().width / 2,
						(_room_ptr->getContentSize().height - 90) / 6 * (6 - player_count)));
				}
			}
		}

		if (temp[0] == MAP_SELECT[0])
		{
			_game_data->setmapIndex(temp[1] + 1 - '0');
		}

		if (temp[0] == START_GAME[0]) {
			for (int i = 0; i < player_list.size(); i++)
			{
				_game_data->player_list.push_back(player_list.at(i));
			}
			auto transition = TransitionSplitRows::create(2.0, Game::createScene(*_game_data, client, _owner_player_name));

			Director::getInstance()->pushScene(transition);
		}

		if (temp[0] == JOIN_ROOM[0] && if_self_joined == false) {
			if_self_joined = true;
			return;
		}

		if (temp[0] == CHAT_MESSAGE[0]) {
			DelayTime * delay = DelayTime::create(2.5f);
			auto callFunc1 = CallFunc::create([=] {
				chatMessage[temp[1] - '0']->setVisible(false);
			});
			chatMessage[temp[1] - '0']->setVisible(true);
			chatMessage[temp[1] - '0']->setTitleText(std::string(&temp[2]));
			chatMessage[temp[1] - '0']->setTitleFontSize(30);
			auto sequence = Sequence::create(delay, callFunc1, NULL);
			chatMessage[temp[1] - '0']->runAction(sequence);
		}
		if (temp[0] == JOIN_ROOM[0]) {
			player_count++;
			current_count = player_count;
			int i = temp.find('|');
			std::string role = temp.substr(2, i - 2);
			std::string player_name = temp.substr(i + 1, temp.size() - i - 1);


			PlayerData player(player_name, role, player_count);
			player_list.push_back(player);
			auto board = Button::create("PlayerBar.png", "PlayerBar.png");
			std::string show_string;

			show_string.append(player.player_name);

			if (temp[1] > '4') {
				temp[1] = '4';
			}

			auto serial_num = Sprite::create(StringUtils::format("%c.png", player_count + '0'));
			board->addChild(serial_num);
			serial_num->setPosition(Vec2(serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

			auto chat_bar = Button::create("ChatBar.png", "ChatBar.png");
			serial_num->addChild(chat_bar);
			chat_bar->setPosition(Vec2(serial_num->getContentSize().width * 2.5, serial_num->getContentSize().height * 1.3));

			chat_bar->setVisible(false);
			chatMessage[player_count] = chat_bar;
			auto player_icon = Sprite::create(StringUtils::format("%s.png", player.player_role.c_str()));
			board->addChild(player_icon);
			player_icon->setPosition(Vec2(board->getContentSize().width - serial_num->getContentSize().width * 1, board->getContentSize().height / 2));

			board->setTitleText(show_string);
			board->setTitleFontSize(30);
			_room_ptr->addChild(board);
			board->setPosition(Vec2(
				_room_ptr->getContentSize().width / 2,
				(_room_ptr->getContentSize().height - 90) / 6 * (6 - player_count)));

		}
	}
}*/

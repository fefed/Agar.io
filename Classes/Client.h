#pragma once

#include "cocos2d.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include "chat_message.hpp"
#include <vector>
#include <string>


/**
* @name 询问网络中是否存在已经建立好的游戏房间
* @{
*/
#define QUERY_FOR_ROOM  "a"
/** @} */

/**
* @name 回应QUERY_FOR_ROOM, 并提供房间基本信息
* @{
*/
#define ANSWER_FOR_ROOM "b"
/** @} */

/**
* @name 多个房间被创建
* @{
*/
#define START_FAILED       "c"
/** @} */

/**
* @name 广播开始游戏指令
* @{
*/
#define START_GAME      "d"
/** @} */

/**
* @name 向房间内所有玩家传输退出房间的玩家信息
* @{
*/
#define EXIT_ROOM       "e"
/** @} */

/**
* @name 新加入房间的玩家向房间拥有者请求房间内所有玩家的信息
* @{
*/
#define QUERY_FOR_PLAYERS_IN_ROOM "f"
/** @} */

/**
* @name QUERY_FOR_PLAYERS_IN_ROOM, 房间所有者\n
* 		向新加入者传输房间内所有玩家的信息
* @{
*/
#define ANSWER_FOR_PLAYERS_IN_ROOM "g"
/** @} */

/**
* @name 向房间内所有玩家传输聊天信息
* @{
*/
#define CHAT_MESSAGE    "h"
/** @} */

/**
* @name 出生位置
* @{
*/
#define INIT_POS      "i"
/** @} */

/**
* @name 向游戏内其他玩家广播死亡消息，确保同步死亡
* @{
*/
//example j1287||||||||||||
//j for DEAD_MESSAGE
//1 for player1
//2 for player1's ball 2(start from 0)
//8 for player8
//7 for player8's ball 7
#define DEAD_MESSAGE    "j"
/** @} */

//询问开始
#define QUERY_FOR_START "k"

//回应QUERY_FOR_START
#define ANSWER_FOR_START "l"

//动作种类
//began: 1, moved: 2, ended: 3
//
//tan: m1 > 77.5 > m2 > 22.5  > m3
//
//		+y		-y
//+x	1		2
//-x	3		4
//
//example: m2134|||||||||||
//m for MOVE_ACTION
//2 for player2
//1 for touch began
//3 for tan < 22.5
//4 for x < 0, y < 0
#define MOVE_ACTION "m"

//分裂
//example n8120||||||||||||
//n for DIVIDE_ACTION
//8 for player8
//1 for move1
//2 for x > 0, y < 0
//0 for if_is_moving_ol == false
#define DIVIDE_ACTION "n"

//体积
//example: o12345|||||||||||
//o for PLAYER_SCALE
//1 for player1
//2 for player1's ball 2 (start from 0)
//345 for scale 3.45
#define PLAYER_SCALE "o"

//位置
//example: p219876541234561|
//p for PLAYER_POSITION
//2 for player2
//1 for player2's ball 1 (start from 0)
//987654 for x 9876.54
//123456 for y 1234.56
//1 for x > 0, y > 0
//		+y		-y
//+x	1		2
//-x	3		4
#define PLAYER_POSITION "p"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

/**
*   @brief  数据接收、操作类\n
*           一次存储并处理一条数据, 异步接收、发送数据\n
*/
class chat_client
{
public:

	/**
	* @brief                        构造一个可以连接的客户端
	*
	* @param io_service             asio的核心类, 用于创建socket
	* @param endpoint_iterator      要连接位置的端口号
	*
	*/
	chat_client(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		:
		io_service_(io_service), socket_(io_service)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
			boost::bind(&chat_client::handle_connect, this,
				boost::asio::placeholders::error)); //所有的操作都采用异步的方式
	}

	/**
	* @brief        将指令传给io_service以发送给server
	*
	* @param msg    自定义格式的数据包
	*
	*/
	void write(const chat_message& msg)
	{
		io_service_.post(boost::bind(&chat_client::do_write, this, msg));
	}

	/**
	* @brief        客户端主动终止时调用该函数
	*/
	void close()
	{
		io_service_.post(boost::bind(&chat_client::do_close, this));
	}

private:

	/**
	* @brief        成功建立连接时调用该函数
	*
	* @param error  调用I/O例程时返回的错误码
	*
	*/
	void handle_connect(const boost::system::error_code& error);

	/**
	* @brief        在接受到消息后处理数据字符串，并调用处理命令的函数(handle_read_body)
	*
	* @param error  调用I/O例程时返回的错误码
	*
	*/
	void handle_read_header(const boost::system::error_code& error);

	/**
	* @brief        当接受完数据并处理完字符串长度后调用该函数，处理命令本体, 在操作\n
	*               结束后继续读取报头, 在读取结束后调用handle_read_header继续处理
	*
	* @param error  调用I/O例程时返回的错误码
	*
	*/
	void handle_read_body(const boost::system::error_code& error);
		
	/**
	* @brief        将要发送的数据压入等待发送的数据队列，在操作压入操作结束后先发送一\n
	*               个数据包，如果待发送的数据包多于一个的话则调用handle_connect发送\n
	*               剩余的数据包
	*
	* @param msg    要发送的已经编码好的数据包
	*
	*/
	void do_write(chat_message msg);

	/**
	* @brief        递归发送等待发送的数据，在没有数据发送时停止递归
	*
	* @param error  调用I/O例程时返回的错误码
	*
	*/
	void handle_write(const boost::system::error_code& error);

	/**
	* @brief        从服务器读取消息失败时调用
	*/
	void do_close()
	{
		socket_.close();
	}

private:
	boost::asio::io_service&    io_service_;    ///asio的核心类, 用于创建socket
	tcp::socket                 socket_;        ///tcp类套接字
	chat_message                read_msg_;      ///每次循环所读取的数据对象
	chat_message_queue          write_msgs_;    ///等待被发送的数据对象
};

//----------------------------------------------------------------------

/**
*   @brief  搜索并连接server\n
*           分两种工作模式，在局域网下负责搜索并连接局域网内\n
*           的server，在互联网模式下则直接连接远程server
*/
class Client : public cocos2d::Node
{
public:

	bool                        _search_finished;       ///是否完成搜索并于server建立起连接        
	bool                        _filter_mode;           ///是否开启filter mode
	bool                        _with_server;           ///client启动时是否有server同步启动
	chat_client*                _clientInstance;        ///用于处理数据的接受和发送
	std::mutex                  t_lock;                 ///线程锁，避免多个线程下对数据进行同时操作
	std::string                 sensitive_word;         ///filter mode下用于甄别指令的敏感词
	std::deque<std::string>     _orderList;             ///读取到的正确指令

														/**
														* @brief    创建client对象并运行
														*/
	static Client* create(int mode)
	{
		Client *sprite = new Client();
		if (sprite)
		{
			//remember to release?
			//remember to release?
			//remember to release?

			sprite->autorelease();
			sprite->runClient(mode);

			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	/**
	* @brief    运行client
	*
	* @param    mode         运行client的模式
	*        -<em>1</em>    同时与server启动
	*        -<em>2/em>     只启动client
	*
	*/
	void runClient(int mode);

	/**
	* @brief    取出读取到的指令
	*
	* @return   读取到并经过过滤的指令
	*        -<em>"no"</em>             没有更多指令
	*        -<em>std::string</em>      正确指令
	*/
	std::string executeOrder(void);

	/**
	* @brief    运行client
	*
	* @param    code        指令码
	* @param    message     要发送的指令
	*
	*/
	void sendMessage(const std::string & code, const std::string & message);

	/**
	* @brief    启动client并搜索、连接server
	*
	* @return   是否正常结束
	*/
	int client(void);

	void close()
	{
		_clientInstance->close();
	}
};

//extern Client* this_client;
extern int player_num;
extern int player_count;
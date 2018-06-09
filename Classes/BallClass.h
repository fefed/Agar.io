#pragma once

#include<cocos2d.h>

USING_NS_CC;

class Ball : public Sprite
{
public:
	/*Ball( )
		:_ballScale(1.0), _ballNum(1)
	{
		_ballSprite = Sprite::create("game/player50x50.png");
	}

	Ball(const char* _Ptr = "game/player50x50.png", float scale = 1.0)
		:_ballScale(scale), _ballNum(1)
	{
		_ballSprite = Sprite::create(_Ptr);
	}*/

	/*Ball()
		:_ballScale(1.0), _ballNum(1)
	{ }*/
	//virtual ~Ball();

	static Ball* create(const char* _Ptr = "game/player50x50.png")
	{
		Ball* ball = (Ball*)Sprite::create(_Ptr);
		return ball;
	}

	float& getBallScale() { return _ballScale; }
	//int& getBallNum() { return _ballNum; }
	//Sprite* getBallSpritePointer() { return _ballSprite; }

	void setScale(float scale)
	{
		_ballScale = scale;
		this->Sprite::setScale(scale);
	}

private:
	float _ballScale;
	//Sprite* _ballSprite;
	//int _ballNum;
};


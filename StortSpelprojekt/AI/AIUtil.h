 #pragma once
#define AI_EXPORT __declspec(dllexport)

namespace AI
{
	struct AI_EXPORT Vec2D
	{
		short _x, _y;
		Vec2D()
		{
			_x = 0;
			_y = 0;
		}
 		Vec2D(int x, int y)
		{
			_x = x;
			_y = y;
		}
		~Vec2D()
		{}
		bool operator==(const Vec2D& comp) const
		{
			return _x == comp._x && _y == comp._y;
		}
		bool operator!=(const Vec2D& comp) const
		{
			return _x != comp._x || _y != comp._y;
		}
		Vec2D operator+(const Vec2D& comp)
		{
			return {_x + comp._x, _y + comp._y};
		}
		Vec2D operator-(const Vec2D& comp)
		{
			return {_x - comp._x, _y - comp._y};
		}
		Vec2D operator*(const int mult)
		{
			return{_x * mult, _y * mult};
		}
		Vec2D operator+=(const Vec2D& comp)
		{
			_x += comp._x;
			_y += comp._y;
			return *this;
		}
		Vec2D operator-=(const Vec2D& comp)
		{
			_x -= comp._x;
			_y -= comp._y;
			return *this;
		}
	};

	const float SQRT2 = 1.41421356f;								//used for diagonals. Placing a constant means no root-calculations.
	const Vec2D NEIGHBOUR_OFFSETS[8] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ 1, -1 },{ -1, 1 },{ 1, 1 } };	//Straight moves in 0-3, diagonal in 4-7
	const Vec2D CLOCKWISE_ROTATION[8] = {{-1, 0},{-1, 1},{0, 1},{1, 1},{1, 0},{1, -1},{0, -1},{-1, -1}};

	static Vec2D GetNextDirection(Vec2D originalDirection, bool clockwise)
	{
		int counter = 8;
		while (originalDirection != CLOCKWISE_ROTATION[counter - 8] && counter < 16)
		{
			counter++;
		}
		if (clockwise)
		{
			counter++;
		}
		else
		{
			counter--;
		}
		return CLOCKWISE_ROTATION[counter % 8];
	}
}
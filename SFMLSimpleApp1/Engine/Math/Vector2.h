#pragma once

#include "math.h"
#include "SFML/Graphics.hpp"

struct Vector2 {
	float x;
	float y;

	Vector2(float x, float y) : x(x), y(y) { }
	Vector2() : x(0), y(0) { }
	Vector2(const sf::Vector2f& vec) : x(vec.x), y(vec.y) { }
	Vector2(const sf::Vector2i& vec) : x((float)vec.x), y((float)vec.y) { }
	Vector2 operator+(const Vector2& rhs) const {	return Vector2(x + rhs.x, y + rhs.y); }
	Vector2 operator*(const float scale) const { return Vector2(x * scale, y * scale); }
	Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
	Vector2 operator/(const float scale) const { return Vector2(x / scale, y / scale); }
	bool operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }
	Vector2& operator+=(const Vector2& rhs) 
	{
		x += rhs.x;
		y += rhs.y;
		return *this; 
	}
	Vector2& operator-=(const Vector2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	Vector2& operator*=(const float scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}
	Vector2& operator/=(const float scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}
	float dot(const Vector2& vecB)
	{
		return (x * vecB.x) + (y * vecB.y);
	}
	float magnitudeSqr() const
	{
		return (x * x) + (y * y);
	}
	float magnitude() const
	{
		return sqrtf(magnitudeSqr());
	}
	void normalize()
	{
		x /= magnitude();
		y /= magnitude();
	}
	static float dist(const Vector2& vecA, const Vector2& vecB)
	{
		return Vector2(vecB.x - vecA.x, vecB.y - vecB.y).magnitude();
	}
	static float distSqr(const Vector2& vecA, const Vector2& vecB)
	{
		return Vector2(vecB.x - vecA.x, vecB.y - vecB.y).magnitudeSqr();
	}
};
#include "stdafx.h"
#include "Vector2d.h"


Vector2d::Vector2d() : x{0.0f} , y{0.0f} {}
Vector2d::Vector2d(float X, float Y) : x{X}, y{Y} {}


Vector2d & Vector2d::Add(const Vector2d & vec)
{
	this->x += vec.x;
	this->y += vec.y;
	return *this;
}

Vector2d & Vector2d::Multiply(const Vector2d & vec)
{
	this->x *= vec.x;
	this->y *= vec.y;
	return *this;
}

Vector2d & Vector2d::Subtract(const Vector2d & vec)
{
	this->x -= vec.x;
	this->y -= vec.y;
	return *this;
}

Vector2d & Vector2d::Divide(const Vector2d & vec)
{
	this->x /= vec.x;
	this->y /= vec.y;
	return *this;
}

Vector2d & Vector2d::operator+=(const Vector2d & vec)
{
	return this->Add(vec);
}

Vector2d & Vector2d::operator-=(const Vector2d & vec)
{
	return this->Subtract(vec);
}

Vector2d & Vector2d::operator/=(const Vector2d & vec)
{
	return this->Divide(vec);
}

Vector2d & Vector2d::operator*=(const Vector2d & vec)
{
	return this->Multiply(vec);
}

Vector2d & Vector2d::operator*(const int & i)
{
	this->x *= i;
	this->y *= i;
	return *this;
}

Vector2d & Vector2d::Zero()
{
	this->x = 0; this->y = 0;
	return *this;
}

Vector2d & operator+(Vector2d & v1, const Vector2d & v2)
{
	return v1.Add(v2);
}

Vector2d & operator-(Vector2d & v1, const Vector2d & v2)
{
	return v1.Subtract(v2);
}

Vector2d & operator*(Vector2d & v1, const Vector2d & v2)
{
	return v1.Multiply(v2);
}

Vector2d & operator/(Vector2d & v1, const Vector2d & v2)
{
	return v1.Divide(v2);
}

std::ostream & operator<<(std::ostream & out, const Vector2d & vec)
{
	out << "( " << vec.x << ", " << vec.y << " )" << std::endl;
	return out;
}



Vector2d::~Vector2d()
{

}
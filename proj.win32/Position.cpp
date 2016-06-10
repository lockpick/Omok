#include "Position.h"

Position::Position(std::uint8_t x, std::uint8_t y) : _x(x), _y(y)
{
}

Position::~Position()
{
}

bool Position::operator==(const Position & rhs) const
{
    return _x==rhs._x && _y==rhs._y;
}

bool Position::operator<(const Position & rhs) const
{
    return _y < rhs._y || ((_y == rhs._y) && _x < rhs._x);
}

bool Position::operator>(const Position & rhs) const
{
    return _y > rhs._y || ((_y == rhs._y) && _x > rhs._x);
}

Position & Position::operator+=(const Position & rhs)
{
    _x += rhs._x;
    _y += rhs._y;
    return *this;
}

Position Position::operator+(const Position & rhs) const
{
    return Position(*this)+=rhs;
}

Position & Position::operator-=(const Position & rhs)
{
    _x -= rhs._x;
    _y -= rhs._y;
    return *this;
}

Position Position::operator-(const Position & rhs) const
{
    return Position(*this) -= rhs;
}

Position & Position::operator*=(const int rhs)
{
    _x *= rhs;
    _y *= rhs;
    return *this;
}

Position Position::operator*(const int rhs) const
{
    return Position(*this) *= rhs;
}

std::uint8_t Position::getX() const
{
    return _x;
}

std::uint8_t Position::getY() const
{
    return _y;
}

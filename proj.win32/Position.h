#ifndef  _POSITION_H_
#define  _POSITION_H_

#include <cstddef>
#include <cstdint>
#include <functional>

class Position final
{
public:
    Position(std::uint8_t x, std::uint8_t y);
    Position(const Position &position) = default;
    ~Position();

    Position& operator=(const Position &rhs) = default;

    bool operator==(const Position &rhs) const;
    bool operator<(const Position &rhs) const;
    bool operator>(const Position &rhs) const;

    Position& operator+=(const Position &rhs);
    Position operator+(const Position &rhs) const;
    Position& operator-=(const Position &rhs);
    Position operator-(const Position &rhs) const;
    Position& operator*=(const int rhs);
    Position operator*(const int rhs) const;

    std::uint8_t getX() const;
    std::uint8_t getY() const;

private:
    std::uint8_t _x, _y;
};

namespace std
{
    template<> struct hash<Position>
    {
        typedef Position argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& p) const
        {
            result_type const h1(p.getX());
            result_type const h2(p.getY());
            return h1 ^ (h2 << 8);
        }
    };
}

#endif // _POSITION_H_
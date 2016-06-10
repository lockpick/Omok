#ifndef  _PIECE_H_
#define  _PIECE_H_

#include <memory>

#include "Position.h"


class Piece
{
public:
    static std::shared_ptr<Piece> createBlack(const Position &pos);
    static std::shared_ptr<Piece> createWhite(const Position &pos);

    Piece(const Piece &piece) = delete;

    Piece& operator= (const Piece &rhs) = delete;

    Position getPosition() const;

    bool isBlack() const;
    bool isWhite() const;

protected:
    static const char BLACK = 'B';
    static const char WHITE = 'W';

    explicit Piece(const char owner, const Position &pos);
    virtual ~Piece();

private:
    const char _owner;
    Position _position;
};

#endif // _PIECE_H_
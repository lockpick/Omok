#include "Piece.h"

std::shared_ptr<Piece> Piece::createBlack(const Position &pos)
{
    std::shared_ptr<Piece> piece(new Piece(BLACK, pos), [](Piece* p) {delete p; });
    return piece;
}

std::shared_ptr<Piece> Piece::createWhite(const Position &pos)
{
    std::shared_ptr<Piece> piece(new Piece(WHITE, pos), [](Piece* p) {delete p; });
    return piece;
}

Position Piece::getPosition() const
{
    return _position;
}

bool Piece::isBlack() const
{
    return _owner == BLACK;
}

bool Piece::isWhite() const
{
    return _owner == WHITE;
}

Piece::Piece(const char owner, const Position &pos) : _owner(owner), _position(pos)
{
}

Piece::~Piece()
{
}

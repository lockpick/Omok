#include "OmokGame.h"

#include <cassert>

#include "Piece.h"

#include "../Classes/GameScene.h"

OmokGame::OmokGame(GameScene *gameScene) : _turn(Player::BLACK), _end(false), _state(std::make_shared<State>(15)), _gameScene(gameScene), _previousBlack(Position(-1, -1)), _previousWhite(Position(-1, -1))
{
}

OmokGame::~OmokGame()
{
}

void OmokGame::reset()
{
    _turn = Player::BLACK;
    _end = false;
    _state->reset();
}

bool OmokGame::put(int x, int y)
{
    return put(Position(x, y));
}

bool OmokGame::put(Position pos)
{
    bool success = false;
    std::shared_ptr<Piece> piece;
    switch (_turn) {
    case Player::BLACK:
        piece = Piece::createBlack(pos);
        break;
    case Player::WHITE:
        piece = Piece::createWhite(pos);
        break;
    default:
        assert(false);
    }
    success = _state->set(piece);

    if (success) {
        switch (_turn) {
        case Player::BLACK:
            _previousBlack = pos;
            _gameScene->printBlackStone(pos);
            if (_state->isBlackWin()) {
                _end = true;
            } else {
                _turn = WHITE;
            }
            break;
        case Player::WHITE:
            _previousWhite = pos;
            _gameScene->printWhiteStone(pos);
            if (_state->isWhiteWin()) {
                _end = true;
            }
            else {
                _turn = BLACK;
            }
            break;
        default:
            assert(false);
        }
    }
    return success;
}

bool OmokGame::isBlackTurn()
{
    return _turn == BLACK;
}

size_t OmokGame::getBoardSize() const
{
    return _state->getSize();
}

bool OmokGame::isGameOver() const
{
    return _end;
}

Position OmokGame::getPreviousBlack() const
{
    return _previousBlack;
}

Position OmokGame::getPreviousWhite() const
{
    return _previousWhite;
}

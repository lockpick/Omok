#ifndef  _OMOKGAME_H_
#define  _OMOKGAME_H_

#include "State.h"
#include "Position.h"

class GameScene;

class OmokGame
{
public:
    OmokGame(GameScene *gameScene);
    ~OmokGame();

    void reset();
    
    bool put(int x, int y);
    bool put(Position pos);

    bool isBlackTurn();

    size_t getBoardSize() const;
    bool isGameOver() const;

    Position getPreviousBlack() const;
    Position getPreviousWhite() const;

private:
    enum Player {BLACK, WHITE};

    enum Player _turn;
    bool _end;
    std::shared_ptr<State> _state;
    GameScene *_gameScene;

    Position _previousBlack;
    Position _previousWhite;
};

#endif // _OMOKGAME_H_
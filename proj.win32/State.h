#ifndef  _STATE_H_
#define  _STATE_H_

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Piece.h"

class State
{
public:
    explicit State(const std::size_t n);
    State(const State &board);
    virtual ~State();

    State& operator=(const State &rhs);

    void reset();

    bool set(const std::shared_ptr<Piece> &p);

    size_t getSize();

    std::vector<Position> getNextPoints(size_t size);
    double getScore();
    double getBlackScore();
    double getWhiteScore();

    bool isWin();

    bool isBlackWin();
    bool isWhiteWin();

protected:
    typedef char Block;

    static const Block SPACE = '\0';
    static const Block BLACK = 'B';
    static const Block WHITE = 'W';

    void noTestSet(const std::shared_ptr<Piece> &p);

    const Block get(Position p) const;
    void set(Block b, Position p);

    

private:
    enum Rank {NONE, ONE, TWO, THREE, S_THREE, FOUR, S_FOUR, FIVE};
    static double valueTable[7];

    struct Tst {
        std::vector<Block> line;
        std::vector<Position> position;
    };
    typedef std::shared_ptr<Tst> Test;

    struct Attackable {
        Block player;
        Rank rank;
        std::vector<Position> chances;
    };

    Test getVertical(Position p) const;
    Test getHorizontal(Position p) const;
    Test getDiagonalA(Position p) const;
    Test getDiagonalB(Position p) const;

    void update();
    void updateLine(const Test &test);

    bool isForbidden(Position p, Block b, const Test &test = Test()) const;
    bool check5(const Test &test, std::size_t start) const;
    std::shared_ptr<Attackable> check4(const Test &test, std::size_t start) const;
    std::shared_ptr<Attackable> check3(const Test &test, std::size_t start) const;
    std::shared_ptr<Attackable> check(const Test &test, std::size_t start) const;

    std::size_t _size;
    Block *_board;
    Block _winner;
    bool updated;

    std::vector<std::shared_ptr<Attackable>> _attackables;
};

#endif // _STATE_H_
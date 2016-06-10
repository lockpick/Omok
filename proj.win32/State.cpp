#include "State.h"

#include <cassert>
#include <algorithm>
#include <limits>

#include "Rule.h"

double State::valueTable[7] = { 0, 2, 4, 512, 16384, 11585, 2147483648 };

State::State(const std::size_t n) : _size(n), _board(nullptr), _winner(SPACE), updated(false)
{
    assert(n % 2 == 1);
    _board = new Block[_size*_size];
    std::memset(_board, SPACE, _size*_size);
}

State::State(const State & board) : _size(board._size), _board(nullptr), _winner(board._winner), updated(false)
{
    assert(board._size % 2 == 1);
    _board = new Block[_size*_size];
    std::memcpy(_board, board._board, _size*_size);

}

State::~State()
{
    if (_board != nullptr) {
        delete[] _board;
    }
}

State & State::operator=(const State & rhs)
{
    if (this != &rhs) {
        if (_size != rhs._size) {
            _size = rhs._size;
            delete[] _board;
            _board = new char[_size*_size];
        }
        _winner = rhs._winner;
        updated = false;
        std::memmove(_board, rhs._board, _size*_size);
    }
    return *this;
}

void State::reset()
{
    std::memset(_board, SPACE, _size*_size);
    _winner = SPACE;
    updated = false;
}

bool State::set(const std::shared_ptr<Piece>& p)
{
    bool success = false;
    if (!isForbidden(p->getPosition(), p->isBlack() ? BLACK : WHITE)) {
        noTestSet(p);
        success = true;
    }
    return success;
}

size_t State::getSize()
{
    return _size;
}

std::vector<Position> State::getNextPoints(size_t size)
{
    if (!updated) {
        update();
    }

    std::sort(_attackables.begin(), _attackables.end(), [](std::shared_ptr<Attackable> a, std::shared_ptr<Attackable> b) {
        return a->rank > b->rank;
    });

    std::vector<Position> points;
    size_t count = 0;
    for (const auto &attackable : _attackables) {
        points.insert(points.end(), attackable->chances.cbegin(), attackable->chances.cend());
        count += attackable->chances.size();
        if (count > size) {
            break;
        }
    }

    return points;
}

double State::getScore()
{
    return getBlackScore() - getWhiteScore();
}

double State::getBlackScore()
{
    if (!updated) {
        update();
    }
    double score = 0.0;
    if (_winner == BLACK) {
        score = std::numeric_limits<double>::infinity();
    }
    else if (_winner == WHITE) {
        score = -std::numeric_limits<double>::infinity();
    }
    else {
        std::unordered_map<Position, std::vector<std::shared_ptr<Attackable>>> black;
        for (const auto &attackable : _attackables) {
            if (attackable->player == BLACK) {
                for (const auto &chances : attackable->chances) {
                    black[chances].push_back(attackable);
                }
            }
        }
        for (auto pair : black) {
            auto &v = pair.second;
            std::sort(v.begin(), v.end(), [](std::shared_ptr<Attackable> a, std::shared_ptr<Attackable> b) {
                return a->rank < b->rank;
            });
        }
        for (auto pair : black) {
            double localValue = 1.0;
            for (const auto &attackable : pair.second) {
                localValue *= valueTable[attackable->rank];
            }
            score += localValue;
        }
    }

    return score;
}

double State::getWhiteScore()
{
    if (!updated) {
        update();
    }
    double score = 0.0;
    if (_winner == WHITE) {
        score = std::numeric_limits<double>::infinity();
    }
    else if (_winner == BLACK) {
        score = -std::numeric_limits<double>::infinity();
    }
    else {
        std::unordered_map<Position, std::vector<std::shared_ptr<Attackable>>> white;
        for (const auto &attackable : _attackables) {
            if (attackable->player == WHITE) {
                for (const auto &chances : attackable->chances) {
                    white[chances].push_back(attackable);
                }
            }
        }
        for (auto pair : white) {
            auto &v = pair.second;
            std::sort(v.begin(), v.end(), [](std::shared_ptr<Attackable> a, std::shared_ptr<Attackable> b) {
                return a->rank < b->rank;
            });
        }
        for (auto pair : white) {
            double localValue = 1.0;
            for (const auto &attackable : pair.second) {
                localValue *= valueTable[attackable->rank] * attackable->chances.size();
            }
            score += localValue;
        }
    }

    return score;
}

bool State::isWin()
{
    return isBlackWin() || isWhiteWin();
}

bool State::isBlackWin()
{
    if (!updated) {
        update();
    }
    return _winner == BLACK;
}

bool State::isWhiteWin()
{
    if (!updated) {
        update();
    }
    return _winner == WHITE;
}

void State::noTestSet(const std::shared_ptr<Piece> &p)
{
    bool checkDir[8] = {};
    Position pos = p->getPosition();
    Block player = p->isBlack() ? BLACK : WHITE;

    set(player, p->getPosition());

    updated = false;
}

const State::Block State::get(Position p) const
{
    assert(p.getX() < _size && p.getY() < _size);
    return _board[p.getX()*_size + p.getY()];
}

void State::set(Block b, Position p)
{
    assert(b == BLACK || b == WHITE);
    _board[p.getX()*_size + p.getY()] = b;
}

State::Test State::getVertical(Position p) const
{
    size_t y = p.getY();
    assert(y < _size);

    Test t = std::make_shared<Tst>();
    for (size_t x = 0; x < _size; ++x) {
        Position pos = Position(x, y);
        assert(p.getX() < _size && p.getY() < _size);
        t->position.push_back(pos);
        t->line.push_back(get(pos));
    }
    return t;
}

State::Test State::getHorizontal(Position p) const
{
    size_t x = p.getX();
    assert(x < _size);

    Test t = std::make_shared<Tst>();
    for (size_t y = 0; y < _size; ++y) {
        Position pos = Position(x, y);
        assert(p.getX() < _size && p.getY() < _size);
        t->position.push_back(pos);
        t->line.push_back(get(pos));
    }
    return t;
}

State::Test State::getDiagonalA(Position p) const
{
    assert(p.getX() < _size);
    assert(p.getY() < _size);

    Test t = std::make_shared<Tst>();
    size_t x = 0;
    size_t y = 0;
    if (p.getX() <  p.getY()) {
        y = p.getY() - p.getX();
    } else {
        x = p.getX() - p.getY();
    }
    

    while(x < _size && y < _size) {
        Position pos = Position(x, y);
        assert(p.getX() < _size && p.getY() < _size);
        t->position.push_back(pos);
        t->line.push_back(get(pos));
        ++x;
        ++y;
    }
    return t;
}

State::Test State::getDiagonalB(Position p) const
{
    assert(p.getX() < _size);
    assert(p.getY() < _size);

    Test t = std::make_shared<Tst>();
    size_t x = _size-1;
    size_t y = 0;
    if (p.getY() < _size - 1 - p.getX()) {
        x = p.getY() + p.getX();
    }
    else {
        y = p.getY() - (_size - 1 - p.getX());
    }


    while (x < _size && y < _size) {
        Position pos = Position(x, y);
        assert(p.getX() < _size && p.getY() < _size);
        t->position.push_back(pos);
        t->line.push_back(get(pos));
        --x;
        ++y;
    }
    return t;
}

void State::update()
{
    _attackables.clear();
    //vertical
    for (size_t i = 0; i < _size; ++i) {
        auto v = getVertical(Position(0, i));
        updateLine(v);
    }
    //horizontal
    for (size_t i = 0; i < _size; ++i) {
        auto v = getHorizontal(Position(i, 0));
        updateLine(v);
    }
    //diagonalA
    for (size_t i = 4; i < _size-1; ++i) {
        auto v = getDiagonalA(Position(i, _size-1));
        updateLine(v);
    }
    {
        auto v = getDiagonalA(Position(_size - 1, _size - 1));
        updateLine(v);
    }
    for (size_t i = 1; i < _size - 4; ++i) {
        auto v = getDiagonalA(Position(i, 0));
        updateLine(v);
    }
    //diagonalB
    for (size_t i = 4; i < _size - 1; ++i) {
        auto v = getDiagonalB(Position(i, 0));
        updateLine(v);
    }
    {
        auto v = getDiagonalB(Position(_size - 1, 0));
        updateLine(v);
    }
    for (size_t i = 1; i < _size - 4; ++i) {
        auto v = getDiagonalB(Position(i, _size - 1));
        updateLine(v);
    }
    updated = true;
}

void State::updateLine(const Test & test)
{
    size_t i = 0;
    Block old = SPACE;
    while (i < test->line.size()) {
        if (old != test->line[i] && test->line[i] != SPACE) {
            bool win = check5(test, i);
            if (win) {
                _winner = get(test->position[i]);
            }
            else {
                auto attackable = check4(test, i);
                if (attackable->rank != NONE) {
                    _attackables.push_back(attackable);
                } else {
                    auto attackable = check3(test, i);
                    if (attackable->rank != NONE) {
                        _attackables.push_back(attackable);
                    } else {
                        auto attackable = check(test, i);
                        if (attackable->rank != NONE) {
                            _attackables.push_back(attackable);
                        }
                    }
                }
            }
        }
        old = test->line[i];
        ++i;
    }
}

bool State::isForbidden(Position p, Block b, const Test &test) const
{
    bool isForbidden = true;

    size_t count33 = 0;
    auto check33 = [&](Test t) {
        auto iter = std::find(t->position.cbegin(), t->position.cend(), p);
        assert(iter != t->position.cend());
        size_t i = iter - t->position.cbegin();
        assert(i < t->line.size());
        t->line[i] = b;

        size_t count = 0;
        for (size_t j = i+1; j < t->line.size(); ++j) {
            if (t->line[j] != b)
                break;
            ++count;
        }

        bool isS3 = false;

        if (isS3 == false  && count < 3 && i > 0 && t->line[i - 1] == SPACE) {
            auto attackable = check3(t, i);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }

        if (isS3 == false && count < 2 && i > 1 && t->line[i - 1] == b && t->line[i - 2] == SPACE) {
            auto attackable = check3(t, i - 1);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }

        if (isS3 == false && count == 0 && i > 2 && t->line[i - 1] == b && t->line[i - 2] == b && t->line[i - 3] == SPACE) {
            auto attackable = check3(t, i - 2);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }

        if (isS3 == false && count < 2 && i > 2 && t->line[i - 1] == SPACE && t->line[i - 2] == b && t->line[i - 3] == SPACE) {
            auto attackable = check3(t, i - 2);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }
        if (isS3 == false && count == 0 &&  i > 3 && t->line[i - 1] == SPACE && t->line[i - 2] == b && t->line[i - 3] == b && t->line[i  - 4] == SPACE) {
            auto attackable = check3(t, i - 3);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }
        if (isS3 == false && count == 0 && i > 3 && t->line[i - 1] == b && t->line[i - 2] == SPACE && t->line[i - 3] == b && t->line[i - 4] == SPACE) {
            auto attackable = check3(t, i - 3);
            if (attackable->rank == S_THREE) {
                isS3 = true;
            }
        }
        if (isS3) {
            ++count33;
        }
    };

    if (get(p) == SPACE) {
        isForbidden = false;
        auto vertical = getVertical(p);
        auto horizontal = getHorizontal(p);
        auto diagonalA = getDiagonalA(p);
        auto diagonalB = getDiagonalB(p);
        if (test) {
            if (vertical->position.front() == test->position.front()
                && vertical->position.back() == test->position.back()) {
                vertical = test;
            }
            else if (horizontal->position.front() == test->position.front()
                && horizontal->position.back() == test->position.back()) {
                horizontal = test;
            }
            else if (diagonalA->position.front() == test->position.front()
                && diagonalA->position.back() == test->position.back()) {
                diagonalA = test;
            }
            else if (diagonalB->position.front() == test->position.front()
                && diagonalB->position.back() == test->position.back()) {
                diagonalB = test;
            }
        }

        Rule *rule = b == BLACK ? Rule::getBlack() : Rule::getWhite();
        if (rule->getThreeAndThree() == false) {
            check33(vertical);
            check33(horizontal);
            if (diagonalA->line.size() > 0) {
                check33(diagonalA);
            }
            if (diagonalB->line.size() > 0) {
                check33(diagonalB);
            }
            if (count33 > 1) {
                isForbidden = true;
            }
        }
    }
    return isForbidden;
}

bool State::check5(const Test &test, std::size_t start) const
{
    assert(start < test->line.size());
    Block b = test->line[start];
    assert(b == WHITE || b == BLACK);
    const Rule *r = (b == BLACK) ? Rule::getBlack() : Rule::getWhite();
    bool overline = r->getOverlines();

    return (start + 4 < test->line.size())
        && (overline || start == 0 || test->line[start - 1] != b)
        && (test->line[start + 1] == b)
        && (test->line[start + 2] == b)
        && (test->line[start + 3] == b)
        && (test->line[start + 4] == b)
        && (overline || start + 5 == test->line.size() || test->line[start + 5] != b);
}

std::shared_ptr<State::Attackable> State::check4(const Test &test, std::size_t start) const
{
    assert(test->line.size() == test->position.size());
    assert(start < test->line.size());
    Block b = test->line[start];
    assert(b == WHITE || b == BLACK);

    std::shared_ptr<State::Attackable> attackable = std::make_shared<Attackable>();
    auto testFunc = [&](size_t space, size_t start) {
        Test t = std::make_shared<Tst>();
        t->line = test->line;
        t->position = test->position;
        t->line[space] = b;
        if (check5(t, start) && !isForbidden(test->position[space], b, t)) {
            attackable->chances.push_back(test->position[space]);
        }
    };

    if (start + 4 < test->line.size() && test->line[start + 4] == b) {
        size_t count = 0;
        std::vector<size_t> blank;
        for (size_t i = start + 1; i < start + 4; ++i) {
            Block t = test->line[i];
            if (t == b) {
                ++count;
            } else if (t == SPACE) {
                blank.push_back(i);
            } else {
                break;
            }
        }
        if (blank.size() == 1 && count == 2) {
            testFunc(blank[0], start);
        }
    } else {
        size_t count = 0;
        for (size_t i = start; i < start + 4; ++i) {
            if (test->line[i] == b) {
                ++count;
            }
            else {
                break;
            }
        }
        if (count == 4) {
            if (start > 0 && test->line[start - 1] == SPACE) {
                testFunc(start - 1, start - 1);
            }
            if (start + 4 < test->line.size() && test->line[start + 4] == SPACE) {
                testFunc(start + 4, start);
            }
        }
    }
    

    attackable->player = b;
    switch (attackable->chances.size()) {
    case 0:
        attackable->rank = NONE;
        break;
    case 1:
        attackable->rank = FOUR;
        break;
    case 2:
        attackable->rank = S_FOUR;
        break;
    default:
        assert(false);
    }


    return attackable;
}

std::shared_ptr<State::Attackable> State::check3(const Test &test, std::size_t start) const
{
    assert(test->line.size() == test->position.size());
    assert(start < test->line.size());
    Block b = test->line[start];
    assert(b == WHITE || b == BLACK);

    std::shared_ptr<State::Attackable> attackable = std::make_shared<Attackable>();
    Rank rank = NONE;
    auto testFunc = [&](size_t space, size_t start) {
        if (!isForbidden(test->position[space], b, test)) {
            Test t = std::make_shared<Tst>();
            t->line = test->line;
            t->position = test->position;
            t->line[space] = b;
            auto result = check4(t, start);
            if (result->rank == S_FOUR || result->rank == FOUR) {
                rank = result->rank == S_FOUR ? S_THREE : THREE;
                attackable->chances.push_back(test->position[space]);
            }
        }
    };

    
    if (start + 2 < test->line.size() &&test->line[start + 1] == b && test->line[start + 2] == b) {
        if (start > 0 && test->line[start - 1] == SPACE) {
            testFunc(start - 1, start - 1);
        }

        if (start+3 < test->line.size() && test->line[start + 3] == SPACE) {
            testFunc(start + 3, start);
        }

    }

    if (start + 3 < test->line.size() && test->line[start + 1] == SPACE && test->line[start + 2] == b && test->line[start + 3] == b) {
        testFunc(start + 1, start);
    }
    if (start + 3 < test->line.size() && test->line[start + 1] == b && test->line[start + 2] == SPACE && test->line[start + 3] == b) {
        testFunc(start + 2, start);
    }
    

    attackable->player = b;
    attackable->rank = rank;

    return attackable;
}

std::shared_ptr<State::Attackable> State::check(const Test & test, std::size_t start) const
{
    assert(test->line.size() == test->position.size());
    assert(start < test->line.size());
    Block b = test->line[start];
    assert(b == WHITE || b == BLACK);

    std::shared_ptr<State::Attackable> attackable = std::make_shared<Attackable>();
    attackable->rank = NONE;

    size_t count = 0;
    size_t pieceCount = 0;
    size_t i = start - 1;
    while (i < test->line.size() && start - i < 5) {
        Block block = test->line[i];
        if (block == SPACE) {
            attackable->chances.push_back(test->position[i]);
        } else {
            break;
        }
        --i;
        ++count;
    }

    i = start;
    while (i < test->line.size() && i - start < 5) {
        Block block = test->line[i];
        if (block == SPACE) {
            attackable->chances.push_back(test->position[i]);
        } else if (block == b && pieceCount == 0) {
            ++pieceCount;
        } else {
            break;
        }
        ++i;
        ++count;
    }

    if (count >= 5) {
        attackable->player = b;
        if (pieceCount == 0) {
            attackable->rank = ONE;
        } else if (pieceCount == 1) {
            attackable->rank = TWO;
        } else {
            assert(false);
        }
    }
    return attackable;
}

#include "OmokAI.h"

#include <cassert>
#include <limits>

#include "Piece.h"

OmokAI::OmokAI(std::shared_ptr<OmokGame> game) :_game(game), _depth(5), _width(12)
{
    f = std::bind(&OmokAI::first, this);
    _beginning = myclock::now();
}


OmokAI::~OmokAI()
{
}

void OmokAI::search()
{
    f();
}

void OmokAI::setDepth(size_t depth)
{
    _depth = depth;
}

void OmokAI::setWidth(size_t width)
{
    _width = width;
}

void OmokAI::reset()
{
    f = std::bind(&OmokAI::first, this);
    _root.reset();
}

void OmokAI::first()
{
    _root = std::make_shared<OmokNode>();
    size_t size = _game->getBoardSize();
    _root->state = std::make_shared<State>(size);

    Position p(size / 2, size / 2);
    assert(_game->isBlackTurn());
    _game->put(p);
    _root->state->set(Piece::createBlack(p));

    f = std::bind(&OmokAI::second, this);
}

void OmokAI::second()
{
    myclock::duration d = myclock::now() - _beginning;
    auto seed = d.count();
    _generator.seed(seed);
    
    size_t size = _game->getBoardSize();
    Position w = _game->getPreviousWhite();
    _root->state->set(Piece::createWhite(w));

    std::uniform_int_distribution<int> distribution(0, 1);
    int number = distribution(_generator);
    int sign = number == 0 ? 1 : -1;

    Position next(0,0);
    if (w == Position(size / 2 + 1, size / 2)) {
        next = Position(size / 2 + 1, size / 2 + sign);
    } else if (w == Position(size / 2 - 1, size / 2)) {
        next = Position(size / 2 - 1, size / 2 + sign);
    } else if (w == Position(size / 2, size / 2 - 1)) {
        next = Position(size / 2 + sign, size / 2 - 1);
    } else if (w == Position(size / 2, size / 2 + 1)) {
        next = Position(size / 2 + sign, size / 2 + 1);
    } else if (w == Position(size / 2 + 1, size / 2 + 1)
        || w == Position(size / 2 - 1, size / 2 - 1)) {
        next = Position(size / 2 + sign, size / 2 - sign);
    } else if (w == Position(size / 2 + 1, size / 2 - 1)
        || w == Position(size / 2 - 1, size / 2 + 1)) {
        next = Position(size / 2 + sign, size / 2 + sign);
    } else {
        if (w.getX() > size / 2 && w.getY() >= size / 2) {
            next = Position(size / 2 - number, size / 2 - 1);
        } else if (w.getX() <= size / 2 && w.getY() > size / 2) {
            next = Position(size / 2 + 1, size / 2 - number);
        } else if (w.getX() < size / 2 && w.getY() <= size / 2) {
            next = Position(size / 2 + number, size / 2 + 1);
        } else {
            next = Position(size / 2 - 1, size / 2 + number);
        }
    }

    assert(_game->isBlackTurn());
    _game->put(next);
    _root->state->set(Piece::createBlack(next));

    f = std::bind(&OmokAI::normalSearch, this);
}

void OmokAI::normalSearch()
{
    Position w = _game->getPreviousWhite();
    auto iter = _root->children.find(w);
    if (iter == _root->children.cend()) {
        _root->state->set(Piece::createWhite(w));
        _root->children.clear();
    } else {
        _root = iter->second;
    }

    double alpha = -std::numeric_limits<double>::infinity();
    double beta = std::numeric_limits<double>::infinity();

    std::shared_ptr<OmokNode> maxOmokNode;
    Position maxMove(-1, -1);

    if (!_root->state->isWin()) {
        if (_root->children.size() == 0) {
            buildChildren(_root, 0);
        }
        for (auto &node : _root->children) {
            double a = alphabeta(node.second, alpha, beta, 1);
            if (a > alpha) {
                alpha = a;
                maxOmokNode = node.second;
                maxMove = node.first;
            }
            if (beta <= alpha) { // beta cut-off
                break;
            }
        }

        assert(_game->isBlackTurn());
        _game->put(maxMove);
        assert(maxOmokNode);
        _root = maxOmokNode;
    }
    
}

double OmokAI::alphabeta(std::shared_ptr<OmokNode> node, double alpha, double beta, size_t depth)
{
    size_t size = _game->getBoardSize();
    if (depth == _depth || node->state->isWin()) {
        return node->state->getScore();
        /*if (depth % 2 == 1) {
            return node->state->getBlackScore();
        }
        else {
            return -node->state->getWhiteScore();
        }*/
        
    } else {
        if (node->children.size() == 0) {
            buildChildren(node, depth);
        }
        if (depth % 2 == 0) { //maximizing
            for (auto &node : node->children) {
                double a = alphabeta(node.second, alpha, beta, depth + 1);
                alpha = a > alpha? a : alpha;
                if (beta <= alpha) { // beta cut-off
                    break;
                }
            }
            return alpha;
        } else { // minimizing
            for (auto &node : node->children) {
                double b = alphabeta(node.second, alpha, beta, depth + 1);
                beta = b < beta ? b : beta;
                if (beta <= alpha) { // beta cut-off
                    break;
                }
            }
            return beta;
        }
    }
}

void OmokAI::buildChildren(std::shared_ptr<OmokNode> node, size_t depth)
{
    auto points = node->state->getNextPoints(_width);

    for (const auto &point : points) {
        auto newNode = std::make_shared<OmokNode>();
        newNode->state = std::make_shared<State>(*node->state.get());
        if (depth % 2 == 0) {
            newNode->state->set(Piece::createBlack(point));
        } else {
            newNode->state->set(Piece::createWhite(point));
        }
        node->children[point] = newNode;
    }
}
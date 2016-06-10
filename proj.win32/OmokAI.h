#ifndef  _OMOKAI_H_
#define  _OMOKAI_H_

#include <cstddef>
#include <unordered_map>
#include <functional>
#include <memory>
#include <random>
#include <chrono>

#include "OmokGame.h"
#include "State.h"
#include "Position.h"

struct OmokNode {
    std::shared_ptr<State> state;
    std::unordered_map<Position, std::shared_ptr<OmokNode>> children;
};

class OmokAI
{
public:
    OmokAI(std::shared_ptr<OmokGame> game);
    virtual ~OmokAI();

    void search();

    void setDepth(size_t depth);
    void setWidth(size_t width);

    void reset();

protected:
    
    void first();
    void second();
    void normalSearch();

    double alphabeta(std::shared_ptr<OmokNode> node, double alpha, double beta, size_t depth);

    std::function<void()> f;

private:
    typedef std::chrono::high_resolution_clock myclock;

    void buildChildren(std::shared_ptr<OmokNode> node, size_t depth);

    std::shared_ptr<OmokGame> _game;
    std::shared_ptr<OmokNode> _root;
    std::size_t _depth;
    std::size_t _width;
    std::mt19937_64 _generator;
    myclock::time_point _beginning;
};


#endif // _OMOKAI_H_
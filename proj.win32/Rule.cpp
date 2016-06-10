#include "Rule.h"

Rule Rule::_black;
Rule Rule::_white;
Rule::Opening Rule::_opening = Opening::FREE;

Rule::Rule() : _overlines(false), _threeAndThree(false), _fourAndFour(true)
{
}

Rule::~Rule()
{
}

Rule * Rule::getBlack()
{
    return &_black;
}

Rule * Rule::getWhite()
{
    return &_white;
}

void Rule::setOverlines(bool allow)
{
    _overlines = allow;
}

bool Rule::getOverlines() const
{
    return _overlines;
}

void Rule::setThreeAndThree(bool allow)
{
    _threeAndThree = allow;
}

bool Rule::getThreeAndThree() const
{
    return _threeAndThree;
}

void Rule::setFourAndFour(bool allow)
{
    _fourAndFour = allow;
}

bool Rule::getFourAndFour() const
{
    return _fourAndFour;
}

void Rule::setOpening(Rule::Opening opening)
{
    _opening = opening;
}

Rule::Opening Rule::getOpening()
{
    return _opening;
}

#ifndef  _RULE_H_
#define  _RULE_H_

#include <unordered_map>

class Rule
{
public:
    enum Opening {FREE, SWAP1, SWAP2, RENJU, RIF};

    Rule();
    virtual ~Rule();

    static Rule* getBlack();
    static Rule* getWhite();

    void setOverlines(bool allow);
    bool getOverlines() const;

    void setThreeAndThree(bool allow);
    bool getThreeAndThree() const;

    void setFourAndFour(bool allow);
    bool getFourAndFour() const;

    static void setOpening(Rule::Opening opening);
    static Rule::Opening getOpening();

private:
    static Rule _black;
    static Rule _white;

    bool _overlines;
    bool _threeAndThree;
    bool _fourAndFour;
    static Opening _opening;
};

#endif // _RULE_H_
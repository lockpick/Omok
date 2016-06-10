#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "proj.win32/Position.h"
#include "proj.win32/OmokGame.h"
#include "proj.win32/OmokAI.h"

#include <memory>

class GameScene : public cocos2d::LayerColor
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    void printBlackStone(Position pos);
    void printWhiteStone(Position pos);
    void blackWin();
    void whiteWin();

private:
    
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void onMouseScroll(cocos2d::Event* event);

    const cocos2d::Vec2 convertP2V(Position pos) const;
    const Position convertV2P(cocos2d::Vec2 v) const;

    cocos2d::DrawNode* createBoard(int board_size = 15, cocos2d::Color4F board_color = cocos2d::Color4F(1.0f, 217.0f / 255.0f, 166.0f / 255.0f, 1.0f));

    cocos2d::DrawNode* createWhiteStone(Position pos);
    cocos2d::DrawNode* createBlackStone(Position pos);

    void reset(cocos2d::Ref *pSender);

    cocos2d::Vec2 _origin;
    cocos2d::Vec2 _destination;

    float _min;
    cocos2d::Vec2 _mid;
    cocos2d::Vec2 _size;
    float _interval;

    size_t number_stone;

    std::shared_ptr<OmokGame> _game;
    std::shared_ptr<OmokAI> _ai;
};

#endif // __HELLOWORLD_SCENE_H__

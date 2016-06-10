#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B(0xDC,0xDC,0xDC,0xFF)) )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    _min = visibleSize.width < visibleSize.height ? visibleSize.width : visibleSize.height;
    _mid = Vec2(visibleSize.width / 2.0f, visibleSize.height / 2.0f);
    _size = Vec2(_min, _min);
    _origin = _mid - _size/2.2f;
    _destination = _mid + _size / 2.2f;

    number_stone = 0;

    _game = std::make_shared<OmokGame>(this);
    _ai = std::make_shared<OmokAI>(_game);


    auto board = createBoard(_game->getBoardSize());
    this->addChild(board, 1);

    auto Mouse = EventListenerMouse::create();
    Mouse->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    Mouse->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    Mouse->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMove, this);
    Mouse->onMouseScroll = CC_CALLBACK_1(GameScene::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

    
    auto label_reset = Label::createWithSystemFont("Reset", "Airal", _min/18);
    label_reset->setColor(Color3B::BLACK);
    auto menuItem = MenuItemLabel::create(label_reset, CC_CALLBACK_1(GameScene::reset, this));
    menuItem->setAnchorPoint(Vec2(0.0f, 1.0f));
    auto menu = Menu::create(menuItem, NULL);
    menu->setAnchorPoint(Vec2(0.0f, 1.0f));
    menu->setPosition(_destination + Vec2(_min*0.1f, 0));
    this->addChild(menu, 10);

    /**
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);
    **/

    _ai->search();

    return true;
}

void GameScene::printBlackStone(Position pos)
{
    auto node = createBlackStone(pos);    this->addChild(node, 2);
}

void GameScene::printWhiteStone(Position pos)
{
    auto node = createWhiteStone(pos);    this->addChild(node, 2);
}

void GameScene::blackWin()
{
}

void GameScene::whiteWin()
{
}

void GameScene::onMouseDown(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 clickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    auto button = mouseEvent->getMouseButton();
}
void GameScene::onMouseUp(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 clickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    auto buttom = mouseEvent->getMouseButton();
    if (buttom == MOUSE_BUTTON_LEFT && !_game->isGameOver()) {
        Position p = convertV2P(clickPoint);
        if (p.getX() < _game->getBoardSize() && p.getY() < _game->getBoardSize()) {
            bool playerTurn = !_game->isBlackTurn();            if (playerTurn) {                bool success = _game->put(p);                if (success && !_game->isGameOver()) {                    _ai->search();                }            }
        }
    }
}
void GameScene::onMouseMove(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 clickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
}
void GameScene::onMouseScroll(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 clickPoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
}


const cocos2d::Vec2 GameScene::convertP2V(Position pos) const
{
    return Vec2(_origin.x + pos.getX()*_interval, _origin.y + pos.getY()*_interval);
}

const Position GameScene::convertV2P(cocos2d::Vec2 v) const
{
    Vec2 vec = v - _origin;
    int x = (vec.x + _interval / 2.0f) / _interval;
    int y = (vec.y + _interval / 2.0f) / _interval;
    return Position(x, y);
}

cocos2d::DrawNode * GameScene::createBoard(int board_size, cocos2d::Color4F board_color)
{
    auto drawNode = DrawNode::create();
    drawNode->setLineWidth(1);

    drawNode->drawSolidRect(_mid - _size / 2.1f, _mid + _size / 2.1f, board_color);
    Vec2 o = _origin;
    Vec2 d = _destination;
    drawNode->drawSolidRect(o, d, board_color);
    drawNode->drawRect(o, d, Color4F::BLACK);
    float interval = (d.x - o.x) / (board_size - 1.0f);
    for (int i = 0; i < (board_size - 1); i++) {
        drawNode->drawLine(Vec2(o.x + i*interval, o.y), Vec2(o.x + i*interval, d.y), Color4F::BLACK);
        drawNode->drawLine(Vec2(o.x, o.y + i*interval), Vec2(d.x, o.y + i*interval), Color4F::BLACK);
    }
    drawNode->drawDot(Vec2(o.x + board_size / 2 * interval, o.y + board_size / 2 * interval), _min / 150.0f, Color4F::BLACK);
    drawNode->drawDot(Vec2(o.x + board_size / 4 * interval, o.y + board_size / 4 * interval), _min / 150.0f, Color4F::BLACK);
    drawNode->drawDot(Vec2(o.x + board_size / 4 * interval, d.y - board_size / 4 * interval), _min / 150.0f, Color4F::BLACK);
    drawNode->drawDot(Vec2(d.x - board_size / 4 * interval, o.y + board_size / 4 * interval), _min / 150.0f, Color4F::BLACK);
    drawNode->drawDot(Vec2(d.x - board_size / 4 * interval, d.y - board_size / 4 * interval), _min / 150.0f, Color4F::BLACK);

    _interval = interval;
    return drawNode;
}

cocos2d::DrawNode * GameScene::createWhiteStone(Position pos)
{
    auto drawNode = DrawNode::create();
    drawNode->setLineWidth(1);
    drawNode->drawSolidCircle(convertP2V(pos), _min / 40.0f, CC_DEGREES_TO_RADIANS(360), 16u, Color4F::WHITE);
    drawNode->drawCircle(convertP2V(pos), _min / 40.0f, CC_DEGREES_TO_RADIANS(360), 36u, false, Color4F::BLACK);
    drawNode->setTag(1);
    ++number_stone;
    return drawNode;
}

cocos2d::DrawNode * GameScene::createBlackStone(Position pos)
{
    auto drawNode = DrawNode::create();
    drawNode->setLineWidth(1);
    drawNode->drawSolidCircle(convertP2V(pos), _min / 40.0f, CC_DEGREES_TO_RADIANS(360), 16u, Color4F::BLACK);
    drawNode->setTag(1);
    ++number_stone;
    return drawNode;
}

void GameScene::reset(cocos2d::Ref * pSender)
{
    _game->reset();
    _ai->reset();
    while (number_stone != 0) {
        this->removeChildByTag(1);
        --number_stone;
    }
    _ai->search();
    
}

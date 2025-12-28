#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "NPC/NPC.h" 

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // 处理键盘事件
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // 每帧更新
    void update(float delta);


    // 鼠标/触摸事件
    void onMouseDown(cocos2d::Event* event); // 鼠标右键点击

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

private:
    // 地图精灵
    cocos2d::Sprite* mapSprite;
    // 瓦片地图
    cocos2d::TMXTiledMap* tileMap;

    // 地图大小
    cocos2d::Size mapSize;
    // 缩放后的地图大小
    cocos2d::Size scaledMapSize;
    // 视图大小
    cocos2d::Size viewSize;
    // 地图位置
    cocos2d::Vec2 mapPosition;

    // 移动速度
    float moveSpeed;

    // 键盘按键状态
    bool keys[256];

    // NPC 相关
    NPC* _npc;


};

#endif // __GAME_SCENE_H__
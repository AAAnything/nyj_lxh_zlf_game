#ifndef __NPC_TEST_SCENE_H__
#define __NPC_TEST_SCENE_H__

#include "cocos2d.h"
#include "NPC.h"

class NPCTestScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(NPCTestScene);

private:
    NPC* _testNPC;

    // 鼠标事件
    void onMouseDown(cocos2d::Event* event);

    // 更新函数，检查玩家是否在NPC附近
    void update(float delta);

    // 创建UI
    void createUI();
};

#endif // __NPC_TEST_SCENE_H__

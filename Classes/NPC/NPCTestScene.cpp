#include "NPCTestScene.h"
#include "Core/MainMenuScene.h"

USING_NS_CC;

// 1. 创建一个场景对象（空的容器）
//    Scene 就像一张空白的画布
// 2. 创建一个图层对象（包含游戏逻辑）
//    NPCTestScene 是你写的类，继承自 Layer
//    包含了你的NPC、背景、按钮等所有内容
// 3. 把图层添加到场景中
//    把内容（图层）放到画布（场景）上
// 4. 返回这个完整的场景
Scene* NPCTestScene::createScene()
{
    auto scene = Scene::create();
    auto layer = NPCTestScene::create();
    scene->addChild(layer);
    return scene;
}


bool NPCTestScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    

    // 1. 创建NPC
    _testNPC = NPC::create("npcImages/Haley.png");
    if (_testNPC)
    {
        // 设置NPC属性
        _testNPC->setName("NPC");

        // 设置对话内容
        std::vector<std::string> dialogue = {
            "Hello, world!",
            "Recently, some strange creatures have appeared near the village.",
            "If you need help, you can go to Master Wang at the blacksmith shop.",
            "Good luck!"
        };
        _testNPC->setDialogue(dialogue);

        // 设置位置
        _testNPC->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
        this->addChild(_testNPC, 1);

        // 设置对话结束回调
        _testNPC->setDialogueEndCallback([]() {
            CCLOG("对话结束回调被调用");
            });
    }


    // 3. 添加鼠标事件监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(NPCTestScene::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    

    // 4. 启用更新函数，用于检测玩家是否在NPC附近
    this->scheduleUpdate();

    return true;
}



// 鼠标右键点击事件
void NPCTestScene::onMouseDown(Event* event)
{
    // 将事件传递给NPC处理
    if (_testNPC)
    {
        _testNPC->onMouseDown(event);
    }
}



// 更新函数：检查鼠标/玩家位置是否在NPC附近
void NPCTestScene::update(float delta)
{
    // 获取鼠标位置
    auto mousePos = Director::getInstance()->getVisibleOrigin();

    // 或者获取玩家位置
    // Vec2 playerPos = _player->getPosition();

    // 遍历场景中所有NPC，检查是否在交互范围内
    for (auto child : this->getChildren())
    {
        NPC* npc = dynamic_cast<NPC*>(child);
        if (npc)
        {
            // 检查是否在范围内（这里用鼠标位置代替玩家位置）
            bool inRange = npc->isInRange(mousePos);
        }
    }
}
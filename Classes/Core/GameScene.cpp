#include"GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init())
        return false;

    // 获取视图大小
    viewSize = Director::getInstance()->getVisibleSize();

    // 输出调试信息
    CCLOG("View size: %f x %f", viewSize.width, viewSize.height);

    // 使用瓦片地图系统
    // 加载TMX瓦片地图
    tileMap = cocos2d::TMXTiledMap::create("Maps/farmSpring/farmMap.tmx");

    if (tileMap)
    {
        // 获取地图大小
        mapSize = cocos2d::Size(tileMap->getMapSize().width * tileMap->getTileSize().width,
            tileMap->getMapSize().height * tileMap->getTileSize().height);

        // 输出调试信息
        CCLOG("Map size: %f x %f", mapSize.width, mapSize.height);
        CCLOG("Tile size: %f x %f", tileMap->getTileSize().width, tileMap->getTileSize().height);
        CCLOG("Map grid: %d x %d", tileMap->getMapSize().width, tileMap->getMapSize().height);

        // 设置地图位置（初始在地图中心）
        mapPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

        // 计算缩放比例，计算地图完全显示在视图中的比例
        float scaleX = viewSize.width / mapSize.width;
        float scaleY = viewSize.height / mapSize.height;
        float minScale = std::min(scaleX, scaleY);

        // 输出调试信息
        CCLOG("Scale factors - X: %f, Y: %f, Min: %f", scaleX, scaleY, minScale);

        // 设置地图缩放 - 放大地图，让窗口只显示地图的一部分
        // 这里使用1.5倍的最小缩放比例，可以根据需要调整这个值
        float desiredScale = minScale * 1.5f;
        tileMap->setScale(desiredScale);

        // 重新计算缩放后的地图大小
        scaledMapSize = cocos2d::Size(mapSize.width * desiredScale, mapSize.height * desiredScale);

        // 输出调试信息
        CCLOG("Desired scale: %f", desiredScale);
        CCLOG("Scaled map size: %f x %f", scaledMapSize.width, scaledMapSize.height);

        // 使用标准锚点(0.5, 0.5)，边界计算更简单
        tileMap->setAnchorPoint(Vec2(0.5f, 0.5f));
        tileMap->setPosition(Vec2(viewSize.width / 2, viewSize.height / 2));

        // 添加地图到场景
        this->addChild(tileMap, 0);

        CCLOG("Tile map added successfully with scale: %f", desiredScale);
        CCLOG("Map initialized with anchor point: (%f, %f)", tileMap->getAnchorPoint().x, tileMap->getAnchorPoint().y);

        // 可以在这里获取特定的图层和瓦片，用于实现交互功能
        // 例如：auto groundLayer = tileMap->getLayer("Ground");
        // auto tileGID = groundLayer->getTileGIDAt(Vec2(tileX, tileY));
        // auto properties = tileMap->getPropertiesForGID(tileGID).asValueMap();


         // 2. 初始化采集管理器
         //    设置可以采集的物品
        CollectManager::getInstance()->initialize(tileMap);
    }
    else
    {
        CCLOG("Failed to load tile map!");

        // 如果瓦片地图加载失败，创建一个红色方块作为替代
        auto placeholder = cocos2d::Sprite::create();
        placeholder->setTextureRect(cocos2d::Rect(0, 0, 200, 200));
        placeholder->setColor(cocos2d::Color3B(255, 0, 0));
        placeholder->setPosition(Vec2(viewSize.width / 2, viewSize.height / 2));
        this->addChild(placeholder, 1);

        CCLOG("Added red placeholder sprite");
    }


    // 3. 创建NPC
    _npc = NPC::create("npcImages/Haley.png");
    if (_npc)
    {
        _npc->setName("Haley");

        // 设置NPC对话
        std::vector<std::string> dialogue = {
            "Hello, welcome to our farm!",
            "Be careful, there are monsters nearby.",
            "You can harvest crops by pressing E.",
            "Good luck!"
        };
        _npc->setDialogue(dialogue);

        // 设置NPC初始位置（地图坐标，可根据需要调整）
        // 这里以地图中心偏移(0, 100)为例，可根据实际地图修改
        Vec2 npcMapPos = Vec2(mapSize.width / 2, mapSize.height / 2 + 100);
        // 将地图坐标转换为场景坐标（考虑地图缩放和锚点）
        Vec2 npcScenePos = tileMap->convertToNodeSpace(npcMapPos);
        _npc->setPosition(npcScenePos);

        // 添加到地图图层（确保显示在地图上方）
        tileMap->addChild(_npc, 10);

        // 设置对话结束回调
        _npc->setDialogueEndCallback([]() {
            CCLOG("NPC dialogue ended");
            });
    }

    // 创建玩家角色
    player = Player::create();
    if (player)
    {
        // 设置玩家初始位置为地图中心
        player->setPosition(Vec2(mapSize.width / 2, mapSize.height / 2));
        
        // 设置玩家移动速度
        player->setMoveSpeed(150.0f);
        
        // 设置动画帧率
        player->setAnimationFrameRate(10.0f);
        
        // 添加玩家到场景
        this->addChild(player, 10); // 玩家层级高于地图
        
        CCLOG("Player created successfully at position: (%f, %f)", mapSize.width / 2, mapSize.height / 2);
    }
    else
    {
        CCLOG("Failed to create player!");
    }

    // 设置移动速度（用于地图移动）
    moveSpeed = 200.0f;

    // 初始化键盘按键状态
    //memset(keys, false, sizeof(keys));

    // 设置键盘动作监听器
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 添加鼠标事件监听器（NPC调用）
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);


    // 开启更新
    scheduleUpdate();

    return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = true;
    
    if (player && player->getParent())
    {
        player->onKeyPressed(keyCode);
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = false;
    
    if (player && player->getParent())
    {
        player->onKeyReleased(keyCode);
    }
}

// 处理鼠标点击事件（与NPC有关）
void GameScene::onMouseDown(cocos2d::Event* event)
{
    if (_npc)
    {
        _npc->onMouseDown(event);  // 将事件传递给NPC处理
    }
}

void GameScene::update(float delta)
{
    // 更新玩家
    if (player)
    {
        player->update(delta);
        
        // 摄像机跟随玩家
        updateCamera();
    }
}

void GameScene::updateCamera()
{
    if (!player || !tileMap) return;
    
    // 获取玩家位置
    Vec2 playerPos = player->getPlayerPosition();
    
    // 更新地图位置（摄像机跟随）
    mapPosition = playerPos;
    
    // 限制地图位置在地图边界内
    float visibleMapWidth = viewSize.width / tileMap->getScale();
    float visibleMapHeight = viewSize.height / tileMap->getScale();
    
    float minX = visibleMapWidth / 2;
    float maxX = mapSize.width - visibleMapWidth / 2;
    float minY = visibleMapHeight / 2;
    float maxY = mapSize.height - visibleMapHeight / 2;
    
    minX = std::max(0.0f, minX);
    maxX = std::min(mapSize.width, maxX);
    minY = std::max(0.0f, minY);
    maxY = std::min(mapSize.height, maxY);
    
    if (mapSize.width > viewSize.width)
    {
        mapPosition.x = clampf(mapPosition.x, minX, maxX);
    }
    else
    {
        mapPosition.x = mapSize.width / 2;
    }
    
    if (mapSize.height > viewSize.height)
    {
        mapPosition.y = clampf(mapPosition.y, minY, maxY);
    }
    else
    {
        mapPosition.y = mapSize.height / 2;
    }
    
    // 设置地图位置（反向移动地图以模拟摄像机移动）
    tileMap->setPosition(Vec2(viewSize.width / 2 - (mapPosition.x - mapSize.width / 2) * tileMap->getScale(),
                             viewSize.height / 2 - (mapPosition.y - mapSize.height / 2) * tileMap->getScale()));
}

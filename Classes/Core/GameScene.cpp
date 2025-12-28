#include "GameScene.h"

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

    // 设置移动速度
    moveSpeed = 200.0f;

    // 初始化键盘按键状态
    memset(keys, false, sizeof(keys));

    // 设置键盘动作监听器
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 开启更新
    scheduleUpdate();


    // 1. 创建斧子实例 (调用 Item::init 设置基本信息)
    auto axe = Item::create("xe_001", "axe", "tools/axe.png");

    // 2. 为斧子增加开始的属性 (逻辑初始化)
    axe->setMaxStackSize(1);      // 斧子不可堆叠，最大 1 个
    axe->setCurrentStackSize(1);  // 当前拥有 1 个
    axe->setIsUsable(true);       // 斧子是可用的
    axe->setState(1);             // 假设 1 代表耐久度满

    // 创建背包实例
    auto playerInventory = Inventory::create(10); // 例如10格
    // addItem 会自动寻找第一个空槽位 (调用了你代码里的 findEmptySlot)
    playerInventory->addItem(axe);
    auto inventoryUI = InventoryUI::create(playerInventory);
    this->addChild(inventoryUI, 10); // zOrder 10 保证在地图之上
    // 5. 创建 UI 并把数据传进去
    auto ui = InventoryUI::create(playerInventory);
    this->addChild(ui, 100);


    return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 记录按键按下状态，将KeyCode转换为整数
    keys[static_cast<int>(keyCode)] = true;
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 记录按键释放状态，将KeyCode转换为整数
    keys[static_cast<int>(keyCode)] = false;
}

void GameScene::update(float delta)
{
    // 计算移动方向
    Vec2 moveDirection(0, 0);

    // 检测WASD键，将KeyCode转换为整数
    if (keys[static_cast<int>(EventKeyboard::KeyCode::KEY_W)])
    {
        moveDirection.y += 1;
    }
    if (keys[static_cast<int>(EventKeyboard::KeyCode::KEY_S)])
    {
        moveDirection.y -= 1;
    }
    if (keys[static_cast<int>(EventKeyboard::KeyCode::KEY_A)])
    {
        moveDirection.x -= 1;
    }
    if (keys[static_cast<int>(EventKeyboard::KeyCode::KEY_D)])
    {
        moveDirection.x += 1;
    }

    // 统一移动方向
    if (moveDirection != Vec2::ZERO)
    {
        moveDirection.normalize();
    }

    // 计算新的地图位置
    Vec2 newPosition = mapPosition + moveDirection * moveSpeed * delta;

    // 限制地图位置在地图边界内
    // 确保视图不会超出地图范围
    // 正确的边界计算需要考虑地图的实际大小、缩放比例和锚点
    float visibleMapWidth = viewSize.width / tileMap->getScale();
    float visibleMapHeight = viewSize.height / tileMap->getScale();

    // 计算实际的可见区域边界
    // 对于X轴，使用标准边界计算
    float minX = visibleMapWidth / 2;
    float maxX = mapSize.width - visibleMapWidth / 2;

    // 对于Y轴，也使用标准边界计算
    // 锚点的影响在地图位置更新时处理，而不是在边界计算时
    float minY = visibleMapHeight / 2;
    float maxY = mapSize.height - visibleMapHeight / 2;

    // 确保边界值不会为负
    minX = std::max(0.0f, minX);
    maxX = std::min(mapSize.width, maxX);
    minY = std::max(0.0f, minY);
    maxY = std::min(mapSize.height, maxY);

    // 输出调试信息
    CCLOG("Boundary calculation - Visible map: %f x %f", visibleMapWidth, visibleMapHeight);
    CCLOG("Anchor point: (%f, %f)", tileMap->getAnchorPoint().x, tileMap->getAnchorPoint().y);
    CCLOG("Boundary limits - MinX: %f, MaxX: %f, MinY: %f, MaxY: %f", minX, maxX, minY, maxY);

    // 检查地图是否大于视图
    if (mapSize.width > viewSize.width)
    {
        newPosition.x = clampf(newPosition.x, minX, maxX);
    }
    else
    {
        newPosition.x = mapSize.width / 2;
    }

    if (mapSize.height > viewSize.height)
    {
        newPosition.y = clampf(newPosition.y, minY, maxY);
    }
    else
    {
        newPosition.y = mapSize.height / 2;
    }

    // 更新地图位置
    mapPosition = newPosition;

    // 更新地图的位置，实现视野移动效果
    // 当地图位置变化时，地图的位置需要反向移动
    if (tileMap)
    {
        // 计算地图精灵的位置偏移
        // 正确处理锚点的影响
        Vec2 anchor = tileMap->getAnchorPoint();
        float anchorOffsetX = (anchor.x - 0.5f) * (mapSize.width * tileMap->getScale() - viewSize.width);
        float anchorOffsetY = (anchor.y - 0.5f) * (mapSize.height * tileMap->getScale() - viewSize.height);

        Vec2 offset = Vec2(
            (mapSize.width / 2 - mapPosition.x) * tileMap->getScale() + anchorOffsetX,
            (mapSize.height / 2 - mapPosition.y) * tileMap->getScale() + anchorOffsetY
        );

        // 设置地图精灵的位置
        Vec2 tileMapPosition = Vec2(viewSize.width / 2, viewSize.height / 2) + offset;
        tileMap->setPosition(tileMapPosition);
    }
}
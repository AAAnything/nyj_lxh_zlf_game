#include "CollectSpot.h"
#include "CollectManager.h"  // 在 .cpp 中包含完整定义

CollectSpot* CollectSpot::create(const std::string& itemId, Vec2 position, const std::string& tileType) {
    CollectSpot* spot = new CollectSpot();
    if (spot && spot->init(itemId, tileType)) {
        spot->autorelease();
        spot->setPosition(position);  // 设置世界坐标
        return spot;
    }
    CC_SAFE_DELETE(spot);
    return nullptr;
}

bool CollectSpot::init(const std::string& itemId, const std::string& tileType) {
    if (!Node::init()) return false;

    // 初始化成员变量
    this->tileType = tileType;           // 保存瓦片类型
    collected = false;
    currentClicks = 0;
    collectRange = 200.0f;               // 采集范围200像素

    // 通过CollectManager获取采集物品
    item = CollectManager::getInstance()->getCollectItemById(itemId);

    if (item) {
        requiredClicks = item->getRequiredClicks();

        // 根据瓦片类型设置不同的精灵（功能1：显示对应的物体）
        std::string spriteName;
        if (tileType == "tree") {
            spriteName = "collect/tree.png";        // 树
        }
        else if (tileType == "rock") {
            spriteName = "collect/stone.png";       // 石头
        }
        else if (tileType == "grass") {
            spriteName = "collect/grass.png";       // 草
        }
        else if (tileType == "wood") {
            spriteName = "collect/wood.png";        // 木头
        }
        else if (tileType == "shell") {
            spriteName = "collect/shell.png";       // 贝壳
        }
        else {
            spriteName = "collect/default.png";    // 默认
        }

        sprite = Sprite::create(spriteName);
        if (sprite) {
            addChild(sprite, 0);

            // 设置锚点
            if (tileType != "tree")
                sprite->setAnchorPoint(Vec2(0.5f, 0.1f));
            else
                sprite->setAnchorPoint(Vec2(0.5f, 0));

            sprite->setScale(1.2f);
            /* ==== = 调试框代码 ==== =
            // 根据精灵实际大小绘制边框
            Size spriteSize = sprite->getContentSize();

            // 调试输出精灵大小
            CCLOG("CollectSpot: %s 精灵大小: %.0f x %.0f",
                tileType.c_str(), spriteSize.width, spriteSize.height);

            auto debugBox = DrawNode::create();
            // 因为精灵锚点是(0.5, 0)，所以边框要对应调整
            // 左下角: (-宽度/2, 0)
            // 右上角: (宽度/2, 高度)
            debugBox->drawRect(
                Vec2(-spriteSize.width / 2, 0),                    // 左下角
                Vec2(spriteSize.width / 2, spriteSize.height),    // 右上角
                Color4F::RED
            );
            this->addChild(debugBox, 100);

            // 添加调试输出
            auto label = Label::createWithSystemFont(
                StringUtils::format("%s\nsize:%.0fx%.0f",
                    tileType.c_str(), spriteSize.width, spriteSize.height),
                "Arial", 20);
            label->setPosition(Vec2(0, spriteSize.height + 20));
            label->setTextColor(Color4B::YELLOW);
            this->addChild(label, 101);*/
            // =======================
        }
    }
    else {
        // 如果没有物品，创建一个默认的调试框
        auto debugBox = DrawNode::create();
        debugBox->drawRect(Vec2(-32, -32), Vec2(32, 32), Color4F::RED);
        this->addChild(debugBox, 100);
    }




    // ==============================================
    // 添加鼠标事件监听
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(CollectSpot::onMouseDown, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    this->touchListener = mouseListener;
    // ==============================================


    return true;
}


// 在CollectSpot.cpp中实现鼠标事件处理
void CollectSpot::onMouseDown(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        // 将鼠标屏幕坐标转换为节点局部坐标（更准确的精灵范围内判定）
        Vec2 mouseWorldPos = this->convertToNodeSpace(Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY()));

        // 使用精灵自身的碰撞盒判断点击（替代原有的矩形范围判定）
        if (sprite && sprite->getBoundingBox().containsPoint(mouseWorldPos) && !collected) {
            bool completed = this->collect();
            if (completed && CollectManager::getInstance()) {
                CollectManager::getInstance()->handleCollectSuccess(this);
            }
        }
    }
}


// 功能2：判断玩家是否在采集范围内
bool CollectSpot::isPlayerInRange(Vec2 playerPos) const {


    // 先注释掉，进行调试
    /*if (collected) return false;  // 已采集的物品不在范围内

    float distance = playerPos.distance(getPosition());
    return distance <= collectRange;*/

    return true;
}

// 功能4：检查工具是否匹配
bool CollectSpot::canCollectWithTool(const std::string& currentTool) const {



    // 先注释掉，进行调试
    /*if (!item || collected) return false;

    std::string requiredTool = item->getRequiredTool();

    // 特殊处理：草可以用镰刀或斧子（功能5）
    if (tileType == "grass") {
        return currentTool == "sickle" || currentTool == "axe";
    }

    // 其他物品需要精确匹配工具
    return currentTool == requiredTool;*/


    return true;
}

// 采集操作（每次点击调用）
bool CollectSpot::collect() {
    if (collected || !item) return false;

    currentClicks++;

    // 功能3：播放抖动效果
    playShakeEffect();

    // 检查是否达到点击次数
    if (currentClicks >= requiredClicks) {
        collected = true;
        playCollectCompleteEffect();  // 播放完成效果
        return true;  // 采集完成
    }

    return false;  // 还需要继续点击
}

// 功能3：抖动效果实现
void CollectSpot::playShakeEffect() {
    if (!sprite) return;

    // 创建抖动动画序列：右-左-右-左-恢复
    auto moveRight = MoveBy::create(0.05f, Vec2(3, 0));
    auto moveLeft = MoveBy::create(0.05f, Vec2(-3, 0));
    auto moveRight2 = MoveBy::create(0.05f, Vec2(2, 0));
    auto moveLeft2 = MoveBy::create(0.05f, Vec2(-2, 0));

    sprite->runAction(Sequence::create(
        moveRight,
        moveLeft,
        moveRight2,
        moveLeft2,
        nullptr
    ));


}


// 采集完成效果
// 修改playCollectCompleteEffect函数，添加采集后图片替换逻辑
void CollectSpot::playCollectCompleteEffect() {
    if (!sprite) return;

    // 1. 先执行消失动画
    auto scaleDown = ScaleTo::create(0.3f, 0.1f);  // 0.3秒内缩小到0.1倍（接近消失）
    auto spawn = Spawn::create(scaleDown, nullptr);  // 只执行缩放动作

    // 2. 动画结束后显示采集后的状态图片
    auto showCollected = CallFunc::create([this]() {
        if (!sprite) return;

        // 根据物体类型设置对应的采集后图片
        std::string collectedSprite;
        if (tileType == "tree") {
            collectedSprite = "collect/groundTree.png";
        }
        else if (tileType == "rock") {
            collectedSprite = "collect/ground.png";
        }
        else if (tileType == "grass") {
            collectedSprite = "collect/ground.png";
        }
        else if (tileType == "wood") {
            collectedSprite = "collect/ground.png";
        }
        else if (tileType == "shell") {
            collectedSprite = "collect/beach.png";
        }

        // 创建新精灵替换原有精灵
        Sprite* newSprite = Sprite::create(collectedSprite);
        if (newSprite) {
            newSprite->setAnchorPoint(sprite->getAnchorPoint());  // 保持锚点一致
            newSprite->setScale(1.4f);  // 放大1.3倍
            this->removeChild(sprite);  // 移除原精灵
            this->addChild(newSprite);  // 添加新精灵
            sprite = newSprite;  // 更新精灵指针
        }
        });

    // 执行动画序列：消失 -> 显示采集后状态
    sprite->runAction(Sequence::create(spawn, showCollected, nullptr));
}
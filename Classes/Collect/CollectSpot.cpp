#include "CollectSpot.h"
#include "CollectManager.h"
#include "cocos2d.h"

USING_NS_CC;

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
        else {
            spriteName = "collect/default.png";    // 默认
        }

        sprite = Sprite::create(spriteName);
        if (sprite) {
            addChild(sprite, 0);

            // 设置锚点在底部中心，这样抖动效果更自然
            sprite->setAnchorPoint(Vec2(0.5f, 0));

            // ===== 修改调试框代码 =====
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
            // =======================
        }
    }
    else {
        // 如果没有物品，创建一个默认的调试框
        auto debugBox = DrawNode::create();
        debugBox->drawRect(Vec2(-32, -32), Vec2(32, 32), Color4F::RED);
        this->addChild(debugBox, 100);
    }

    return true;
}
// 功能2：判断玩家是否在采集范围内
bool CollectSpot::isPlayerInRange(Vec2 playerPos) const {
    if (collected) return false;  // 已采集的物品不在范围内

    float distance = playerPos.distance(getPosition());
    return distance <= collectRange;
}

// 功能4：检查工具是否匹配
bool CollectSpot::canCollectWithTool(const std::string& currentTool) const {
    if (!item || collected) return false;

    std::string requiredTool = item->getRequiredTool();

    // 特殊处理：草可以用镰刀或斧子（功能5）
    if (tileType == "grass") {
        return currentTool == "sickle" || currentTool == "axe";
    }

    // 其他物品需要精确匹配工具
    return currentTool == requiredTool;
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

    // 可选：添加粒子效果增强视觉反馈
    auto emitter = ParticleSystemQuad::create("particles/shake.plist");
    if (emitter) {
        emitter->setPosition(getPosition() + Vec2(0, 30));
        emitter->setAutoRemoveOnFinish(true);
        getParent()->addChild(emitter, 100);
    }
}

// 采集完成效果
void CollectSpot::playCollectCompleteEffect() {
    if (!sprite) return;

    // 缩放消失效果
    auto scaleDown = ScaleTo::create(0.3f, 0.1f);
    auto fadeOut = FadeOut::create(0.3f);
    auto spawn = Spawn::create(scaleDown, fadeOut, nullptr);


    // 延迟后移除节点
    auto delay = DelayTime::create(0.5f);
    auto remove = RemoveSelf::create();

    sprite->runAction(Sequence::create(spawn, delay, remove->clone(), nullptr));
}
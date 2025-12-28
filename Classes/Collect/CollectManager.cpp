#include "CollectManager.h"
#include "Core/GameManager.h"
#include "cocos2d.h"

USING_NS_CC;


// 静态成员初始化
CollectManager* CollectManager::instance = nullptr;


// 获取单例实例
CollectManager* CollectManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) CollectManager();
        CCASSERT(instance, "FATAL: Not enough memory for CollectManager");
    }
    return instance;
}


// 销毁单例实例
void CollectManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}


// 构造函数，初始化成员变量
CollectManager::CollectManager()
    : inventorySystem(nullptr)
    , currentTilemap(nullptr) {

    // 初始化采集物品数据库
    initializeItemDatabase();
}


// 析构函数，清理资源
CollectManager::~CollectManager() {
    // 清理所有采集点
    for (auto spot : collectSpots) {
        spot->removeFromParent();
    }
    collectSpots.clear();

    // 清理物品数据库
    itemDB.clear();
}


// 初始化采集物品数据库，注册所有可采集物品
void CollectManager::initializeItemDatabase() {
    // 注册所有可采集物品（功能5）


    // 木头
    CollectItem* wood = CollectItem::create("Wood", "items/wood.png",
        CollectType::WOOD, 10);
    wood->setRequiredTool("axe");
    wood->setRequiredClicks(3);
    itemDB.insert("wood", wood);


    // 石头
    CollectItem* stone = CollectItem::create("Stone", "collect/stone.png",
        CollectType::STONE, 15);
    stone->setRequiredTool("pickaxe");
    stone->setRequiredClicks(5);
    itemDB.insert("stone", stone);


    // 草
    CollectItem* grass = CollectItem::create("Grass", "items/grass.png",
        CollectType::GRASS, 5);
    grass->setRequiredTool("sickle");  // 镰刀，但斧子也可以（特殊处理在CollectSpot中）
    grass->setRequiredClicks(2);
    itemDB.insert("grass", grass);


    // 树（大树，产更多木材）
    CollectItem* tree = CollectItem::create("Hardwood", "items/hardwood.png",
        CollectType::TREE, 25);
    tree->setRequiredTool("axe");
    tree->setRequiredClicks(8);
    itemDB.insert("tree", tree);
}


// ==============================================
// 重要：连接背包系统的接口
// ==============================================
// 设置背包系统接口
void CollectManager::setInventorySystem(IInventorySystem* inventory) {
    this->inventorySystem = inventory;
    CCLOG("CollectManager: 背包系统接口已连接");
}


// 初始化采集管理器，根据瓦片地图生成采集点
void CollectManager::initialize(TMXTiledMap* tilemap) {
    if (!tilemap) {
        CCLOG("CollectManager: 瓦片地图为空，无法初始化");
        return;
    }

    currentTilemap = tilemap;

    // 从瓦片地图生成采集点
    spawnCollectFromTilemap(tilemap);

    CCLOG("CollectManager: 采集系统初始化完成，生成 %d 个采集点",
        (int)collectSpots.size());
}



// 从瓦片地图的指定层生成采集点
void CollectManager::spawnCollectFromTilemap(TMXTiledMap* tilemap) {
    // 获取瓦片地图中的采集层
    auto collectLayer = tilemap->getLayer("Collect");
    if (!collectLayer) {
        CCLOG("CollectManager: 警告 - 瓦片地图中没有'Collect'层");
        return;
    }

    Size mapSize = tilemap->getMapSize();
    Size tileSize = tilemap->getTileSize();

    CCLOG("CollectManager: 开始从瓦片地图生成采集点...");

    // 遍历所有瓦片
    for (int x = 0; x < mapSize.width; ++x) {
        for (int y = 0; y < mapSize.height; ++y) {
            int tileGID = collectLayer->getTileGIDAt(Vec2(x, y));
            if (tileGID > 0) {
                // 获取瓦片属性
                Value properties = tilemap->getPropertiesForGID(tileGID);
                if (!properties.isNull()) {
                    ValueMap propMap = properties.asValueMap();

                    // 检查是否有type属性（功能1：判断地图上是什么）
                    if (propMap.find("type") != propMap.end()) {
                        std::string tileType = propMap["type"].asString();

                        // 只处理可采集的瓦片类型
                        if (tileType == "tree" || tileType == "rock" ||
                            tileType == "grass" || tileType == "wood") {

                            // 计算世界坐标（瓦片中心点）
                            Vec2 worldPos = Vec2(
                                x * tileSize.width + tileSize.width / 2,
                                y * tileSize.height + tileSize.height / 2
                            );

                            // 获取对应的采集物品ID
                            std::string collectId = getCollectIdFromTileType(tileType);

                            // 创建采集点
                            CollectSpot* spot = CollectSpot::create(collectId, worldPos, tileType);
                            if (spot) {
                                collectSpots.push_back(spot);
                                tilemap->addChild(spot, 10);  // 添加到瓦片地图，较高层级

                                CCLOG("  创建采集点: %s 在位置(%d,%d)",
                                    tileType.c_str(), x, y);
                            }
                        }
                    }
                }
            }
        }
    }
}



// 核心交互处理：处理玩家点击采集点的交互
bool CollectManager::handleCollectInteraction(Vec2 touchPos, Vec2 playerPos) {
    // 1. 找到被点击的采集点
    CollectSpot* targetSpot = findCollectAtPosition(touchPos);
    if (!targetSpot) {
        return false;  // 没有点击到采集点
    }

    // 2. 检查采集点是否可用
    if (!targetSpot->isAvailable()) {
        CCLOG("CollectManager: 这个采集点已被采集");
        return false;
    }

    // 3. 检查玩家是否在范围内（功能2）
    if (!targetSpot->isPlayerInRange(playerPos)) {
        CCLOG("CollectManager: 距离太远，无法采集");
        // 可以在这里显示提示信息
        GameManager::getInstance()->showHint("需要靠近才能采集！");
        return false;
    }

    // 4. 检查背包系统是否已连接
    if (!inventorySystem) {
        CCLOG("CollectManager: 错误 - 背包系统接口未连接");
        return false;
    }

    // 5. 获取当前工具并检查是否匹配（功能4）
    std::string currentTool = inventorySystem->getCurrentTool();
    if (!targetSpot->canCollectWithTool(currentTool)) {
        CCLOG("CollectManager: 工具不匹配，需要: %s, 当前: %s",
            targetSpot->getItem()->getRequiredTool().c_str(),
            currentTool.c_str());

        // 根据工具类型显示不同的提示
        if (currentTool == "none") {
            GameManager::getInstance()->showHint("需要装备工具才能采集！");
        }
        else {
            GameManager::getInstance()->showHint("需要" +
                targetSpot->getItem()->getRequiredTool() + "才能采集！");
        }
        return false;
    }

    // 6. 检查背包空间
    if (!inventorySystem->hasSpaceFor(targetSpot->getItem())) {
        CCLOG("CollectManager: 背包已满");
        GameManager::getInstance()->showHint("背包已满！");
        return false;
    }

    // 7. 执行采集（点击一次）
    bool collectComplete = targetSpot->collect();

    // 8. 如果采集完成，处理后续逻辑
    if (collectComplete) {
        handleCollectSuccess(targetSpot);
    }
    else {
        // 显示还需要点击的次数
        int remaining = targetSpot->getRequiredClicks() - targetSpot->getCurrentClicks();
        CCLOG("CollectManager: 还需要点击 %d 次", remaining);
    }

    return true;  // 成功处理了采集交互
}



// 找到指定位置的采集点（带扩大点击范围检测）
CollectSpot* CollectManager::findCollectAtPosition(Vec2 position) const {
    for (auto spot : collectSpots) {
        if (!spot->isAvailable()) continue;

        // 获取采集点的包围盒
        Rect spotRect = spot->getBoundingBox();

        // 扩大点击范围，方便玩家点击
        spotRect.origin -= Vec2(10, 10);
        spotRect.size += Size(20, 20);

        if (spotRect.containsPoint(position)) {
            return spot;
        }
    }
    return nullptr;
}



// 处理采集成功的后续逻辑
void CollectManager::handleCollectSuccess(CollectSpot* spot) {
    if (!spot || !inventorySystem) return;

    // 获取采集到的物品
    CollectItem* collectedItem = spot->getItem();
    if (!collectedItem) return;

    CCLOG("CollectManager: 采集完成！获得: %s",
        collectedItem->getName().c_str());

    // 将物品添加到背包（通过接口调用）
    bool success = inventorySystem->addItem(collectedItem);

    if (success) {
        // 增加采集技能经验
        GameManager::getInstance()->addSkillExperience("Collecting", 15);

        // 显示获得物品提示
        GameManager::getInstance()->showItemGetPopup(
            collectedItem->getName(),
            collectedItem->getSpriteName()
        );

        // 从管理器中移除采集点
        auto it = std::find(collectSpots.begin(), collectSpots.end(), spot);
        if (it != collectSpots.end()) {
            collectSpots.erase(it);
        }

        // 注意：spot节点会在播放完消失动画后自动移除
    }
    else {
        CCLOG("CollectManager: 添加到背包失败");
    }
}



// 瓦片类型到采集物品ID的映射（功能1：根据瓦片类型判断是什么物品）
std::string CollectManager::getCollectIdFromTileType(const std::string& tileType) {
    // 功能1：根据瓦片类型判断是什么物品
    if (tileType == "tree") return "tree";      // 大树 -> 硬木
    if (tileType == "wood") return "wood";      // 灌木 -> 普通木头
    if (tileType == "rock") return "stone";     // 石头 -> 石头
    if (tileType == "grass") return "grass";    // 草 -> 草
    return "wood";  // 默认
}



// 根据物品ID获取采集物品（返回副本避免共享实例）
CollectItem* CollectManager::getCollectItemById(const std::string& itemId) {
    auto item = itemDB.at(itemId);
    if (item) {
        // 返回副本，避免多个采集点共享同一个物品实例
        return CollectItem::create(item->getName(), item->getSpriteName(),
            item->getCollectType(), item->getSellPrice());
    }
    return nullptr;
}



// 每帧更新，处理采集点高亮和清理
void CollectManager::update(float dt) {
    // 获取玩家位置
    auto player = GameManager::getInstance()->getPlayer();
    if (player) {
        // 更新采集点的高亮状态
        updateCollectHighlights(player->getPosition());
    }

    // 清理已移除的采集点
    auto it = collectSpots.begin();
    while (it != collectSpots.end()) {
        if (!(*it)->getParent()) {  // 如果节点已从父节点移除
            it = collectSpots.erase(it);
        }
        else {
            ++it;
        }
    }
}



// 更新采集点的高亮状态（根据玩家距离调整透明度）
void CollectManager::updateCollectHighlights(Vec2 playerPos) {
    // 更新所有采集点的可采集状态指示
    for (auto spot : collectSpots) {
        if (spot->isAvailable()) {
            bool inRange = spot->isPlayerInRange(playerPos);

            // 这里可以添加高亮效果，比如改变颜色或添加发光
            // 示例：spot->setHighlight(inRange);

            // 如果不在范围内，可以降低透明度提示
            if (!inRange && spot->getOpacity() == 255) {
                spot->setOpacity(180);
            }
            else if (inRange && spot->getOpacity() != 255) {
                spot->setOpacity(255);
            }
        }
    }
}
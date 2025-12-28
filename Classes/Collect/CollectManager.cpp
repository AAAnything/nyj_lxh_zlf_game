#include "CollectManager.h"
#include "cocos2d.h"

USING_NS_CC;

// 静态成员初始化
CollectManager* CollectManager::instance = nullptr;



CollectManager* CollectManager::getInstance() {
    if (!instance) {
        instance = new CollectManager();
    }
    return instance;
}



void CollectManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}



CollectManager::CollectManager()
    : inventorySystem(nullptr)
    , currentTilemap(nullptr) {
    initializeItemDatabase();
}



CollectManager::~CollectManager() {
    for (auto spot : collectSpots) {
        spot->removeFromParent();
    }
    collectSpots.clear();

    for (auto& pair : itemDB) {
        delete pair.second;
    }
    itemDB.clear();
}



void CollectManager::initializeItemDatabase() {
    // 木头
    CollectItem* wood = CollectItem::create("Wood", "items/wood.png", CollectType::WOOD, 10);
    wood->setRequiredTool("axe");
    wood->setRequiredClicks(2);
    itemDB["wood"] = wood;

    // 石头
    CollectItem* stone = CollectItem::create("Stone", "collect/stone.png", CollectType::STONE, 15);
    stone->setRequiredTool("pickaxe");
    stone->setRequiredClicks(2);
    itemDB["stone"] = stone;

    // 草
    CollectItem* grass = CollectItem::create("Grass", "items/grass.png", CollectType::GRASS, 5);
    grass->setRequiredTool("sickle");
    grass->setRequiredClicks(2);
    itemDB["grass"] = grass;

    // 树
    CollectItem* tree = CollectItem::create("Hardwood", "items/hardwood.png", CollectType::TREE, 25);
    tree->setRequiredTool("axe");
    tree->setRequiredClicks(5);
    itemDB["tree"] = tree;

    // 贝壳
    CollectItem* shell = CollectItem::create("Shell", "items/shell.png", CollectType::SHELL, 8);
    // 可以显式设置无需工具
    shell->setRequiredTool("");
    shell->setRequiredClicks(1);
    itemDB["shell"] = shell;
}


// 处理瓦片地图的采集点
void CollectManager::spawnCollectFromTilemap(TMXTiledMap* tilemap) {
    CCLOG("CollectManager: 开始从多个图层生成采集点...");

    // 处理tree图层(下移200像素)
    processCollectTreeByGID(tilemap, "plant", 178, 697, "tree", 200);
    // 处理stone图层
    processCollectSingleByGID(tilemap, "stone", 89, 89, "rock");
    // 处理shrub图层，以及stone上的一个小问题
    processCollectSingleByGID(tilemap, "shrub", 90, 90, "wood");
    processCollectSingleByGID(tilemap, "stone", 90, 90, "wood");
    // 处理grass图层
    processCollectSingleByGID(tilemap, "plant", 177, 177, "grass");

    CCLOG("CollectManager: 采集点生成完成，总数: %d", (int)collectSpots.size());
}


// tilemap：瓦片地图对象
// layerName：要处理的图层名称（如"tree"、"stone"）
// minGID / maxGID：瓦片ID的范围（如90 - 90）
// collectType：采集点类型标识（如"grass"）
void CollectManager::processCollectSingleByGID(TMXTiledMap* tilemap,
    const std::string& layerName,
    int minGID, int maxGID,
    const std::string& collectType)
{
    auto layer = tilemap->getLayer(layerName);
    if (!layer) {
        CCLOG("processCollectLayerByGID: 图层%s不存在", layerName.c_str());
        return;
    }

    Size mapSize = tilemap->getMapSize();       // 地图的瓦片数量（宽/高）
    Size tileSize = tilemap->getTileSize();     // 单个瓦片的像素尺寸
    float mapScale = tilemap->getScale();       // 地图缩放比例

    int spotCount = 0;

    for (int x = 0; x < mapSize.width; ++x) {
        for (int y = 0; y < mapSize.height; ++y) {
            int tileGID = layer->getTileGIDAt(Vec2(x, y));
            if (tileGID >= minGID && tileGID <= maxGID) {
                // 修正y轴坐标（反转Tiled的y轴）
                // 公式：地图总高度（像素） - 瓦片y坐标（像素） - 瓦片高度
                float tileY = (mapSize.height * tileSize.height) - (y * tileSize.height) - tileSize.height;

                // 修正
                tileY -= 30;

                // 瓦片中心坐标（基于tilemap的本地坐标，锚点(0,0)）
                Vec2 tileCenterPos = Vec2(
                    x * tileSize.width + tileSize.width / 2,  // x轴：瓦片左+半宽=中心
                    tileY + tileSize.height / 2               // y轴：修正后+半高=中心
                );

                // 核心修复2：去掉多余的世界/本地坐标转换，直接用tilemap的本地坐标
                // 若tilemap有锚点/位置偏移，直接叠加即可（可选，根据你的场景）
                Vec2 finalPos = tileCenterPos;


                CCLOG("创建采集点: 瓦片(%d,%d) -> 修正后位置(%.0f,%.0f)",
                    x, y, finalPos.x, finalPos.y);

                std::string collectId = getCollectIdFromTileType(collectType);
                CollectSpot* spot = CollectSpot::create(collectId, finalPos, collectType);

                if (spot) {
                    spot->setScale(mapScale);  // 保持和地图相同缩放
                    collectSpots.push_back(spot);
                    tilemap->addChild(spot, 10);  // 添加到tilemap节点下，继承其缩放/位置
                    spotCount++;
                }
            }
        }
    }

    CCLOG("图层%s处理完成，生成%d个采集点", layerName.c_str(), spotCount);
}



// 处理连续GID区间（整棵树）
void CollectManager::processCollectTreeByGID(TMXTiledMap* tilemap,
    const std::string& layerName,
    int treeMinGID, int treeMaxGID,
    const std::string& collectType,
    int downOffset) // 自定义下移偏移值
{
    auto layer = tilemap->getLayer(layerName);
    if (!layer) return;

    Size mapSize = tilemap->getMapSize();
    Size tileSize = tilemap->getTileSize();
    float mapScale = tilemap->getScale();
    std::set<std::string> processedTiles;
    int spotCount = 0;

    for (int x = 0; x < mapSize.width; ++x) {
        for (int y = 0; y < mapSize.height; ++y) {
            std::string tileKey = StringUtils::format("%d_%d", x, y);
            if (processedTiles.count(tileKey)) continue;

            int tileGID = layer->getTileGIDAt(Vec2(x, y));
            // 匹配树的连续GID区间
            if (tileGID >= treeMinGID && tileGID <= treeMaxGID) {
                // 1. 查找整棵树的完整矩形范围
                int treeLeft = x, treeRight = x;
                int treeTop = y, treeBottom = y;

                // 向右找连续的树ID
                while (treeRight + 1 < mapSize.width) {
                    int nextGID = layer->getTileGIDAt(Vec2(treeRight + 1, y));
                    if (nextGID >= treeMinGID && nextGID <= treeMaxGID) treeRight++;
                    else break;
                }

                // 向下找连续的树ID
                while (treeBottom + 1 < mapSize.height) {
                    bool isAllTree = true;
                    for (int col = treeLeft; col <= treeRight; col++) {
                        int nextGID = layer->getTileGIDAt(Vec2(col, treeBottom + 1));
                        if (!(nextGID >= treeMinGID && nextGID <= treeMaxGID)) {
                            isAllTree = false;
                            break;
                        }
                    }
                    if (isAllTree) treeBottom++;
                    else break;
                }

                // 2. 计算树的中心坐标 + 下移微调
                int centerTileX = (treeLeft + treeRight) / 2;
                int centerTileY = (treeTop + treeBottom) / 2;
                float tileY = (mapSize.height * tileSize.height) - (centerTileY * tileSize.height) - tileSize.height;
                Vec2 treeCenterPos = Vec2(
                    centerTileX * tileSize.width + tileSize.width / 2 + 160,
                    tileY + tileSize.height / 2 - downOffset // 自定义下移
                );

                // 3. 标记整棵树的瓦片为已处理
                for (int col = treeLeft; col <= treeRight; col++) {
                    for (int row = treeTop; row <= treeBottom; row++) {
                        processedTiles.insert(StringUtils::format("%d_%d", col, row));
                    }
                }

                // 4. 生成树的采集点（扩大交互范围）
                std::string collectId = getCollectIdFromTileType(collectType);
                CollectSpot* spot = CollectSpot::create(collectId, treeCenterPos, collectType);
                if (spot) {
                    // 交互范围匹配树的尺寸
                    float treeWidth = (treeRight - treeLeft + 1) * tileSize.width;
                    float treeHeight = (treeBottom - treeTop + 1) * tileSize.height;
                    spot->setContentSize(Size(treeWidth, treeHeight));
                    spot->setAnchorPoint(Vec2(0.5, 0)); // 底部对齐
                    spot->setScale(mapScale);

                    collectSpots.push_back(spot);
                    tilemap->addChild(spot, 10);
                    spotCount++;
                }
            }
        }
    }
    CCLOG("图层%s（整棵树-%s）生成%d个采集点", layerName.c_str(), collectType.c_str(), spotCount);
}



void CollectManager::initialize(TMXTiledMap* tilemap) {
    if (!tilemap) return;

    currentTilemap = tilemap;
    spawnCollectFromTilemap(tilemap);
    CCLOG("CollectManager: 初始化完成，生成 %d 个采集点", (int)collectSpots.size());
}



void CollectManager::setInventorySystem(IInventorySystem* inventory) {
    this->inventorySystem = inventory;
    CCLOG("CollectManager: 背包系统接口已连接");
}


// 新增的触摸事件处理方法
void CollectManager::handleTouchEvent(Vec2 touchPos) {
    CCLOG("CollectManager: 处理触摸事件 (%.0f, %.0f)", touchPos.x, touchPos.y);

    for (auto spot : collectSpots) {
        if (spot && !spot->isCollected()) {
            // 检查点击是否在采集点范围内
            Vec2 spotPos = spot->getPosition();
            float distance = touchPos.distance(spotPos);

            CCLOG("检查采集点: %s 位置(%.0f,%.0f) 距离: %.0f 范围: %.0f",
                spot->getTileType().c_str(), spotPos.x, spotPos.y, distance, spot->getCollectRange());

            if (distance <= spot->getCollectRange()) {
                // 检查工具
                std::string currentTool = "";
                if (inventorySystem) { // 先判断背包系统是否已连接（避免空指针）
                    currentTool = inventorySystem->getCurrentTool(); // 正确调用方式
                }
                if (spot->canCollectWithTool(currentTool)) {
                    bool completed = spot->collect();
                    if (completed && inventorySystem) {
                        CollectItem* collectedItem = spot->getCollectItem();
                        if (collectedItem) {
                            inventorySystem->addItem(collectedItem);
                            CCLOG("采集完成: %s 已添加到背包", collectedItem->getName().c_str());
                        }
                    }
                }
                else {
                    CCLOG("工具不匹配，无法采集 %s", spot->getTileType().c_str());
                }
                break; // 只处理一个采集点
            }
        }
    }
}



CollectItem* CollectManager::getCollectItemById(const std::string& itemId) {
    auto it = itemDB.find(itemId);
    if (it != itemDB.end() && it->second) {
        // 返回副本
        return CollectItem::create(it->second->getName(),
            it->second->getSpriteName(),
            it->second->getCollectType(),
            it->second->getSellPrice());
    }
    return nullptr;
}



std::string CollectManager::getCollectIdFromTileType(const std::string& tileType) {
    if (tileType == "tree") return "tree";
    if (tileType == "wood") return "wood";
    if (tileType == "rock") return "stone";
    if (tileType == "grass") return "grass";
    if (tileType == "shell") return "shell";
    return "wood";
}



void CollectManager::update(float dt) {
    // 简单的更新逻辑
    auto it = collectSpots.begin();
    while (it != collectSpots.end()) {
        if (!(*it)->getParent()) {
            it = collectSpots.erase(it);
        }
        else {
            ++it;
        }
    }
}


void CollectManager::handleCollectSuccess(CollectSpot* spot) {
    // 这里可以写采集成功后的逻辑：比如添加物品到背包、播放音效、刷新UI等

}
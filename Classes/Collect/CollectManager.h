#ifndef __COLLECT_MANAGER_H__
#define __COLLECT_MANAGER_H__

#include "cocos2d.h"
#include <vector>
#include <unordered_map>
#include "CollectSpot.h"
#include "CollectItem.h"

USING_NS_CC;

// 背包系统接口
class IInventorySystem {
public:
    virtual bool addItem(Item* item) = 0;
    virtual std::string getCurrentTool() = 0;
    virtual bool hasSpaceFor(Item* item) = 0;
    virtual ~IInventorySystem() {}
};

// 采集管理器
class CollectManager {
public:
    static CollectManager* getInstance();
    static void destroyInstance();

    // 初始化
    void initialize(TMXTiledMap* tilemap);

    // 背包系统连接
    void setInventorySystem(IInventorySystem* inventory);

    // 交互处理
    bool handleCollectInteraction(Vec2 touchPos, Vec2 playerPos);

    // 获取物品
    CollectItem* getCollectItemById(const std::string& itemId);

    // 更新
    void update(float dt);

private:
    CollectManager();
    ~CollectManager();

    // 私有方法
    void initializeItemDatabase();
    void spawnCollectFromTilemap(TMXTiledMap* tilemap);
    void processCollectSingleByGID(TMXTiledMap* tilemap, const std::string& layerName,
        int minGID, int maxGID, const std::string& collectType);
    void processCollectTreeByGID(cocos2d::TMXTiledMap* tilemap,
        const std::string& layerName,
        int treeMinGID, int treeMaxGID,
        const std::string& collectType,
        int downOffset);
    CollectSpot* findCollectAtPosition(Vec2 position) const;
    void handleCollectSuccess(CollectSpot* spot);
    void updateCollectHighlights(Vec2 playerPos);


   

    // 辅助方法
    std::string getCollectIdFromTileType(const std::string& tileType);

    // 单例实例
    static CollectManager* instance;

    // 成员变量
    std::vector<CollectSpot*> collectSpots;
    std::unordered_map<std::string, CollectItem*> itemDB;
    IInventorySystem* inventorySystem;
    TMXTiledMap* currentTilemap;
};

#endif // __COLLECT_MANAGER_H__
#include "cocos2d.h"
#include "CollectSpot.h"
#include "CollectItem.h"
#include "Core/Item.h"
#include <vector>

// ==============================================
// 接口定义：背包系统需要实现的接口
// ==============================================
// 这个接口是你的采集系统与背包系统之间的"合同"
// 你只需要知道背包系统能提供这些功能，不需要知道具体实现
class IInventorySystem {
public:
    // 合同方法1：添加物品到背包
    virtual bool addItem(Item* item) = 0;

    // 合同方法2：获取当前装备的工具类型
    // 返回工具类型字符串："axe"/"pickaxe"/"sickle"/"none"
    virtual std::string getCurrentTool() = 0;

    // 合同方法3：检查背包是否有空间
    virtual bool hasSpaceFor(Item* item) = 0;

    virtual ~IInventorySystem() {}  // 虚析构函数
};


// ==============================================
// 采集管理器 - 管理所有采集点
// ==============================================
class CollectManager {
public:
    // 单例模式获取实例
    static CollectManager* getInstance();

    // 销毁实例
    static void destroyInstance();

    // 初始化采集系统
    // @param tilemap: 瓦片地图，用于生成采集点
    void initialize(cocos2d::TMXTiledMap* tilemap);

    // 设置背包系统接口
    // 将背包系统的实现连接到采集系统
    void setInventorySystem(IInventorySystem* inventory);

    // 处理采集交互（核心方法）
    // @param touchPos: 触摸位置（世界坐标）
    // @param playerPos: 玩家位置（世界坐标）
    // @return: 是否处理了采集交互
    bool handleCollectInteraction(cocos2d::Vec2 touchPos, cocos2d::Vec2 playerPos);

    // 根据瓦片类型获取采集物品ID
    // @param tileType: 瓦片类型
    // @return: 对应的采集物品ID
    std::string getCollectIdFromTileType(const std::string& tileType);

    // 获取采集物品（用于CollectSpot）
    CollectItem* getCollectItemById(const std::string& itemId);

    // 更新采集点状态（每帧调用）
    void update(float dt);

private:
    // 私有构造函数（单例模式）
    CollectManager();
    ~CollectManager();

    // 从瓦片地图生成采集点
    void spawnCollectFromTilemap(cocos2d::TMXTiledMap* tilemap);

    // 找到被点击的采集点
    CollectSpot* findCollectAtPosition(cocos2d::Vec2 position) const;

    // 处理采集成功（物品添加到背包）
    void handleCollectSuccess(CollectSpot* spot);

    // 检查玩家周围采集点并更新高亮状态
    void updateCollectHighlights(cocos2d::Vec2 playerPos);

    // 静态单例实例
    static CollectManager* instance;

    // 成员变量
    std::vector<CollectSpot*> collectSpots;             // 所有采集点
    cocos2d::Map<std::string, CollectItem*> itemDB;    // 采集物品数据库
    IInventorySystem* inventorySystem;                // 背包系统接口
    cocos2d::TMXTiledMap* currentTilemap;             // 当前瓦片地图
};
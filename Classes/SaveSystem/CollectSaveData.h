#include <string>
#include <ctime>

/**
 * 采集点数据 - 结构体
 * 存储单个采集点的状态信息
 */
struct CollectSpotData {
    // 基本信息
    std::string key;          // 唯一标识，如"rock_12_8"
    std::string tileType;     // 瓦片类型：rock, wood, grass, tree
    int tileX;                // 瓦片X坐标
    int tileY;                // 瓦片Y坐标

    // 采集状态
    bool isCollected;         // 是否已采集
    time_t collectedTime;     // 采集时间（Unix时间戳）
    int respawnDuration;      // 重生所需时间（秒）

    // 采集信息
    std::string itemId;       // 采集到的物品ID
    int collectCount;         // 采集次数（如果可重复采集）

    // 默认构造函数
    CollectSpotData();

    // 带参数的构造函数
    CollectSpotData(const std::string& key, const std::string& tileType,
        int x, int y, const std::string& itemId = "");

    // ==================== 序列化方法 ====================

    // 将结构体序列化为字符串
    std::string serialize() const;

    // 从字符串反序列化为结构体
    bool deserialize(const std::string& data);

    // ==================== 工具方法 ====================

    // 检查采集点是否应该重生
    bool shouldRespawn(time_t currentTime = 0) const;

    // 获取剩余重生时间（秒）
    int getRemainingRespawnTime(time_t currentTime = 0) const;

    // 获取从采集到现在经过的时间（秒）
    int getElapsedTime(time_t currentTime = 0) const;

    // 重置为未采集状态（用于重生）
    void resetToUncollected();

    // ==================== 静态方法 ====================

    // 根据类型和坐标生成唯一key
    static std::string generateKey(const std::string& tileType, int x, int y);

    // 从key解析出类型和坐标
    static bool parseKey(const std::string& key, std::string& tileType, int& x, int& y);

    // 根据类型获取默认的重生时间
    static int getDefaultRespawnTime(const std::string& tileType);
};
#include <string>
#include <vector>
#include <unordered_map>

/**
 * 游戏存储管理器 - 单例类
 * 负责所有游戏数据的保存和加载
 */
class SaveManager {
public:
    // 获取单例实例
    static SaveManager* getInstance();

    // 销毁单例（游戏退出时调用）
    static void destroyInstance();

    // ==================== 基础存储功能 ====================

    // 保存字符串类型的数据
    void saveString(const std::string& key, const std::string& value);

    // 加载字符串类型的数据
    std::string loadString(const std::string& key, const std::string& defaultValue = "");

    // 保存整数类型的数据
    void saveInt(const std::string& key, int value);

    // 加载整数类型的数据
    int loadInt(const std::string& key, int defaultValue = 0);

    // 保存布尔类型的数据
    void saveBool(const std::string& key, bool value);

    // 加载布尔类型的数据
    bool loadBool(const std::string& key, bool defaultValue = false);

    // ==================== 游戏特定功能 ====================

    // 检查游戏是否有存档数据
    bool hasSaveData();

    // 标记游戏已经初始化完成
    void markGameInitialized();

    // 清除所有存档数据（仅用于测试）
    void clearAllSaveData();

    // ==================== 采集系统专用 ====================

    // 保存单个采集点的数据
    void saveCollectSpot(const std::string& spotKey, const std::string& spotData);

    // 加载所有采集点的数据
    std::unordered_map<std::string, std::string> loadAllCollectSpots();

    // 删除指定采集点的数据
    void removeCollectSpot(const std::string& spotKey);

private:
    // 私有构造函数（单例模式）
    SaveManager();

    // 析构函数
    ~SaveManager();

    // 单例实例指针
    static SaveManager* instance;

    // 采集点数据的前缀（用于区分不同类型的数据）
    const std::string COLLECT_SPOT_PREFIX = "collect_spot_";

    // 生成采集点的完整存储key
    std::string getCollectSpotKey(const std::string& spotKey) const;
};

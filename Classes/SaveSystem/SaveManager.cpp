#include "SaveManager.h"
#include "cocos2d.h"
#include <sstream>

USING_NS_CC;

// 静态成员初始化
SaveManager* SaveManager::instance = nullptr;

// 获取单例实例
SaveManager* SaveManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) SaveManager();
        CCASSERT(instance, "FATAL: Not enough memory for SaveManager");
    }
    return instance;
}

// 销毁单例实例
void SaveManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
        CCLOG("SaveManager destroyed");
    }
}

// 构造函数
SaveManager::SaveManager() {
    CCLOG("SaveManager initialized");
}

// 析构函数
SaveManager::~SaveManager() {
    CCLOG("SaveManager destroyed");
}

// ==================== 基础存储功能实现 ====================

// 保存字符串到UserDefault
void SaveManager::saveString(const std::string& key, const std::string& value) {
    UserDefault::getInstance()->setStringForKey(key.c_str(), value);
}

// 从UserDefault加载字符串
std::string SaveManager::loadString(const std::string& key, const std::string& defaultValue) {
    return UserDefault::getInstance()->getStringForKey(key.c_str(), defaultValue);
}

// 保存整数到UserDefault
void SaveManager::saveInt(const std::string& key, int value) {
    UserDefault::getInstance()->setIntegerForKey(key.c_str(), value);
}

// 从UserDefault加载整数
int SaveManager::loadInt(const std::string& key, int defaultValue) {
    return UserDefault::getInstance()->getIntegerForKey(key.c_str(), defaultValue);
}

// 保存布尔值到UserDefault
void SaveManager::saveBool(const std::string& key, bool value) {
    UserDefault::getInstance()->setBoolForKey(key.c_str(), value);
}

// 从UserDefault加载布尔值
bool SaveManager::loadBool(const std::string& key, bool defaultValue) {
    return UserDefault::getInstance()->getBoolForKey(key.c_str(), defaultValue);
}

// ==================== 游戏特定功能实现 ====================

// 检查是否有游戏存档
bool SaveManager::hasSaveData() {
    // 检查是否有游戏初始化标记
    return loadBool("game_initialized", false);
}

// 标记游戏已经初始化
void SaveManager::markGameInitialized() {
    saveBool("game_initialized", true);
    UserDefault::getInstance()->flush();  // 立即写入文件
    CCLOG("Game marked as initialized");
}

// 清除所有存档数据
void SaveManager::clearAllSaveData() {
    UserDefault::getInstance()->destroyInstance();
    CCLOG("All save data cleared");

    // 重新获取UserDefault实例
    UserDefault::getInstance();
}

// ==================== 采集系统专用实现 ====================

// 生成采集点的完整存储key
std::string SaveManager::getCollectSpotKey(const std::string& spotKey) const {
    return COLLECT_SPOT_PREFIX + spotKey;
}

// 保存采集点数据
void SaveManager::saveCollectSpot(const std::string& spotKey, const std::string& spotData) {
    std::string fullKey = getCollectSpotKey(spotKey);
    saveString(fullKey, spotData);

    CCLOG("Saved collect spot: %s", spotKey.c_str());
}

// 加载所有采集点数据（目前未完全实现）
std::unordered_map<std::string, std::string> SaveManager::loadAllCollectSpots() {
    std::unordered_map<std::string, std::string> result;

    // 注意：UserDefault没有直接获取所有key的方法
    // 所以我们需要采用其他方式，这里先留空
    // 实际项目中可能需要维护一个列表

    CCLOG("Warning: loadAllCollectSpots not fully implemented yet");
    return result;
}

// 删除采集点数据
void SaveManager::removeCollectSpot(const std::string& spotKey) {
    std::string fullKey = getCollectSpotKey(spotKey);
    UserDefault::getInstance()->setStringForKey(fullKey.c_str(), "");  // 清空值

    CCLOG("Removed collect spot: %s", spotKey.c_str());
}
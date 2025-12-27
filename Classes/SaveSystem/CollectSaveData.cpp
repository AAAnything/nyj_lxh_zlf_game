#include "CollectSaveData.h"
#include <sstream>
#include <vector>
#include "cocos2d.h"

USING_NS_CC;

// 分隔符
const char DELIMITER = '|';

// ==================== 构造函数 ====================

// 默认构造函数
CollectSpotData::CollectSpotData()
    : tileX(0), tileY(0), isCollected(false), collectedTime(0),
    respawnDuration(0), collectCount(0) {
}

// 带参数的构造函数
CollectSpotData::CollectSpotData(const std::string& key, const std::string& tileType,
    int x, int y, const std::string& itemId)
    : key(key), tileType(tileType), tileX(x), tileY(y),
    isCollected(false), collectedTime(0),
    respawnDuration(getDefaultRespawnTime(tileType)),
    itemId(itemId), collectCount(0) {
}

// ==================== 序列化方法 ====================

// 将结构体序列化为字符串
std::string CollectSpotData::serialize() const {
    std::stringstream ss;

    // 使用|作为分隔符
    ss << key << DELIMITER
        << tileType << DELIMITER
        << tileX << DELIMITER
        << tileY << DELIMITER
        << (isCollected ? "1" : "0") << DELIMITER
        << collectedTime << DELIMITER
        << respawnDuration << DELIMITER
        << itemId << DELIMITER
        << collectCount;

    return ss.str();
}

// 从字符串反序列化为结构体
bool CollectSpotData::deserialize(const std::string& data) {
    if (data.empty()) {
        return false;
    }

    std::stringstream ss(data);
    std::string item;
    std::vector<std::string> tokens;

    // 分割字符串
    while (std::getline(ss, item, DELIMITER)) {
        tokens.push_back(item);
    }

    // 检查token数量
    if (tokens.size() < 9) {
        CCLOG("CollectSpotData: 数据格式错误，token数量不足: %d", (int)tokens.size());
        return false;
    }

    try {
        key = tokens[0];
        tileType = tokens[1];
        tileX = std::stoi(tokens[2]);
        tileY = std::stoi(tokens[3]);
        isCollected = (tokens[4] == "1");
        collectedTime = std::stoll(tokens[5]);
        respawnDuration = std::stoi(tokens[6]);
        itemId = tokens[7];
        collectCount = std::stoi(tokens[8]);

        return true;
    }
    catch (const std::exception& e) {
        CCLOG("CollectSpotData: 反序列化失败: %s", e.what());
        return false;
    }
}

// ==================== 工具方法 ====================

// 检查采集点是否应该重生
bool CollectSpotData::shouldRespawn(time_t currentTime) const {
    if (!isCollected) {
        return false;  // 未采集，不需要重生
    }

    if (currentTime == 0) {
        currentTime = time(nullptr);
    }

    int elapsed = static_cast<int>(difftime(currentTime, collectedTime));
    return elapsed >= respawnDuration;
}

// 获取剩余重生时间（秒）
int CollectSpotData::getRemainingRespawnTime(time_t currentTime) const {
    if (!isCollected || collectedTime == 0) {
        return 0;
    }

    if (currentTime == 0) {
        currentTime = time(nullptr);
    }

    int elapsed = static_cast<int>(difftime(currentTime, collectedTime));
    int remaining = respawnDuration - elapsed;
    return (remaining > 0) ? remaining : 0;
}

// 获取从采集到现在经过的时间（秒）
int CollectSpotData::getElapsedTime(time_t currentTime) const {
    if (!isCollected || collectedTime == 0) {
        return 0;
    }

    if (currentTime == 0) {
        currentTime = time(nullptr);
    }

    return static_cast<int>(difftime(currentTime, collectedTime));
}

// 重置为未采集状态（用于重生）
void CollectSpotData::resetToUncollected() {
    isCollected = false;
    collectedTime = 0;
    collectCount++;
}

// ==================== 静态方法 ====================

// 根据类型和坐标生成唯一key
std::string CollectSpotData::generateKey(const std::string& tileType, int x, int y) {
    return tileType + "_" + std::to_string(x) + "_" + std::to_string(y);
}

// 从key解析出类型和坐标
bool CollectSpotData::parseKey(const std::string& key, std::string& tileType, int& x, int& y) {
    size_t firstUnderscore = key.find('_');
    if (firstUnderscore == std::string::npos) {
        return false;
    }

    size_t secondUnderscore = key.find('_', firstUnderscore + 1);
    if (secondUnderscore == std::string::npos) {
        return false;
    }

    try {
        tileType = key.substr(0, firstUnderscore);
        x = std::stoi(key.substr(firstUnderscore + 1, secondUnderscore - firstUnderscore - 1));
        y = std::stoi(key.substr(secondUnderscore + 1));
        return true;
    }
    catch (...) {
        return false;
    }
}

// 根据类型获取默认的重生时间
int CollectSpotData::getDefaultRespawnTime(const std::string& tileType) {
    // 根据类型设置不同的默认重生时间（秒）
    if (tileType == "grass") {
        return 1800;  // 30分钟
    }
    else if (tileType == "rock") {
        return 3600;  // 1小时
    }
    else if (tileType == "wood") {
        return 7200;  // 2小时
    }
    else if (tileType == "tree") {
        return 86400; // 24小时
    }

    return 3600;  // 默认1小时
}
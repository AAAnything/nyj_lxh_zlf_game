// 文件名：FarmlandTile.h
// 作用： 地块类，管理地块状态、作物生长和交互
// 作者： Niu
#pragma once

#include "cocos2d.h"
#include "../Core/GameObject.h"
#include "../Crop/Crop.h"

USING_NS_CC;

// 地块状态枚举
enum class TileState {
    EMPTY = 0,      // 空地
    TILLED = 1,     // 已耕地
    PLANTED = 2,    // 已播种
    WATERED = 3,    // 已浇水
    MATURE = 4,     // 成熟
    WITHERED = 5    // 枯萎
};

// 季节枚举
enum class Season {
    SPRING = 0,
    SUMMER = 1,
    AUTUMN = 2,
    WINTER = 3
};

class FarmlandTile : public GameObject {
private:
    // 基本状态
    TileState state;
    Crop* plantedCrop;           // 当前种植的作物
    int daysSincePlanted;        // 种植后的天数
    bool hasWaterToday;          // 今天是否浇过水
    bool hasFertilizer;          // 是否有施肥

    // 地块位置和大小
    Vec2 position;
    Size tileSize;

    // 生长相关
    int consecutiveWaterDays;    // 连续浇水天数
    int cropStress;              // 作物压力（缺水/缺肥）
    int fertilizerBonusDays;     // 施肥带来的额外生长天数

    // Sprite
    Sprite* tileSprite;
    Sprite* cropSprite;

public:
    // 构造函数和析构函数
    FarmlandTile();
    FarmlandTile(const Vec2& pos, const Size& size);
    virtual ~FarmlandTile();

    // 初始化
    virtual bool init() override;

    // 地块操作
    bool till();                    // 耕地
    bool plant(Crop* crop);         // 播种
    bool water();                   // 浇水
    bool fertilize();               // 施肥
    Item* harvest();                // 收获
    void clear();                   // 清除地块

    // 时间更新
    void onDayPassed();             // 每天调用
    void resetDailyState();         // 重置每日状态

    // 状态检查
    bool canTill() const;
    bool canPlant() const;
    bool canWater() const;
    bool canFertilize() const;
    bool canHarvest() const;
    bool isWithered() const;

    // 作物检查
    Crop* getPlantedCrop() const { return plantedCrop; }
    bool hasCrop() const { return plantedCrop != nullptr; }

    // 状态获取
    TileState getState() const { return state; }
    int getDaysSincePlanted() const { return daysSincePlanted; }
    bool getHasWaterToday() const { return hasWaterToday; }
    bool getHasFertilizer() const { return hasFertilizer; }

    // 设置位置和精灵
    void setPosition(const Vec2& pos);
    Vec2 getPosition() const { return position; }

    // 更新显示
    void updateAppearance();

    // 地块信息
    std::string getStatusString() const;

    // 序列化/反序列化（用于保存游戏）
    virtual cocos2d::ValueMap toValueMap() const;
    virtual bool fromValueMap(const cocos2d::ValueMap& map);

private:
    // 私有方法
    void updateTileSprite();        // 更新地块精灵
    void updateCropSprite();        // 更新作物精灵
    void createDefaultSprite();     // 创建默认精灵
    void checkWithering();          // 检查枯萎
    void applyFertilizerEffects();  // 应用肥料效果
    void applyWaterEffects();       // 应用浇水效果

    // 辅助方法
    std::string getTileImageName() const;
    std::string getCropImageName() const;
    Color4F getTileColor() const;
};
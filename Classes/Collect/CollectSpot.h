#pragma once
#include "cocos2d.h"
#include "CollectItem.h"

// 采集点类 - 代表场景中可采集的物体
class CollectSpot : public cocos2d::Node {
public:
    // 创建采集点
    // @param itemId: 采集物品ID
    // @param position: 世界坐标位置
    // @param tileType: 瓦片类型（tree/rock/grass）
    static CollectSpot* create(const std::string& itemId, cocos2d::Vec2 position,
        const std::string& tileType = "");

    virtual bool init(const std::string& itemId, const std::string& tileType);

    // ===== 核心功能方法 =====

    // 判断玩家是否在采集范围内（功能2：需要靠近才能采集）
    bool isPlayerInRange(cocos2d::Vec2 playerPos) const;

    // 检查当前工具是否能采集此物品（功能4：需要相应工具）
    // @param currentTool: 玩家当前装备的工具类型
    bool canCollectWithTool(const std::string& currentTool) const;

    // 执行采集操作（点击一次）
    // @return: 是否完成采集（达到点击次数）
    bool collect();

    // 播放采集时的抖动效果（功能3：不需要进度条，用抖动效果）
    void playShakeEffect();

    // 播放采集完成效果（物品消失）
    void playCollectCompleteEffect();

    // ===== 状态查询方法 =====

    // 是否已完成采集
    bool isCollectComplete() const { return currentClicks >= requiredClicks; }

    // 是否可用（未被采集）
    bool isAvailable() const { return !collected; }

    // 获取当前点击次数
    int getCurrentClicks() const { return currentClicks; }

    // 获取需要点击次数
    int getRequiredClicks() const { return requiredClicks; }

    // 获取瓦片类型（功能1：判断地图上是什么）
    std::string getTileType() const { return tileType; }

    // 获取对应的采集物品
    CollectItem* getItem() const { return item; }

private:
    CollectItem* item;           // 对应的采集物品
    bool collected;             // 是否已采集完成
    int currentClicks;          // 当前已点击次数
    int requiredClicks;         // 需要点击的总次数
    float collectRange;         // 采集范围（像素）
    std::string tileType;       // 瓦片类型：tree/rock/grass
    cocos2d::Sprite* sprite;    // 显示的精灵
};
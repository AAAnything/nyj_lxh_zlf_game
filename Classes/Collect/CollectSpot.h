#ifndef __COLLECT_SPOT_H__
#define __COLLECT_SPOT_H__

#include "cocos2d.h"
#include "CollectItem.h"

USING_NS_CC;

class CollectSpot : public Node {
public:
    static CollectSpot* create(const std::string& itemId, Vec2 position,
        const std::string& tileType = "");

    virtual bool init(const std::string& itemId, const std::string& tileType);

    // 核心方法
    bool isPlayerInRange(Vec2 playerPos) const;
    bool canCollectWithTool(const std::string& currentTool) const;
    bool collect();
    void playShakeEffect();
    void playCollectCompleteEffect();

    // 状态查询
    bool isCollectComplete() const { return currentClicks >= requiredClicks; }
    bool isAvailable() const { return !collected; }
    int getCurrentClicks() const { return currentClicks; }
    int getRequiredClicks() const { return requiredClicks; }
    std::string getTileType() const { return tileType; }
    CollectItem* getItem() const { return item; }

private:
    CollectItem* item;
    bool collected;
    int currentClicks;
    int requiredClicks;
    float collectRange;
    std::string tileType;
    Sprite* sprite;
};

#endif // __COLLECT_SPOT_H__
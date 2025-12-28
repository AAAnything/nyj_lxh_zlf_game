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


    // 鼠标事件处理函数
    void onMouseDown(cocos2d::Event* event);


    // 添加触摸相关方法
    void enableTouch(bool enable = true);
    void disableTouch() { enableTouch(false); }
    void removeTouchListener();


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

    // 新增的接口方法（供 CollectManager 使用）
    bool isCollected() const { return collected; }
    float getCollectRange() const { return collectRange; }
    std::string getRequiredTool() const {
        return item ? item->getRequiredTool() : "";
    }
    CollectItem* getCollectItem() const { return item; }


private:
    CollectItem* item;
    bool collected;
    int currentClicks;
    int requiredClicks;
    float collectRange;
    std::string tileType;
    Sprite* sprite;

    // 新增：触摸事件监听器成员变量
    EventListenerMouse* touchListener;
};

#endif // __COLLECT_SPOT_H__
#ifndef __COLLECT_ITEM_H__
#define __COLLECT_ITEM_H__

#include "Core/Item.h"
#include "cocos2d.h" 
#include <string>

USING_NS_CC;

// 采集物品类型枚举
enum class CollectType {
    WOOD,       // 木头
    STONE,      // 石头  
    GRASS,      // 草
    TREE        // 树（大树，产更多木材）
};

// 采集物品类
class CollectItem : public Item, public Ref {
public:
    // 创建采集物品的工厂方法
    static CollectItem* create(const std::string& name, const std::string& sprite,
        CollectType type, int baseValue);

    // 初始化方法（私有，只能通过create调用）
    bool init(const std::string& name, const std::string& sprite);

    // Item接口实现
    std::string getName() const override { return name; }
    std::string getSpriteName() const override { return spriteName; }
    int getSellPrice() const override { return sellPrice; }
    void setSellPrice(int price) override { sellPrice = price; }
    std::string getItemType() const override { return "Collectible"; }

    // 采集物品特有方法
    CollectType getCollectType() const { return collectType; }
    std::string getRequiredTool() const { return requiredTool; }
    int getRequiredClicks() const { return requiredClicks; }
    void setRequiredTool(const std::string& tool) { requiredTool = tool; }
    void setRequiredClicks(int clicks) { requiredClicks = clicks; }

private:
    CollectItem() : collectType(CollectType::WOOD), requiredClicks(0), sellPrice(0) {}

    // 采集类型
    CollectType collectType;

    // 工具需求
    std::string requiredTool;
    int requiredClicks;

    // Item基础属性
    std::string name;
    std::string spriteName;
    int sellPrice;
};

#endif // __COLLECT_ITEM_H__
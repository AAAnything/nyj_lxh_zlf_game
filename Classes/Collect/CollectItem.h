#ifndef __COLLECT_ITEM_H__
#define __COLLECT_ITEM_H__

#include "Core/GameObject.h"
#include "Item/Item.h"
#include "cocos2d.h" 
#include <string>

USING_NS_CC;

// 采集物品类型枚举
enum class CollectType {
    WOOD,       // 木头
    STONE,      // 石头  
    GRASS,      // 草
    TREE,       // 树
    SHELL       // 贝壳
};

// 采集物品类
// 继承自Item，可以直接放入背包系统
class CollectItem : public Item{
public:
    // 创建采集物品的工厂方法
    // @param name: 物品名称
    // @param sprite: 图标路径
    // @param type: 采集类型
    // @param baseValue: 基础价值
    static CollectItem* create(const std::string& name, const std::string& sprite,
        CollectType type, int baseValue);

    // 初始化方法（私有，只能通过create调用）
    bool init(const std::string& name, const std::string& sprite);

    // Item接口实现
    std::string getName() const{ return name; }
    std::string getSpriteName() const{ return spriteName; }
    int getSellPrice() const{ return sellPrice; }
    void setSellPrice(int price){ sellPrice = price; }
    std::string getItemType() const{ return "Collectible"; }

    // 采集物品特有方法
    // 获取采集类型
    CollectType getCollectType() const { return collectType; }

    // 获取所需工具类型
    std::string getRequiredTool() const { return requiredTool; }

    // 获取需要点击次数
    int getRequiredClicks() const { return requiredClicks; }

    // 设置所需工具类型
    void setRequiredTool(const std::string& tool) { requiredTool = tool; }

    // 设置需要点击次数
    void setRequiredClicks(int clicks) { requiredClicks = clicks; }

private:
    //CollectItem() : collectType(CollectType::WOOD), requiredClicks(0), sellPrice(0) {}

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
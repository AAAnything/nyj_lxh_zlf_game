#ifndef __ITEM_H__
#define __ITEM_H__

#include <string>

/**
 * 物品基类
 * 所有可放入背包的物品都继承此类
 */
class Item {
public:
    virtual ~Item() {}

    // 获取物品名称
    virtual std::string getName() const = 0;

    // 获取物品图标路径
    virtual std::string getSpriteName() const = 0;

    // 获取物品价值（出售价格）
    virtual int getSellPrice() const = 0;

    // 设置物品价值
    virtual void setSellPrice(int price) = 0;

    // 获取物品类型（用于分类）
    virtual std::string getItemType() const = 0;

    // 获取物品描述
    virtual std::string getDescription() const { return ""; }

    // 物品是否可堆叠
    virtual bool isStackable() const { return true; }

    // 最大堆叠数量
    virtual int getMaxStack() const { return 99; }

    // 物品质量/等级（可选）
    virtual int getQuality() const { return 1; }
};

#endif // __ITEM_H__

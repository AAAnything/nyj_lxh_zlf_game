// Item.h
#pragma once

#include "cocos2d.h"

USING_NS_CC;

// 种类枚举
enum class ItemType
{
    None,
    Egg,
    Milk,
    Wool
};

class Item : public Ref {
private:
    std::string id;
    std::string name;
    std::string texturePath;
    int maxStackSize;
    int currentStackSize;
    bool isUsable;
    int price;
    int state;

public:

    // 创建新Item
    static Item* create(const std::string& id, const std::string& name, const std::string& texturePath);
    
    // item初始化
    bool init(const std::string& id, const std::string& name, const std::string& texturePath);

    // Getter methods
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getTexturePath() const { return texturePath; }
    int getMaxStackSize() const { return maxStackSize; }
    int getCurrentStackSize() const { return currentStackSize; }
    bool getIsUsable() const { return isUsable; }
    int getPrice() const { return price; }
    int getState() const { return state; }

    // 设置工具
    void setId(const std::string& newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
    void setTexturePath(const std::string& newPath) { texturePath = newPath; }
    void setMaxStackSize(int size) { maxStackSize = size; }
    void setCurrentStackSize(int size) { currentStackSize = size; }
    void setIsUsable(bool usable) { isUsable = usable; }
    void setPrice(int newPrice) { price = newPrice; }
    void setState(int newState) { state = newState; }

    // Item methods
    void use();
    bool canStackWith(Item* other);
    bool addToStack(int amount);
    bool removeFromStack(int amount);
};
// Item.cpp

#include "Item.h"

Item* Item::create(const std::string& id, const std::string& name, const std::string& texturePath) {
    Item* item = new Item();
    if (item && item->init(id, name, texturePath)) {
        item->autorelease();
        return item;
    }
    CC_SAFE_DELETE(item);
    return nullptr;
}

bool Item::init(const std::string& id, const std::string& name, const std::string& texturePath) {
    this->id = id;
    this->name = name;
    this->texturePath = texturePath;
    this->maxStackSize = 99; // 默认最大堆叠数量为99
    this->currentStackSize = 1;
    this->isUsable = true;
    this->price = 10; // 默认价格为10
    this->state = 0;

    return true;
}

void Item::use() {
    if (isUsable) {
        // 物品使用逻辑，子类可以重写
        CCLOG("Using item: %s", name.c_str());
    }
}

bool Item::canStackWith(Item* other) {
    if (!other) {
        return false;
    }

    // 相同ID的物品可以堆叠，且状态相同
    return (id == other->getId() && state == other->getState());
}

bool Item::addToStack(int amount) {
    if (amount <= 0) {
        return false;
    }

    int availableSpace = maxStackSize - currentStackSize;
    if (availableSpace <= 0) {
        return false;
    }

    int addAmount = std::min(amount, availableSpace);
    currentStackSize += addAmount;
    return true;
}

bool Item::removeFromStack(int amount) {
    if (amount <= 0 || currentStackSize < amount) {
        return false;
    }

    currentStackSize -= amount;
    return true;
}
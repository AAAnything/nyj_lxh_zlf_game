// Inventory.h
#pragma once

#include "cocos2d.h"
#include "Item/Item.h"
#include <vector>

USING_NS_CC;

class Inventory : public Ref {
private:
    std::vector<Item*> items;
    int capacity;
    
    // Find a slot for stacking an item
    int findStackableSlot(Item* item) const;
    
    // Find an empty slot
    int findEmptySlot() const;

public:
    static Inventory* create(int capacity = 10); // 默认可用10个快捷栏槽位
    bool init(int capacity);
    
    // Getter methods
    int getCapacity() const { return capacity; }
    Item* getItem(int index) const;
    
    // Item management methods
    bool addItem(Item* item);
    bool removeItem(int index, int amount = 1);
    bool useItem(int index);
    bool swapItems(int index1, int index2);
    void save();
    
    // Utility methods
    int findItem(const std::string& itemId) const;
    bool isFull() const;
    bool isSlotEmpty(int index) const;
    int getTotalItemCount() const;
    
    // Cleanup
    void cleanup();
};
// Inventory.cpp
#include "Inventory.h"

Inventory* Inventory::create(int capacity) {
    Inventory* inventory = new Inventory();
    if (inventory && inventory->init(capacity)) {
        inventory->autorelease();
        return inventory;
    }
    CC_SAFE_DELETE(inventory);
    return nullptr;
}

bool Inventory::init(int capacity) {
    this->capacity = capacity;
    
    // Initialize items vector with nullptr
    items.resize(capacity, nullptr);
    
    return true;
}

Item* Inventory::getItem(int index) const {
    if (index >= 0 && index < capacity) {
        return items[index];
    }
    return nullptr;
}

int Inventory::findStackableSlot(Item* item) const {
    for (int i = 0; i < capacity; i++) {
        Item* slotItem = items[i];
        if (slotItem && slotItem->canStackWith(item) && slotItem->getCurrentStackSize() < slotItem->getMaxStackSize()) {
            return i;
        }
    }
    return -1;
}

int Inventory::findEmptySlot() const {
    for (int i = 0; i < capacity; i++) {
        if (items[i] == nullptr) {
            return i;
        }
    }
    return -1;
}

bool Inventory::addItem(Item* item) {
    if (!item) {
        return false;
    }
    
    // Try to stack with existing items first
    int stackableSlot = findStackableSlot(item);
    if (stackableSlot != -1) {
        Item* slotItem = items[stackableSlot];
        int availableSpace = slotItem->getMaxStackSize() - slotItem->getCurrentStackSize();
        
        if (item->getCurrentStackSize() <= availableSpace) {
            // Add all items to the stack
            slotItem->addToStack(item->getCurrentStackSize());
            return true;
        } else {
            // Add as much as possible to the stack
            slotItem->addToStack(availableSpace);
            item->removeFromStack(availableSpace);
            
            // Find an empty slot for the remaining items
            int emptySlot = findEmptySlot();
            if (emptySlot != -1) {
                items[emptySlot] = item;
                return true;
            }
        }
    } else {
        // Find an empty slot
        int emptySlot = findEmptySlot();
        if (emptySlot != -1) {
            items[emptySlot] = item;
            return true;
        }
    }
    
    // No space left
    return false;
}

bool Inventory::removeItem(int index, int amount) {
    if (index < 0 || index >= capacity || amount <= 0) {
        return false;
    }
    
    Item* item = items[index];
    if (!item) {
        return false;
    }
    
    if (item->getCurrentStackSize() <= amount) {
        // Remove the entire item
        items[index] = nullptr;
        item->release();
        return true;
    } else {
        // Remove only the specified amount
        return item->removeFromStack(amount);
    }
}

bool Inventory::useItem(int index) {
    if (index < 0 || index >= capacity) {
        return false;
    }
    
    Item* item = items[index];
    if (!item || !item->getIsUsable()) {
        return false;
    }
    
    // Use the item
    item->use();
    
    // If the item is consumed on use, remove it
    if (item->getCurrentStackSize() == 1) {
        removeItem(index, 1);
    } else {
        item->removeFromStack(1);
    }
    
    return true;
}

bool Inventory::swapItems(int index1, int index2) {
    if (index1 < 0 || index1 >= capacity || index2 < 0 || index2 >= capacity) {
        return false;
    }

    std::swap(items[index1], items[index2]);       
    return true;
}

void Inventory::save() {
    CCLOG("Inventory saved");
}

int Inventory::findItem(const std::string& itemId) const {
    for (int i = 0; i < capacity; i++) {
        if (items[i] && items[i]->getId() == itemId) {
            return i;
        }
    }
    return -1;
}

bool Inventory::isFull() const {
    return findEmptySlot() == -1;
}

bool Inventory::isSlotEmpty(int index) const {
    if (index < 0 || index >= capacity) {
        return false;
    }
    return items[index] == nullptr;
}

int Inventory::getTotalItemCount() const {
    int count = 0;
    for (int i = 0; i < capacity; i++) {
        if (items[i]) {
            count++;
        }
    }
    return count;
}

// Cleanup
void Inventory::cleanup() {
    for (int i = 0; i < capacity; i++) {
        if (items[i]) {
            items[i]->release();
            items[i] = nullptr;
        }
    }
    items.clear();
}
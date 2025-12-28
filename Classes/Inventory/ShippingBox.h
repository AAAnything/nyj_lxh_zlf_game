#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Inventory.h"
#include "UI/InventoryUI.h"
#include "Core/GameManager.h"
#include "Item/ItemManager.h"

USING_NS_CC;
using namespace cocos2d::ui;

class ShippingBox : public Node {
private:
    TMXTiledMap* tileMap;
    Rect interactionArea;
    bool isOpen;
    
    // UI Components
    Layout* shippingLayout;
    ImageView* shippingBackground;
    Text* moneyText;
    Text* itemPriceText;
    InventoryUI* inventoryUI;
    
    // Selected item
    int selectedItemIndex;
    
    // Setup methods
    void setupUI();
    void setupEventListeners();
    
    // Event handlers
    void onMouseClick(Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    void onItemSlotClicked(Ref* sender, Widget::TouchEventType type);
    
    // Helper methods
    bool isPointInInteractionArea(const Vec2& point) const;
    void updateMoneyDisplay();
    void updateItemPriceDisplay();
    void sellSelectedItem();
    
    // Constants
    const float UI_Z_ORDER = 200.0f;

public:
    static ShippingBox* create(TMXTiledMap* tileMap, const Rect& interactionArea);
    bool init(TMXTiledMap* tileMap, const Rect& interactionArea);
    
    // Overload init() for CREATE_FUNC
    bool init() override {
        return init(nullptr, Rect(0, 0, 0, 0));  // Ä¬ÈÏÖµ
    }

    // Public methods
    void openShippingBox();
    void closeShippingBox();
    void sellItem(Item* item, int quantity = 1);
    bool getIsOpen() const { return isOpen; }
    
    CREATE_FUNC(ShippingBox);
};
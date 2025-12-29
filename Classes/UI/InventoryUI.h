#pragma once

#include "ui/CocosGUI.h"
#include "Inventory/Inventory.h"
#include "Item/ItemManager.h"
#include "ui/UILayoutParameter.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace cocos2d::ui;

class InventoryUI : public Layer {
private:
	Inventory* inventory;

	// UI Components
	Layout* bagLayout;
	ImageView* bagBackground;

	// Item slots and icons
	std::vector<Button*> itemSlots;
	std::vector<ImageView*> itemIcons;
	std::vector<Text*> itemCounts;

	// Drag and drop variables
	bool isDragging;
	int draggedSlotIndex;
	ImageView* draggedItemIcon;

	// Slot size and spacing
	float slotSize;
	float slotSpacing;

	// Setup methods
	void setupBag();
	void setupEventListeners();

	// Event handlers
	void onItemSlotClicked(Ref* sender, Widget::TouchEventType type);
	void onBagToggle(Ref* sender, EventKeyboard::KeyCode keyCode, Event* event);
	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	// Helper methods
	int getSlotIndexAtPosition(const Vec2& position) const;
	void startDrag(int slotIndex, const Vec2& touchPosition);
	void dragItem(const Vec2& touchPosition);
	void endDrag(const Vec2& touchPosition);
	void updateUI();

	// Layout constants
	const int BAG_SLOTS = 10;

public:
	static InventoryUI* create(Inventory* inventory); // Keep only one declaration of the `create` method.
	bool init(Inventory* inventory);

	// Update methods
	void refreshUI();

	// Getter methods
	Inventory* getInventory() const { return inventory; }
	const std::vector<Button*>& getInventorySlots() const { return itemSlots; }
};

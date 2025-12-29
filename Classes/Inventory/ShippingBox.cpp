#include "ShippingBox.h"

ShippingBox* ShippingBox::create(TMXTiledMap* tileMap, const Rect& interactionArea) {
    ShippingBox* box = new ShippingBox();
    if (box && box->init(tileMap, interactionArea)) {
        box->autorelease();
        return box;
    }
    CC_SAFE_DELETE(box);
    return nullptr;
}

bool ShippingBox::init(TMXTiledMap* tileMap, const Rect& interactionArea) {
    if (!Node::init()) {
        return false;
    }
    
    this->tileMap = tileMap;
    this->interactionArea = interactionArea;
    this->isOpen = false;
    this->selectedItemIndex = -1;
    this->inventoryUI = nullptr;
    
    // Setup UI components
    setupUI();
    setupEventListeners();
    
    return true;
}

void ShippingBox::setupUI() {
    // Create shipping background
    std::string bgPath = "icon/bagBackground.png";
    if (!FileUtils::getInstance()->isFileExist(bgPath)) {
        bgPath = "Resources/" + bgPath;
    }
    shippingBackground = ImageView::create(bgPath);
    shippingBackground->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2, Director::getInstance()->getWinSize().height / 2));
    shippingBackground->setVisible(false);
    this->addChild(shippingBackground, UI_Z_ORDER);
    
    // Create shipping layout
    shippingLayout = Layout::create();
    shippingLayout->setContentSize(Size(400, 300));
    shippingLayout->setPosition(Vec2(
        Director::getInstance()->getWinSize().width / 2 - shippingLayout->getContentSize().width / 2,
        Director::getInstance()->getWinSize().height / 2 - shippingLayout->getContentSize().height / 2
    ));
    shippingLayout->setVisible(false);
    this->addChild(shippingLayout, UI_Z_ORDER + 10);
    
    // Create title text
    Text* titleText = Text::create("Shipping Box", "fonts/Marker Felt.ttf", 24);
    titleText->setPosition(Vec2(shippingLayout->getContentSize().width / 2, shippingLayout->getContentSize().height - 30));
    titleText->setColor(Color3B::WHITE);
    shippingLayout->addChild(titleText);
    
    // Create money display
    moneyText = Text::create("Money: 0", "fonts/Marker Felt.ttf", 20);
    moneyText->setPosition(Vec2(50, shippingLayout->getContentSize().height - 60));
    moneyText->setColor(Color3B::YELLOW);
    shippingLayout->addChild(moneyText);
    
    // Create item price display
    itemPriceText = Text::create("Selected Item: None", "fonts/Marker Felt.ttf", 18);
    itemPriceText->setPosition(Vec2(shippingLayout->getContentSize().width / 2, 50));
    itemPriceText->setColor(Color3B::WHITE);
    shippingLayout->addChild(itemPriceText);
    
    // Create instructions text
    Text* instructionsText = Text::create("Click an item to select, press 'Q' to sell", "fonts/Marker Felt.ttf", 16);
    instructionsText->setPosition(Vec2(shippingLayout->getContentSize().width / 2, 20));
    instructionsText->setColor(Color3B::GRAY);
    shippingLayout->addChild(instructionsText);
}

void ShippingBox::setupEventListeners() {
    // Mouse listener for opening shipping box
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(ShippingBox::onMouseClick, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
    // Keyboard listener for selling items (Q key) and closing (Escape key)
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(ShippingBox::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void ShippingBox::onMouseClick(Event* event) {
    EventMouse* mouseEvent = static_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    
    // Convert mouse position to world coordinates if needed
    mousePos = Director::getInstance()->convertToGL(mousePos);
    
    // Check if click is on interaction area and shipping box is closed
    if (!isOpen && isPointInInteractionArea(mousePos)) {
        openShippingBox();
    } else if (isOpen && !shippingBackground->getBoundingBox().containsPoint(mousePos)) {
        // Close if clicking outside the shipping box
        closeShippingBox();
    }
}

void ShippingBox::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    if (isOpen) {
        if (keyCode == EventKeyboard::KeyCode::KEY_Q) {
            // Sell selected item
            sellSelectedItem();
        } else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            // Close shipping box
            closeShippingBox();
        }
    }
}

void ShippingBox::onItemSlotClicked(Ref* sender, Widget::TouchEventType type) {
    if (type != Widget::TouchEventType::ENDED) {
        return;
    }
    
    Button* clickedButton = static_cast<Button*>(sender);
    selectedItemIndex = clickedButton->getTag();
    
    // Update item price display
    updateItemPriceDisplay();
}

bool ShippingBox::isPointInInteractionArea(const Vec2& point) const {
    // Convert point to tile map coordinates if needed
    Vec2 tileMapPos = tileMap->convertToNodeSpace(point);
    return interactionArea.containsPoint(tileMapPos);
}

void ShippingBox::openShippingBox() {
    isOpen = true;
    
    // Show shipping box UI
    shippingBackground->setVisible(true);
    shippingLayout->setVisible(true);
    
    // Get player inventory and create inventory UI if not exists
    GameManager* gameManager = GameManager::getInstance();
    Inventory* inventory = gameManager->getPlayerInventory();
    
    if (!inventoryUI) {
        inventoryUI = InventoryUI::create(inventory);
        this->addChild(inventoryUI, UI_Z_ORDER + 20);
        
        // Override item slot click behavior for shipping box
        for (Button* slot : inventoryUI->getInventorySlots()) {
            slot->setTouchEnabled(false);
            slot->addTouchEventListener(CC_CALLBACK_2(ShippingBox::onItemSlotClicked, this));
        }
    } else {
        inventoryUI->setVisible(true);
    }
    
    // Update money display
    updateMoneyDisplay();
    
    // Update item price display
    updateItemPriceDisplay();
    
    CCLOG("Shipping box opened");
}

void ShippingBox::closeShippingBox() {
    isOpen = false;
    
    // Hide shipping box UI
    shippingBackground->setVisible(false);
    shippingLayout->setVisible(false);
    
    if (inventoryUI) {
        inventoryUI->setVisible(false);
    }
    
    // Reset selected item
    selectedItemIndex = -1;
    
    CCLOG("Shipping box closed");
}

void ShippingBox::sellItem(Item* item, int quantity) {
    if (!item || quantity <= 0) {
        return;
    }
    
    GameManager* gameManager = GameManager::getInstance();
    int totalPrice = item->getPrice() * quantity;
    
    // Add money to player
    gameManager->addMoney(totalPrice);
    
    // Update UI
    updateMoneyDisplay();
    
    CCLOG("Sold %d x %s for %d gold", quantity, item->getName().c_str(), totalPrice);
}

void ShippingBox::sellSelectedItem() {
    if (selectedItemIndex == -1 || !inventoryUI) {
        return;
    }
    
    GameManager* gameManager = GameManager::getInstance();
    Inventory* inventory = gameManager->getPlayerInventory();
    
    Item* item = inventory->getItem(selectedItemIndex);
    if (item) {
        // Sell all of the item
        int quantity = item->getCurrentStackSize();
        sellItem(item, quantity);
        
        // Remove the item from inventory
        inventory->removeItem(selectedItemIndex, quantity);
        
        // Reset selected item
        selectedItemIndex = -1;
        
        // Update UI
        inventoryUI->refreshUI();
        updateItemPriceDisplay();
        
        CCLOG("Sold %d x %s", quantity, item->getName().c_str());
    }
}

void ShippingBox::updateMoneyDisplay() {
    GameManager* gameManager = GameManager::getInstance();
    int money = gameManager->getMoney();
    moneyText->setString(StringUtils::format("Money: %d", money));
}

void ShippingBox::updateItemPriceDisplay() {
    GameManager* gameManager = GameManager::getInstance();
    Inventory* inventory = gameManager->getPlayerInventory();
    
    if (selectedItemIndex != -1 && inventory->getItem(selectedItemIndex)) {
        Item* item = inventory->getItem(selectedItemIndex);
        int totalPrice = item->getPrice() * item->getCurrentStackSize();
        itemPriceText->setString(StringUtils::format("Selected: %s x %d - Price: %d", 
            item->getName().c_str(), item->getCurrentStackSize(), totalPrice));
    } else {
        itemPriceText->setString("Selected Item: None");
    }
}
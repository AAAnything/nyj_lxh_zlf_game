#include"InventoryUI.h"

// 静态创建函数：用于实例化背包UI
InventoryUI* InventoryUI::create(Inventory* inventory) {
    InventoryUI* ui = new InventoryUI();
    // 初始化成功则进入 autorelease 队列，否则销毁
    if (ui && ui->init(inventory)) {
        ui->autorelease();
        return ui;
    }
    CC_SAFE_DELETE(ui);
    return nullptr;
}

// 初始化函数：设置基础变量并构建界面
bool InventoryUI::init(Inventory* inventory) {
    if (!Layer::init()) {
        return false;
    }

    this->inventory = inventory;
    this->isDragging = false;         // 初始状态未在拖拽
    this->draggedSlotIndex = -1;      // 初始没有被拖拽的索引
    this->draggedItemIcon = nullptr;  // 初始拖拽图标为空

    // 设置格子的默认大小(64x64)和间距(4像素)
    this->slotSize = 64.0f;
    this->slotSpacing = 4.0f;

    // 调用设置界面的子函数
    setupBag();            // 创建背景和格子
    setupEventListeners(); // 注册点击和拖拽监听

    // 第一次同步数据到UI
    updateUI();

    return true;
}

// 构建背包的视觉组件
void InventoryUI::setupBag() {
    // 1. 创建并添加背包背景图
    bagBackground = ImageView::create("icon/bagBackground.png");
    // 设置在屏幕底部的居中位置
    bagBackground->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2, Director::getInstance()->getWinSize().height / 6));
    this->addChild(bagBackground, 10);

    // 2. 创建背包布局容器（使用线性布局）
    bagLayout = Layout::create();
    // 计算布局总宽度：(格子宽+间距) * 数量 - 最后一个格子的多余间距
    bagLayout->setContentSize(Size((slotSize + slotSpacing) * BAG_SLOTS - slotSpacing, slotSize));
    // 将布局容器水平居中放置
    bagLayout->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2 - bagLayout->getContentSize().width / 2, 50));
    bagLayout->setLayoutType(Layout::Type::HORIZONTAL); // 设置为水平排列
    this->addChild(bagLayout, 20);

    // 3. 创建格子间距参数
    LinearLayoutParameter* layoutParam = LinearLayoutParameter::create();
    layoutParam->setMargin(Margin(slotSpacing / 2, 0, slotSpacing / 2, 0));

    // 4. 循环创建每一个物品槽位
    for (int i = 0; i < BAG_SLOTS; i++) {
        // 创建格子按钮
        Button* slotButton = Button::create();
        slotButton->setContentSize(Size(slotSize, slotSize));
        slotButton->setTag(i); // 将索引存入Tag，方便后面知道点的是第几个
        slotButton->addTouchEventListener(CC_CALLBACK_2(InventoryUI::onItemSlotClicked, this));

        slotButton->setLayoutParameter(layoutParam->clone());
        bagLayout->addChild(slotButton);

        // 创建物品图标（默认隐藏，有东西时才显示）
        ImageView* itemIcon = ImageView::create();
        itemIcon->setContentSize(Size(slotSize * 0.8f, slotSize * 0.8f)); // 图标比格子稍小
        itemIcon->setPosition(Vec2(slotSize / 2, slotSize / 2));
        itemIcon->setVisible(false);
        slotButton->addChild(itemIcon);

        // 创建物品数量文本（显示在右下角）
        Text* itemCount = Text::create("", "fonts/arial.ttf", 16);
        itemCount->setPosition(Vec2(slotSize - 10, 10));
        itemCount->setAnchorPoint(Vec2(1, 0)); // 右下角对齐
        itemCount->setColor(Color3B::WHITE);
        itemCount->setVisible(false);
        slotButton->addChild(itemCount);

        // 存入数组/向量，方便后续 updateUI 快速找到对应的UI控件
        itemSlots.push_back(slotButton);
        itemIcons.push_back(itemIcon);
        itemCounts.push_back(itemCount);
    }
}

// 注册触摸监听器（用于处理拖拽逻辑）
void InventoryUI::setupEventListeners() {
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(InventoryUI::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(InventoryUI::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(InventoryUI::onTouchEnded, this);
    touchListener->setSwallowTouches(true); // 吞掉触摸，防止穿透到下层场景
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 当玩家“点击”某个槽位时触发（短按释放）
void InventoryUI::onItemSlotClicked(Ref* sender, Widget::TouchEventType type) {
    if (type != Widget::TouchEventType::ENDED) {
        return;
    }

    Button* clickedButton = static_cast<Button*>(sender);
    int slotIndex = clickedButton->getTag();

    // 获取数据层对应的物品
    Item* item = inventory->getItem(slotIndex);
    if (item) {
        CCLOG("Clicked item: %s in slot %d", item->getName().c_str(), slotIndex);
        // 调用数据层的“使用”方法
        inventory->useItem(slotIndex);
        // 使用后（如药水消耗了）更新界面显示
        updateUI();
    }
}

// 触摸开始：判断是否点中了某个有物品的格子，开启拖拽
bool InventoryUI::onTouchBegan(Touch* touch, Event* event) {
    Vec2 touchPos = touch->getLocation();

    // 检查点击坐标落在哪一个槽位索引上
    int slotIndex = getSlotIndexAtPosition(touchPos);
    if (slotIndex != -1 && inventory->getItem(slotIndex)) {
        // 如果格子里有东西，开始拖拽逻辑
        startDrag(slotIndex, touchPos);
        return true;
    }

    return false;
}

// 触摸移动：让虚幻的图标跟着手指走
void InventoryUI::onTouchMoved(Touch* touch, Event* event) {
    if (isDragging) {
        Vec2 touchPos = touch->getLocation();
        dragItem(touchPos);
    }
}

// 触摸结束：判断落点，决定是“交换位置”还是“回到原处”
void InventoryUI::onTouchEnded(Touch* touch, Event* event) {
    if (isDragging) {
        Vec2 touchPos = touch->getLocation();
        endDrag(touchPos);
    }
}

// 辅助工具：根据点击的屏幕坐标，遍历并判断点中了哪一个格子
int InventoryUI::getSlotIndexAtPosition(const Vec2& position) const {
    for (int i = 0; i < BAG_SLOTS; i++) {
        Button* slot = itemSlots[i];
        // 检查点击点是否在格子的碰撞框（BoundingBox）内
        if (slot->getBoundingBox().containsPoint(position)) {
            return i;
        }
    }
    return -1;
}

// 初始化拖拽状态：创建一个临时的图标随手指移动
void InventoryUI::startDrag(int slotIndex, const Vec2& touchPosition) {
    isDragging = true;
    draggedSlotIndex = slotIndex;

    Item* draggedItem = inventory->getItem(slotIndex);
    if (!draggedItem) {
        return;
    }

    // 创建一个新的临时 ImageView 作为拖拽时的影子
    draggedItemIcon = ImageView::create(draggedItem->getTexturePath());
    draggedItemIcon->setContentSize(Size(slotSize, slotSize));
    draggedItemIcon->setPosition(touchPosition);
    draggedItemIcon->setOpacity(200); // 设置半透明效果
    this->addChild(draggedItemIcon, 200); // 放在最高层级

    // 拖拽时暂时隐藏原格子里的图标和文字
    itemIcons[slotIndex]->setVisible(false);
    itemCounts[slotIndex]->setVisible(false);
}

// 拖拽进行中：更新影子图标位置
void InventoryUI::dragItem(const Vec2& touchPosition) {
    if (isDragging && draggedItemIcon) {
        draggedItemIcon->setPosition(touchPosition);
    }
}

// 拖拽结束：处理逻辑交换
void InventoryUI::endDrag(const Vec2& touchPosition) {
    if (!isDragging || draggedSlotIndex == -1) {
        return;
    }

    // 获取手指松开时的落点槽位索引
    int targetSlotIndex = getSlotIndexAtPosition(touchPosition);

    // 如果落在一个有效格子内，且不是原来的格子，则交换数据
    if (targetSlotIndex != -1 && targetSlotIndex != draggedSlotIndex) {
        inventory->swapItems(draggedSlotIndex, targetSlotIndex);
    }

    // 销毁临时的拖拽影子图标
    if (draggedItemIcon) {
        draggedItemIcon->removeFromParentAndCleanup(true);
        draggedItemIcon = nullptr;
    }

    // 重置状态变量
    isDragging = false;
    draggedSlotIndex = -1;

    // 交换完成后，刷新界面显示
    updateUI();
}

// 核心函数：根据 Inventory 数据层同步刷新 UI
void InventoryUI::updateUI() {
    // 遍历所有槽位
    for (int i = 0; i < inventory->getCapacity(); i++) {
        Item* item = inventory->getItem(i);

        if (item) {
            // 如果该位置有物品：加载纹理并显示
            itemIcons[i]->loadTexture(item->getTexturePath());
            itemIcons[i]->setVisible(true);

            // 如果数量大于1：显示数字标签，否则隐藏数字
            if (item->getCurrentStackSize() > 1) {
                itemCounts[i]->setString(std::to_string(item->getCurrentStackSize()));
                itemCounts[i]->setVisible(true);
            }
            else {
                itemCounts[i]->setVisible(false);
            }
        }
        else {
            // 如果该位置为空：图标和数字统统隐藏
            itemIcons[i]->setVisible(false);
            itemCounts[i]->setVisible(false);
        }
    }
}

// 刷新界面的公共接口
void InventoryUI::refreshUI() {
    updateUI();
}
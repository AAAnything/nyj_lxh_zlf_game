#include "NPC.h"

USING_NS_CC;

// 创建NPC
NPC* NPC::create(const std::string& npcImage)
{
    NPC* npc = new (std::nothrow) NPC();
    if (npc && npc->init(npcImage))
    {
        npc->autorelease();
        return npc;
    }
    CC_SAFE_DELETE(npc);
    return nullptr;
}



// 初始化
bool NPC::init(const std::string& npcImage)
{
    // 调用父类Sprite的init
    if (!Sprite::initWithFile(npcImage))
    {
        // 如果图片加载失败，使用默认图片
        if (!Sprite::initWithFile("npcImages/cleaner.png"))
        {
            CCLOG("错误：无法加载NPC图片: %s", npcImage.c_str());
            return false;
        }
    }

    // 初始化变量
    _name = "NPC";
    _currentDialogueIndex = 0;
    _isTalking = false;

    // 默认对话
    _dialogueList.push_back("How are you?");
    _dialogueList.push_back("What can I help you?");
   

    // 创建UI元素
    createUI();

    // 启用触摸/鼠标事件
    this->setUserObject(this);  // 保存自身引用
    return true;
}

// 创建UI元素
void NPC::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // 1. 创建对话框（初始隐藏）
    _dialogueBubble = Sprite::create("npcImages/dialogueBox.png");
    float dialogueBubbleX = origin.x + 35;
    float dialogueBubbleY = origin.y - visibleSize.height / 2 + _dialogueBubble->getContentSize().height / 2-20;
    _dialogueBubble->setPosition(Vec2(
        dialogueBubbleX, 
        dialogueBubbleY));
    // 初始隐藏
    _dialogueBubble->setVisible(false);
    this->addChild(_dialogueBubble, 3);


   
    // 2. 创建头像（初始隐藏）
    _headSprite = Sprite::create("npcImages/Haley Talk.png"); 
    float headSpriteX = dialogueBubbleX+373;
    float headSpriteY = dialogueBubbleY+40;
    _headSprite->setPosition(Vec2(
        headSpriteX,
        headSpriteY
    ));
    // 初始隐藏
    _headSprite->setVisible(false); 
    this->addChild(_headSprite, 4);  // 比对话框高一级



    // 3. 创建名字标签
    _nameLabel = Label::createWithTTF(_name, "fonts/Marker Felt.ttf", 48);
    float nameLabelX = headSpriteX;
    float nameLabelY = headSpriteY - 185;
    _nameLabel->setColor(Color3B::BLACK);
    _nameLabel->enableShadow(Color4B::BLACK, Size(1, -1));
    _nameLabel->setPosition(Vec2(
        nameLabelX,
        nameLabelY));
    // 初始隐藏
    _nameLabel->setVisible(false);
    this->addChild(_nameLabel, 4);   // 比对话框高一级

    


    // 4. 创建对话文本标签
    _dialogueLabel = Label::createWithTTF("  ", "fonts/Marker Felt.ttf", 48);
    _dialogueLabel->setDimensions(730, 0);  // 宽度限制，高度自动扩展

    // 关键：只用水平左对齐，垂直方向自然排列
    _dialogueLabel->setAnchorPoint(Vec2(0, 1));      // 锚点在左上角
    _dialogueLabel->setAlignment(TextHAlignment::LEFT);  // 水平左对齐
    // 不要设置 setVerticalAlignment! 让文字自然从上到下排列

    // 获取对话框尺寸并设置相对位置
    Size bubbleSize = _dialogueBubble->getContentSize();
    // 设置到对话框左上角，但留出更多上边距
    _dialogueLabel->setPosition(Vec2(45, bubbleSize.height - 40)); // 上边距40像素

    _dialogueLabel->setColor(Color3B::WHITE);
    _dialogueBubble->addChild(_dialogueLabel);

}



// 设置对话（多句）
void NPC::setDialogue(const std::vector<std::string>& dialogue)
{
    _dialogueList = dialogue;
    _currentDialogueIndex = 0;
}

// 设置对话（单句）
void NPC::setDialogue(const std::string& dialogue)
{
    _dialogueList.clear();
    _dialogueList.push_back(dialogue);
    _currentDialogueIndex = 0;
}



// 检查玩家是否在交互范围内
bool NPC::isInRange(cocos2d::Vec2 position)
{
    // 计算玩家和NPC之间的距离
    Vec2 npcWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
    float distance = position.distance(npcWorldPos);

    // 交互范围：150像素
    float interactionRange = 150.0f;
    return distance <= interactionRange;
}



// 鼠标右键点击事件
void NPC::onMouseDown(Event* event)
{
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        // 获取鼠标位置
        Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());

        // 检查是否点击到NPC
        Rect npcRect = this->getBoundingBox();
        Vec2 npcWorldPos = this->getParent()->convertToWorldSpace(this->getPosition());
        npcRect.origin = npcWorldPos - Vec2(npcRect.size.width / 2, npcRect.size.height / 2);

        if (npcRect.containsPoint(mousePos))
        {
            startDialogue();
        }
    }
}



// 开始对话
void NPC::startDialogue()
{
    if (_isTalking || _dialogueList.empty())
    {
        return;
    }

    _isTalking = true;
    _currentDialogueIndex = 0;



    // 显示对话框
    _dialogueBubble->setVisible(true);
    _dialogueBubble->setScale(0.1f);
    // 缩放动画
    auto scaleIn = ScaleTo::create(0.2f, 1.0f);
    auto easeOut = EaseBackOut::create(scaleIn);
    _dialogueBubble->runAction(easeOut);



    // 显示头像
    _headSprite->setVisible(true);
    _headSprite->setScale(0.1f); // 初始缩到0.1倍（几乎看不见）
    // 头像动画：时长和对话框一致，晚0.05秒开始（层次感）
    auto scaleInHead = ScaleTo::create(0.2f, 1.0f);
    auto easeOutHead = EaseBackOut::create(scaleInHead);
    auto delayHead = DelayTime::create(0.05f); // 延迟0.05秒
    auto seqHead = Sequence::create(delayHead, easeOutHead, nullptr); // 先延迟再放大
    _headSprite->runAction(seqHead);



    // 显示NPC名字
    _nameLabel->setVisible(true);
   


    
    // 显示第一句对话
    updateDialogueDisplay();



    // 添加触摸监听器，点击屏幕显示下一句
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (_isTalking)
        {
            showNextDialogue();
            return true;
        }
        return false;
        };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        listener, this);



    // 保存监听器引用，对话结束后移除
    this->setUserObject(listener);
}

// 更新对话显示
void NPC::updateDialogueDisplay()
{
    if (_currentDialogueIndex < _dialogueList.size())
    {
        std::string currentText = _dialogueList[_currentDialogueIndex];

        // 1. 先设置第一个字母（固定显示）
        if (!currentText.empty()) {
            std::string firstChar = currentText.substr(0, 1);
            _dialogueLabel->setString(firstChar);
        }
        else {
            _dialogueLabel->setString("");
        }

        // 2. 停止之前的动画
        this->stopAllActions();

        // 3. 如果只有一个字符，直接返回
        if (currentText.length() <= 1) {
            return;
        }

        // 4. 创建剩余字符的动画序列
        Vector<FiniteTimeAction*> actions;

        // 添加一个初始延迟，让第一个字母先显示一会儿
        actions.pushBack(DelayTime::create(0.1f));

        // 从第二个字符开始逐个追加（下标 i=1）
        for (int i = 1; i < currentText.length(); i++)
        {
            // 创建追加字符的回调
            auto appendChar = CallFunc::create([this, currentText, i]() {
                // 获取从第一个到第i个字符的子串（0到i，包含i）
                std::string partialText = currentText.substr(0, i + 1);
                _dialogueLabel->setString(partialText);
                });

            actions.pushBack(appendChar);

            // 如果不是最后一个字符，添加延迟
            if (i < currentText.length() - 1) {
                actions.pushBack(DelayTime::create(0.05f));
            }
        }

        // 执行序列
        auto sequence = Sequence::create(actions);
        this->runAction(sequence);
    }
}

// 显示下一句对话
void NPC::showNextDialogue()
{
    _currentDialogueIndex++;

    if (_currentDialogueIndex < _dialogueList.size())
    {
        updateDialogueDisplay();
    }
    else
    {
        endDialogue();
    }
}


// 结束对话
void NPC::endDialogue()
{
    _isTalking = false;



    // 淡出对话框——0.8秒淡出
    auto fadeOutDialogue = FadeOut::create(0.8f);
    auto removeDialogue = CallFunc::create([this]() {
        _dialogueBubble->setVisible(false);
        _dialogueBubble->setOpacity(255);
        });

    _dialogueBubble->runAction(Sequence::create(fadeOutDialogue, removeDialogue, nullptr));



    // 淡出头像——0.3秒淡出
    auto fadeOutHead = FadeOut::create(0.3f);
    auto removeHead = CallFunc::create([this]() {
        _headSprite->setVisible(false);
        _headSprite->setOpacity(255);
        });

    _headSprite->runAction(Sequence::create(fadeOutHead, removeHead, nullptr));



    // 淡出名字——0.5秒淡出
    auto fadeOutName = FadeOut::create(0.5f);
    auto removeName = CallFunc::create([this]() {
        _nameLabel->setVisible(false);
        _nameLabel->setOpacity(255);
        });

    _nameLabel->runAction(Sequence::create(fadeOutName, removeName, nullptr));



    // 移除触摸监听器
    auto listener = dynamic_cast<EventListenerTouchOneByOne*>(this->getUserObject());
    if (listener)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }



    // 调用对话结束回调
    if (_dialogueEndCallback)
    {
        _dialogueEndCallback();
    }
}

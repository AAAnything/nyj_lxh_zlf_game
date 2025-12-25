#include "MainMenuScene.h"
#include "SimpleAudioEngine.h"
#include "MenuManager.h"

USING_NS_CC;

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

// 新游戏按钮回调函数
void MainMenu::menuNewGameCallback(cocos2d::Ref* pSender)
{
    // 通过MenuManager切换到游戏主场景
    MenuManager::getInstance()->goToGameScene();
}

bool MainMenu::init()
{
    if (!Scene::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    // Sprite是Cocos2d-x游戏引擎中的一个核心类，用来表示2D图像/精灵。
    // 添加初始化背景
    auto initialBackground = Sprite::create("background.png");
    // 置于屏幕中间
    initialBackground->setPosition(Vec2(
        visibleSize.width / 2 + origin.x, 
        visibleSize.height / 2 + origin.y));
    // 置于底层
    this->addChild(initialBackground, 0);

    

    // 1.创建游戏标题
    auto titleBtn = Sprite::create("icon/title.png");
    // 置于屏幕中间
    titleBtn->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - titleBtn->getContentSize().height / 2 - 50));
    // 置于背景上层
    this->addChild(titleBtn, 1);

    
    // 2. 创建新游戏按钮
    auto newGameBtn = MenuItemImage::create(
        "icon/newGameButton.png",
        "icon/newGameButtonOn.png",
        CC_CALLBACK_1(MainMenu::menuNewGameCallback, this));

    // 设置新游戏按钮位置
    float newGameX = origin.x + newGameBtn->getContentSize().width / 2 + 40;  
    float newGameY = origin.y + newGameBtn->getContentSize().height / 2 + 20;  
    newGameBtn->setPosition(Vec2(newGameX, newGameY));



    // 3. 创建加载按钮
    auto loadBtn = MenuItemImage::create(
        "icon/loadGameButton.png",
        "icon/loadGameButtonOn.png",
        CC_CALLBACK_1(MainMenu::menuCloseCallback, this));  // 建议用不同回调函数

    // 设置加载按钮位置
    float loadX = newGameX + visibleSize.width/4;  
    float loadY = newGameY;  
    loadBtn->setPosition(Vec2(loadX, loadY));



    // 4. 创建合作按钮
    auto coopBtn = MenuItemImage::create(
        "icon/cooperationButton.png",
        "icon/cooperationButtonOn.png",
        CC_CALLBACK_1(MainMenu::menuCloseCallback, this));  // 建议用不同回调函数

    // 设置合作按钮位置
    float coopX = loadX + visibleSize.width/4;  
    float coopY = newGameY; 
    coopBtn->setPosition(Vec2(coopX, coopY));



    // 5. 创建退出按钮
    auto exitBtn = MenuItemImage::create(
        "icon/exitButton.png",
        "icon/exitButtonOn.png",
        CC_CALLBACK_1(MainMenu::menuCloseCallback, this));  // 建议用不同回调函数

    // 设置退出按钮位置
    float exitX = coopX + visibleSize.width / 4;  
    float exitY = newGameY; 
    exitBtn->setPosition(Vec2(exitX, exitY));



    // 6. 创建开发人员信息
    auto developerBtn = MenuItemImage::create(
        "icon/versionButton.png",
        "icon/versionButtonOn.png",
        CC_CALLBACK_1(MainMenu::menuCloseCallback, this));  // 建议用不同回调函数

    // 设置开发人员信息位置
    float developerX = exitX + (exitBtn->getContentSize().width - developerBtn->getContentSize().width) / 2;
    float developerY = newGameY+200;
    developerBtn->setPosition(Vec2(developerX, developerY));



    // 7. 创建菜单并添加所有按钮
    auto menu = Menu::create(newGameBtn, loadBtn, coopBtn, exitBtn, developerBtn, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
   

    return true;
}


// 用以退出游戏
void MainMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}


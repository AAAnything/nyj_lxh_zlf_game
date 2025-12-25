# MenuManager 框架与使用说明

## 一、MenuManager 框架结构

MenuManager 是一个单例模式的场景管理类，负责游戏中不同场景之间的切换。

### 1. 头文件结构 (MenuManager.h)

```cpp
#ifndef __MENU_MANAGER_H__
#define __MENU_MANAGER_H__

#include "cocos2d.h"

class MenuManager
{
public:
    // 获取MenuManager的单例实例
    static MenuManager* getInstance();
    
    // 释放MenuManager实例
    static void destroyInstance();
    
    // 切换到指定场景
    void switchScene(cocos2d::Scene* scene);
    
    // 切换到主菜单场景
    void goToMainMenu();
    
    // 切换到游戏主场景
    void goToGameScene();
    
    // 切换到设置场景
    void goToSettingsScene();
    
private:
    // 私有构造函数，防止外部创建实例
    MenuManager();
    
    // 私有析构函数
    ~MenuManager();
    
    // 单例实例
    static MenuManager* _instance;
};

#endif // __MENU_MANAGER_H__
```

### 2. 实现文件结构 (MenuManager.cpp)

```cpp
#include "MenuManager.h"
#include "MainMenuScene.h"
#include "GameScene.h"

// 初始化静态成员变量
MenuManager* MenuManager::_instance = nullptr;

MenuManager::MenuManager()
{
}

MenuManager::~MenuManager()
{
}

MenuManager* MenuManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new MenuManager();
    }
    return _instance;
}

void MenuManager::destroyInstance()
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}

void MenuManager::switchScene(cocos2d::Scene* scene)
{
    if (scene)
    {
        auto director = cocos2d::Director::getInstance();

        // 检查当前是否有场景
        if (director->getRunningScene())
        {
            // 有场景则使用replaceScene切换
            director->replaceScene(cocos2d::TransitionFade::create(0.5f, scene));
        }
        else
        {
            // 没有场景则使用runWithScene
            director->runWithScene(scene);
        }
    }
}

void MenuManager::goToMainMenu()
{
    // 创建主菜单场景并切换
    auto scene = MainMenu::createScene();
    switchScene(scene);
}

void MenuManager::goToGameScene()
{
    // 创建游戏场景并切换
    auto scene = GameScene::createScene();
    switchScene(scene);
}

void MenuManager::goToSettingsScene()
{
    // 这里可以创建设置场景并切换
    // 由于当前项目中没有设置场景类，暂时先创建一个空场景
    auto scene = cocos2d::Scene::create();
    switchScene(scene);
}
```

## 二、MenuManager 使用方法

### 1. 基本使用流程

1. **获取MenuManager实例**：
   ```cpp
   MenuManager* menuManager = MenuManager::getInstance();
   ```

2. **切换场景**：
   ```cpp
   // 切换到游戏主场景
   menuManager->goToGameScene();
   
   // 切换到主菜单场景
   menuManager->goToMainMenu();
   
   // 切换到设置场景
   menuManager->goToSettingsScene();
   ```

3. **自定义场景切换**：
   ```cpp
   // 创建自定义场景
   auto customScene = CustomScene::createScene();
   
   // 使用MenuManager切换场景
   menuManager->switchScene(customScene);
   ```

4. **释放实例**（在游戏结束时）：
   ```cpp
   MenuManager::destroyInstance();
   ```

### 2. 在主菜单中使用MenuManager

在主菜单场景中，为新游戏按钮添加回调函数，通过MenuManager切换到游戏主场景：

```cpp
// 新游戏按钮回调函数
void MainMenu::menuNewGameCallback(cocos2d::Ref* pSender)
{
    // 通过MenuManager切换到游戏主场景
    MenuManager::getInstance()->goToGameScene();
}
```

## 三、游戏主场景 (GameScene) 实现

### 1. 功能说明

GameScene 实现了以下功能：
- 加载地图文件（用户需自行选择farmMap.png）
- 使用WASD键控制视野移动
- 视野边界限制，确保不会超出地图范围

### 2. 核心代码解析

#### 地图初始化：
```cpp
// 初始化地图精灵（图片文件位置留空，用户自行选择）
// 这里暂时使用一个占位符，用户需要替换为实际的farmMap.png路径
mapSprite = Sprite::create("farmMap.png");

if (mapSprite)
{
    // 获取地图大小
    mapSize = mapSprite->getContentSize();
    
    // 设置地图位置（初始在地图中心）
    mapPosition = Vec2(mapSize.width / 2, mapSize.height / 2);
    
    // 设置地图的锚点和位置，使地图中心与视图中心对齐
    mapSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    mapSprite->setPosition(Vec2(viewSize.width / 2, viewSize.height / 2));
    
    // 添加地图到场景
    this->addChild(mapSprite, 0);
}
```

#### 键盘事件处理：
```cpp
void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 记录按键按下状态
    keys[keyCode] = true;
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    // 记录按键释放状态
    keys[keyCode] = false;
}
```

#### 视野移动更新：
```cpp
void GameScene::update(float delta)
{
    // 计算移动方向
    Vec2 moveDirection(0, 0);
    
    // 检测WASD键
    if (keys[EventKeyboard::KeyCode::KEY_W])
    {
        moveDirection.y += 1;
    }
    if (keys[EventKeyboard::KeyCode::KEY_S])
    {
        moveDirection.y -= 1;
    }
    if (keys[EventKeyboard::KeyCode::KEY_A])
    {
        moveDirection.x -= 1;
    }
    if (keys[EventKeyboard::KeyCode::KEY_D])
    {
        moveDirection.x += 1;
    }
    
    // 归一化移动方向
    if (moveDirection != Vec2::ZERO)
    {
        moveDirection.normalize();
    }
    
    // 计算新的地图位置
    Vec2 newPosition = mapPosition + moveDirection * moveSpeed * delta;
    
    // 限制地图位置在地图边界内
    // 确保视图不会超出地图范围
    float minX = viewSize.width / 2;
    float maxX = mapSize.width - viewSize.width / 2;
    float minY = viewSize.height / 2;
    float maxY = mapSize.height - viewSize.height / 2;
    
    // 检查地图是否大于视图
    if (mapSize.width > viewSize.width)
    {
        newPosition.x = clampf(newPosition.x, minX, maxX);
    }
    else
    {
        newPosition.x = mapSize.width / 2;
    }
    
    if (mapSize.height > viewSize.height)
    {
        newPosition.y = clampf(newPosition.y, minY, maxY);
    }
    else
    {
        newPosition.y = mapSize.height / 2;
    }
    
    // 更新地图位置
    mapPosition = newPosition;
    
    // 更新地图精灵的位置，实现视野移动效果
    // 当地图位置变化时，地图精灵的位置需要反向移动
    if (mapSprite)
    {
        Vec2 mapSpritePosition = Vec2(viewSize.width / 2, viewSize.height / 2);
        
        // 如果地图大于视图，计算地图精灵的偏移量
        if (mapSize.width > viewSize.width)
        {
            mapSpritePosition.x -= (mapPosition.x - mapSize.width / 2);
        }
        
        if (mapSize.height > viewSize.height)
        {
            mapSpritePosition.y -= (mapPosition.y - mapSize.height / 2);
        }
        
        mapSprite->setPosition(mapSpritePosition);
    }
}
```

## 四、使用步骤

1. **准备地图文件**：
   - 将farmMap.png文件放在Resources目录下

2. **在主菜单中点击"new"按钮**：
   - 系统会通过MenuManager切换到游戏主场景

3. **控制视野移动**：
   - 使用W键向上移动视野
   - 使用S键向下移动视野
   - 使用A键向左移动视野
   - 使用D键向右移动视野

4. **注意事项**：
   - 确保farmMap.png文件存在且路径正确
   - 地图文件会自动适配视野大小，超出视野范围的部分会被裁剪
   - 视野移动会受到地图边界的限制，不会超出地图范围

## 五、扩展说明

如果需要添加新的场景切换功能，可以按照以下步骤进行：

1. **创建新的场景类**：
   ```cpp
   class NewScene : public cocos2d::Scene
   {
   public:
       static cocos2d::Scene* createScene();
       virtual bool init();
       CREATE_FUNC(NewScene);
   };
   ```

2. **在MenuManager中添加新的场景切换方法**：
   ```cpp
   // 在MenuManager.h中添加声明
   void goToNewScene();
   
   // 在MenuManager.cpp中添加实现
   void MenuManager::goToNewScene()
   {
       auto scene = NewScene::createScene();
       switchScene(scene);
   }
   ```

3. **在需要的地方调用**：
   ```cpp
   MenuManager::getInstance()->goToNewScene();
   ```
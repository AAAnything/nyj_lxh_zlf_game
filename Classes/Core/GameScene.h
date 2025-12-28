#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Inventory/Inventory.h"
#include "Inventory/ShippingBox.h"
#include "Inventory/InventoryUI.h" // 记得包含 UI 头文件

// 定义工具初始化的配置结构体
struct ToolDef {
    std::string id;
    std::string name;
    std::string icon;
    bool isSeed;
};

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // 处理键盘事件
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // 每帧更新
    void update(float delta);

    // 手动实现静态 create() 方法
    CREATE_FUNC(GameScene);

private:
    /**
     * @brief 初始化开局自带的工具和种子
     * 逻辑：根据预设的列表创建 Item，设置属性并放入 inventory
     */
    void initStartingItems();

    // --- 游戏数据 ---
    Inventory* _inventory;           // 背包数据模型指针
    InventoryUI* _inventoryUI;       // 背包界面指针

    // --- 地图相关 ---
    cocos2d::Sprite* mapSprite;      // 地图精灵
    cocos2d::TMXTiledMap* tileMap;   // 瓦片地图
    cocos2d::Size mapSize;           // 地图原始大小
    cocos2d::Size scaledMapSize;     // 缩放后的地图大小
    cocos2d::Size viewSize;          // 屏幕/视图大小
    cocos2d::Vec2 mapPosition;       // 地图当前坐标

    // --- 移动逻辑 ---
    float moveSpeed;                 // 玩家/相机移动速度
    bool keys[256];                  // 记录键盘按键状态的阵列
};

#endif // __GAME_SCENE_H__
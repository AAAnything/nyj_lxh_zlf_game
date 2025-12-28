#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include <vector>
#include <map>
#include <string>
#include <functional>

USING_NS_CC;

// 前置声明
class Inventory;
class Player;

// 游戏状态
enum class GameState {
    TITLE_SCREEN,
    PLAYING,
    PAUSED,
    MENU
};

class GameManager {
private:
    // 单例实例
    static GameManager* instance;

    // 游戏状态
    GameState gameState;
    bool isInitialized;

    // 玩家数据
    Player* player;
    Inventory* playerInventory;
    int money;

    // 游戏世界
    std::map<std::string, GameObject*> gameObjects;

    // 私有构造函数
    GameManager();
    ~GameManager();

public:
    // 单例访问
    static GameManager* getInstance();
    static void destroyInstance();

    // 初始化
    bool initialize();
    void shutdown();

    // 游戏状态管理
    GameState getGameState() const { return gameState; }
    void setGameState(GameState newState);
    bool isGamePaused() const { return gameState == GameState::PAUSED; }
    bool isGamePlaying() const { return gameState == GameState::PLAYING; }

    // 玩家管理
    Player* getPlayer() const { return player; }
    void setPlayer(Player* p);

    Inventory* getPlayerInventory() const { return playerInventory; }
    void setPlayerInventory(Inventory* inv);

    // 金钱管理
    int getMoney() const { return money; }
    void addMoney(int amount);
    bool spendMoney(int amount);
    void setMoney(int amount);

    // 游戏对象管理
    void registerGameObject(GameObject* obj);
    void unregisterGameObject(GameObject* obj);
    GameObject* getGameObjectById(const std::string& id) const;
    GameObject* findGameObjectByName(const std::string& name) const;

    // 回调函数
    typedef std::function<void(GameState, GameState)> GameStateChangeCallback;
    std::vector<GameStateChangeCallback> stateCallbacks;

    // 保存/加载
    std::string saveFileName;
};

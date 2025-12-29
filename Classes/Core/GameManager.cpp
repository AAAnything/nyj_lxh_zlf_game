// 文件名：GameManager.cpp
// 功能：  游戏管理器的实现文件，负责管理游戏的核心逻辑和状态。
// 作者：  Niu

#include "GameManager.h"
#include "Inventory/Inventory.h"
// #include "Player.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

USING_NS_CC;

// 初始化静态成员
GameManager* GameManager::instance = nullptr;

// 构造函数
GameManager::GameManager()
    : gameState(GameState::TITLE_SCREEN) // 初始状态为标题界面
    , isInitialized(false) // 未初始化
    , player(nullptr) // 玩家指针初始化为空
    , playerInventory(nullptr) // 玩家背包初始化为空
    , money(500) // 初始资金
    , saveFileName("save1.dat") { // 默认保存文件名
}

// 析构函数
GameManager::~GameManager() {
    shutdown();
}

// 获取单例
GameManager* GameManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) GameManager();
        CCASSERT(instance, "FATAL: Not enough memory for GameManager");
    }
    return instance;
}

void GameManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

// 初始化:新开始 / 读档
bool GameManager::initialize() {

    // 防止重复初始化
    if (isInitialized) {
        return true;
    }

    CCLOG("Initializing GameManager...");

    // 设置默认值
    gameState = GameState::TITLE_SCREEN;

    isInitialized = true;
    CCLOG("GameManager initialized successfully");

    return true;
}

// 关闭游戏
void GameManager::shutdown() {
    if (!isInitialized) {
        return;
    }

    CCLOG("Shutting down GameManager...");

    isInitialized = false;
    CCLOG("GameManager shut down");
}

// 游戏状态切换
void GameManager::setGameState(GameState newState) {
    if (gameState == newState) {
        return;
    }

    GameState oldState = gameState;
    gameState = newState;

    // 通知状态变化
    for (auto& callback : stateCallbacks) {
        callback(oldState, newState);
    }

    CCLOG("Game state changed from %d to %d", static_cast<int>(oldState), static_cast<int>(newState));
}

// 设置玩家
void GameManager::setPlayer(Player* p) {
    player = p;
}

// 设置玩家背包
void GameManager::setPlayerInventory(Inventory* inv) {
    playerInventory = inv;
}

// 金钱管理方法
void GameManager::addMoney(int amount) {
    if (amount <= 0) {
        return;
    }

    money += amount;
    CCLOG("Added %d money. Total: %d", amount, money);
}

bool GameManager::spendMoney(int amount) {
    if (amount <= 0 || money < amount) {
        return false;
    }

    money -= amount;
    CCLOG("Spent %d money. Remaining: %d", amount, money);
    return true;
}

void GameManager::setMoney(int amount) {
    money = amount;
    CCLOG("Set money to: %d", amount);
}

// 游戏对象管理
void GameManager::registerGameObject(GameObject* obj) {
    if (!obj) {
        return;
    }

    gameObjects[obj->getId()] = obj;
    CCLOG("Registered game object: %s", obj->getId().c_str());
}

void GameManager::unregisterGameObject(GameObject* obj) {
    if (!obj) {
        return;
    }

    auto it = gameObjects.find(obj->getId());
    if (it != gameObjects.end()) {
        gameObjects.erase(it);
        CCLOG("Unregistered game object: %s", obj->getId().c_str());
    }
}

GameObject* GameManager::getGameObjectById(const std::string& id) const {
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        return it->second;
    }
    return nullptr;
}

GameObject* GameManager::findGameObjectByName(const std::string& name) const {
    for (const auto& pair : gameObjects) {
        if (pair.second->getName() == name) {
            return pair.second;
        }
    }
    return nullptr;
}
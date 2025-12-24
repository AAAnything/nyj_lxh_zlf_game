// GameManager.h
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>

USING_NS_CC;

// 前置声明
class FarmlandTile;
class Animal;
class Crop;
class Item;
class Inventory;
class Player;
class NPC;

// 季节枚举
enum class Season {
    SPRING = 0,
    SUMMER = 1,
    AUTUMN = 2,
    WINTER = 3
};

// 时间结构
struct GameTime {
    int year;       // 年
    int season;     // 季节 (0-3)
    int day;        // 日 (1-30)
    int hour;       // 小时 (0-23)
    int minute;     // 分钟 (0-59)

    GameTime() : year(1), season(0), day(1), hour(6), minute(0) {}

    std::string toString() const;
    bool isDaytime() const;
    bool isNight() const;
};

// 游戏状态
enum class GameState {
    TITLE_SCREEN,
    PLAYING,
    PAUSED,
    DIALOGUE,
    MENU,
    GAME_OVER,
    SAVING,
    LOADING
};

class GameManager {
private:
    // 单例实例
    static GameManager* instance;

    // 游戏时间
    GameTime currentTime;
    float timeScale;            // 时间流逝速度
    float realSecondsPerGameMinute; // 现实秒数对应游戏分钟数
    float accumulatedTime;      // 累积时间

    // 游戏状态
    GameState gameState;
    bool isInitialized;
    bool isGameOver;

    // 玩家数据
    Player* player;
    Inventory* playerInventory;
    int money;
    int playerEnergy;
    int maxPlayerEnergy;

    // 游戏世界
    std::vector<FarmlandTile*> farmTiles;
    std::vector<Animal*> animals;
    std::vector<NPC*> npcs;
    std::map<std::string, GameObject*> gameObjects;

    // 技能系统
    std::map<std::string, int> skills; // 技能名 -> 等级
    std::map<std::string, int> skillExp; // 技能名 -> 经验值

    // 任务系统
    std::vector<class Quest*> activeQuests;
    std::vector<class Quest*> completedQuests;

    // 节日和事件
    std::map<std::string, bool> festivalFlags;
    std::vector<class GameEvent*> scheduledEvents;

    // 天气系统
    std::string currentWeather;
    std::string tomorrowWeather;
    float temperature;

    // 保存/加载
    std::string saveFileName;

    // 回调函数
    typedef std::function<void(const GameTime&)> TimeChangeCallback;
    typedef std::function<void(Season)> SeasonChangeCallback;
    typedef std::function<void(GameState, GameState)> GameStateChangeCallback;

    std::vector<TimeChangeCallback> timeCallbacks;
    std::vector<SeasonChangeCallback> seasonCallbacks;
    std::vector<GameStateChangeCallback> stateCallbacks;

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

    // 时间管理
    void update(float deltaTime);
    void advanceTime(int minutes);
    void advanceToNextDay();
    void changeSeason(Season newSeason);

    // 时间获取
    GameTime getCurrentTime() const { return currentTime; }
    Season getCurrentSeason() const { return static_cast<Season>(currentTime.season); }
    int getCurrentDay() const { return currentTime.day; }
    int getCurrentYear() const { return currentTime.year; }
    std::string getTimeString() const;
    std::string getDateString() const;

    // 时间设置
    void setTimeScale(float scale);
    float getTimeScale() const { return timeScale; }
    void setGameSpeed(float speed); // 0.5x, 1x, 2x等

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

    int getMoney() const { return money; }
    void addMoney(int amount);
    bool spendMoney(int amount);
    void setMoney(int amount);

    int getPlayerEnergy() const { return playerEnergy; }
    int getMaxPlayerEnergy() const { return maxPlayerEnergy; }
    void consumeEnergy(int amount);
    void restoreEnergy(int amount);
    void setMaxEnergy(int max);

    // 农场管理
    void addFarmTile(FarmlandTile* tile);
    void removeFarmTile(FarmlandTile* tile);
    std::vector<FarmlandTile*> getFarmTiles() const { return farmTiles; }
    FarmlandTile* getFarmTileAtPosition(const Vec2& pos) const;

    void updateFarmTiles(); // 每日更新

    // 动物管理
    void addAnimal(Animal* animal);
    void removeAnimal(Animal* animal);
    std::vector<Animal*> getAnimals() const { return animals; }
    void updateAnimals(); // 每日更新

    // NPC管理
    void addNPC(NPC* npc);
    NPC* getNPCByName(const std::string& name) const;
    std::vector<NPC*> getNPCs() const { return npcs; }

    // 游戏对象管理
    void registerGameObject(GameObject* obj);
    void unregisterGameObject(GameObject* obj);
    GameObject* getGameObjectById(const std::string& id) const;
    GameObject* findGameObjectByName(const std::string& name) const;

    // 技能系统
    void addSkillExperience(const std::string& skill, int exp);
    int getSkillLevel(const std::string& skill) const;
    int getSkillExperience(const std::string& skill) const;
    void setSkillLevel(const std::string& skill, int level);
    bool hasSkill(const std::string& skill) const;

    // 任务系统
    void addQuest(class Quest* quest);
    void completeQuest(const std::string& questId);
    bool isQuestActive(const std::string& questId) const;
    bool isQuestCompleted(const std::string& questId) const;

    // 节日和事件
    void scheduleEvent(class GameEvent* event);
    void triggerFestival(const std::string& festivalName);
    bool isFestivalActive(const std::string& festivalName) const;
    void endFestival(const std::string& festivalName);

    // 天气系统
    std::string getCurrentWeather() const { return currentWeather; }
    void setWeather(const std::string& weather);
    void generateTomorrowWeather();
    float getTemperature() const { return temperature; }
    void setTemperature(float temp);

    // 经济系统
    int calculateSellPrice(Item* item) const;
    void sellItem(Item* item, int quantity = 1);
    void processShippingBox(); // 处理出货箱

    // 保存/加载
    bool saveGame(const std::string& filename = "");
    bool loadGame(const std::string& filename = "");
    void autoSave();
    std::vector<std::string> getSaveFiles() const;

    // 事件回调注册
    void registerTimeCallback(const TimeChangeCallback& callback);
    void registerSeasonCallback(const SeasonChangeCallback& callback);
    void registerStateCallback(const GameStateChangeCallback& callback);

    // 工具方法
    static std::string seasonToString(Season season);
    static Season stringToSeason(const std::string& seasonStr);
    static std::string gameStateToString(GameState state);

    // 调试功能
    void debugPrintStatus() const;
    void cheatAddMoney(int amount);
    void cheatSetTime(int hour, int minute);

private:
    // 私有方法
    void notifyTimeChanged();
    void notifySeasonChanged(Season newSeason);
    void notifyStateChanged(GameState oldState, GameState newState);

    void processDailyEvents();
    void processHourlyEvents();
    void updateWeather();

    void initializeDefaultSkills();
    void initializeDefaultFestivals();

    void clearAllData();
    void resetGame();

    // 序列化辅助方法
    ValueMap serializeGameData() const;
    bool deserializeGameData(const ValueMap& data);

    // 时间计算辅助
    void checkForNewDay();
    void checkForNewSeason();

    // 经济辅助
    int calculateSkillPriceBonus(const std::string& relevantSkill) const;
};
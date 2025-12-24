// 文件名：GameManager.cpp
// 功能：  游戏管理器的实现文件，负责管理游戏的核心逻辑和状态。
// 作者：  Niu

#include "GameManager.h"
#include "Farmland/FarmlandTile.h"
#include "Animal/Animal.h"
#include "Crop/Crop.h"
#include "Items/Inventory.h"
#include "Player/Player.h"
#include "NPC/NPC.h"
#include "Quests/Quest.h"
#include "Events/GameEvent.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

USING_NS_CC;

// 初始化静态成员
GameManager* GameManager::instance = nullptr;

// GameTime方法实现
std::string GameTime::toString() const {
    std::stringstream ss;
    ss << "Year " << year << ", ";

    std::string seasonStr;

    switch (season) {
        case 0: seasonStr = "Spring"; break;
        case 1: seasonStr = "Summer"; break;
        case 2: seasonStr = "Autumn"; break;
        case 3: seasonStr = "Winter"; break;
        default: seasonStr = "Unknown";
    }

    ss << seasonStr << " Day " << day << ", ";
    ss << std::setw(2) << std::setfill('0') << hour << ":";
    ss << std::setw(2) << std::setfill('0') << minute;

    return ss.str();
}

bool GameTime::isDaytime() const {
    return hour >= 6 && hour < 22;
}

bool GameTime::isNight() const {
    return !isDaytime();
}

// 构造函数
GameManager::GameManager()
    : currentTime()
    , timeScale(1.0f) // 默认1倍速，后期调试可改为2.0f等
    , realSecondsPerGameMinute(0.5f) // 现实0.5秒 = 游戏1分钟
    , accumulatedTime(0.0f) // 时间累积
    , gameState(GameState::TITLE_SCREEN) // 初始状态为标题界面
    , isInitialized(false) // 未初始化
    , isGameOver(false) // 游戏未结束
    , player(nullptr) // 玩家指针初始化为空
    , playerInventory(nullptr) // 玩家背包初始化为空
    , money(500) // 初始资金
    , playerEnergy(100) // 初始体力
    , maxPlayerEnergy(100) // 最大体力
    , currentWeather("Sunny") // 初始天气为晴天
    , tomorrowWeather("Sunny") // 明天天气
    , temperature(20.0f) // 初始温度
    , saveFileName("save1.dat") { // 默认保存文件名

    initializeDefaultSkills(); // 初始化默认技能
    initializeDefaultFestivals(); // 初始化默认节日
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

    // 初始化随机种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 设置默认值
    gameState = GameState::TITLE_SCREEN;
    isGameOver = false;

    // 生成明天天气
    generateTomorrowWeather();

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

    // 保存游戏
    autoSave();

    // 清理内存
    clearAllData();

    isInitialized = false;
    CCLOG("GameManager shut down");
}



// 更新
void GameManager::update(float deltaTime) {

    // 确保仅在游戏进行中更新
    if (gameState != GameState::PLAYING || isGameOver) {
        return;
    }

    // 更新时间
    accumulatedTime += deltaTime * timeScale;
    float gameMinutesPassed = accumulatedTime / realSecondsPerGameMinute;

    // 推进游戏时间
    if (gameMinutesPassed >= 1.0f) {
        int minutes = static_cast<int>(gameMinutesPassed);
        advanceTime(minutes);
        accumulatedTime -= minutes * realSecondsPerGameMinute;
    }

    // 检查事件
    for (auto it = scheduledEvents.begin(); it != scheduledEvents.end();) {
        GameEvent* event = *it;
        if (event->shouldTrigger(currentTime)) {
            event->trigger();
            delete event;
            it = scheduledEvents.erase(it);
        }
        else {
            ++it;
        }
    }
}

// 直接推进时间的函数
void GameManager::advanceTime(int minutes) {
    if (minutes <= 0) return;

    int oldHour = currentTime.hour;
    int oldDay = currentTime.day;
    Season oldSeason = getCurrentSeason();

    // 增加时间
    currentTime.minute += minutes;
    while (currentTime.minute >= 60) {
        currentTime.minute -= 60;
        currentTime.hour++;

        // 每小时事件
        processHourlyEvents();
    }

    while (currentTime.hour >= 24) {
        currentTime.hour -= 24;
        currentTime.day++;

        // 每天事件
        processDailyEvents();
    }

    while (currentTime.day > 30) {
        currentTime.day -= 30;
        currentTime.season++;
    }

    while (currentTime.season > 3) {
        currentTime.season -= 4;
        currentTime.year++;
    }

    // 通知时间变化
    if (oldHour != currentTime.hour) {
        notifyTimeChanged();
    }

    // 检查新的一天
    if (oldDay != currentTime.day) {
        checkForNewDay();
    }

    // 检查新季节
    if (oldSeason != getCurrentSeason()) {
        checkForNewSeason();
    }

    CCLOG("Time advanced to: %s", getTimeString().c_str());
}

// 推进到下一天：睡觉或日终
void GameManager::advanceToNextDay() {
    int hoursToAdd = 24 - currentTime.hour;
    advanceTime(hoursToAdd * 60);
}

// 切换季节为所给季节（用于作弊或特殊事件）
void GameManager::changeSeason(Season newSeason) {
    Season oldSeason = getCurrentSeason();
    if (oldSeason == newSeason) return;

    currentTime.season = static_cast<int>(newSeason);

    // 重置天数（可选）
    currentTime.day = 1;

    notifySeasonChanged(newSeason);

    CCLOG("Season changed to: %s", seasonToString(newSeason).c_str());
}


// 获取时间字符串（纯UI,如 "06:30"）
std::string GameManager::getTimeString() const {
    return currentTime.toString();
}

// 获取日期字符串（纯UI,如 "Year 1, Spring Day 1"）
std::string GameManager::getDateString() const {
    std::stringstream ss;
    ss << "Year " << currentTime.year << ", ";
    ss << seasonToString(getCurrentSeason()) << " Day " << currentTime.day;
    return ss.str();
}

// 设置时间倍速
void GameManager::setTimeScale(float scale) {
    timeScale = std::max(0.0f, std::min(10.0f, scale)); // 限制在0-10倍
}

// 设置游戏速度
void GameManager::setGameSpeed(float speed) {
    // speed: 0.5, 1.0, 2.0 等
    setTimeScale(speed);
}
// 两者的区别是：setTimeScale直接设置时间流逝的倍速，
// 而setGameSpeed是一个更通用的接口，可以根据需要扩展更多功能。

// 世界开关
void GameManager::setGameState(GameState newState) {
    if (gameState == newState) return;

    GameState oldState = gameState;
    gameState = newState;

    notifyStateChanged(oldState, newState);

    CCLOG("Game state changed: %s -> %s",
        gameStateToString(oldState).c_str(),
        gameStateToString(newState).c_str());
}

// 玩家管理
void GameManager::setPlayer(Player* p) {
    if (player) {
        // 注销旧的玩家对象
        unregisterGameObject(player);
    }

    player = p;
    if (player) {
        registerGameObject(player);
    }
}

void GameManager::setPlayerInventory(Inventory* inv) {
    playerInventory = inv;
}

// 添加金钱
void GameManager::addMoney(int amount) {
    if (amount <= 0) return;

    money += amount;
    CCLOG("Money added: %d. Total: %d", amount, money);
}

// 花费金钱
bool GameManager::spendMoney(int amount) {
    if (amount <= 0) return true;

    if (money >= amount) {
        money -= amount;
        CCLOG("Money spent: %d. Remaining: %d", amount, money);
        return true;
    }

    CCLOG("Not enough money! Need: %d, Have: %d", amount, money);
    return false;
}

// 设置金钱
void GameManager::setMoney(int amount) {
    money = std::max(0, amount);
    CCLOG("Money set to: %d", money);
}

// 消耗体力
void GameManager::consumeEnergy(int amount) {
    if (amount <= 0) return;

    playerEnergy = std::max(0, playerEnergy - amount);
    CCLOG("Energy consumed: %d. Remaining: %d/%d", amount, playerEnergy, maxPlayerEnergy);

    if (playerEnergy <= 0) {
        CCLOG("Player is exhausted!");
        // 可以触发疲劳事件
    }
}

// 恢复体力
void GameManager::restoreEnergy(int amount) {
    if (amount <= 0) return;

    playerEnergy = std::min(maxPlayerEnergy, playerEnergy + amount);
    CCLOG("Energy restored: %d. Now: %d/%d", amount, playerEnergy, maxPlayerEnergy);
}

// 设置最大体力
void GameManager::setMaxEnergy(int max) {
    maxPlayerEnergy = std::max(1, max);
    if (playerEnergy > maxPlayerEnergy) {
        playerEnergy = maxPlayerEnergy;
    }
}

// 农场管理
void GameManager::addFarmTile(FarmlandTile* tile) {
    if (!tile) return;

    auto it = std::find(farmTiles.begin(), farmTiles.end(), tile);
    if (it == farmTiles.end()) {
        farmTiles.push_back(tile);
        registerGameObject(tile);
        CCLOG("Farm tile added. Total tiles: %d", (int)farmTiles.size());
    }
}

void GameManager::removeFarmTile(FarmlandTile* tile) {
    if (!tile) return;

    auto it = std::find(farmTiles.begin(), farmTiles.end(), tile);
    if (it != farmTiles.end()) {
        farmTiles.erase(it);
        unregisterGameObject(tile);
        CCLOG("Farm tile removed. Total tiles: %d", (int)farmTiles.size());
    }
}

FarmlandTile* GameManager::getFarmTileAtPosition(const Vec2& pos) const {
    for (auto tile : farmTiles) {
        if (tile->containsPoint(pos)) {
            return tile;
        }
    }
    return nullptr;
}

// 更新农场地块（每日）
void GameManager::updateFarmTiles() {
    CCLOG("Updating farm tiles...");

    for (auto tile : farmTiles) {
        tile->onDayPassed();
    }

    CCLOG("Farm tiles updated");
}

// 动物管理
void GameManager::addAnimal(Animal* animal) {
    if (!animal) return;

    auto it = std::find(animals.begin(), animals.end(), animal);
    if (it == animals.end()) {
        animals.push_back(animal);
        registerGameObject(animal);
        CCLOG("Animal added: %s. Total animals: %d",
            animal->getName().c_str(), (int)animals.size());
    }
}

void GameManager::removeAnimal(Animal* animal) {
    if (!animal) return;

    auto it = std::find(animals.begin(), animals.end(), animal);
    if (it != animals.end()) {
        animals.erase(it);
        unregisterGameObject(animal);
        CCLOG("Animal removed: %s. Total animals: %d",
            animal->getName().c_str(), (int)animals.size());
    }
}

// 更新动物（每日）
void GameManager::updateAnimals() {
    CCLOG("Updating animals...");

    for (auto animal : animals) {
        animal->update();
    }

    CCLOG("Animals updated");
}

// NPC管理
void GameManager::addNPC(NPC* npc) {
    if (!npc) return;

    auto it = std::find(npcs.begin(), npcs.end(), npc);
    if (it == npcs.end()) {
        npcs.push_back(npc);
        registerGameObject(npc);
        CCLOG("NPC added: %s. Total NPCs: %d",
            npc->getName().c_str(), (int)npcs.size());
    }
}

NPC* GameManager::getNPCByName(const std::string& name) const {
    for (auto npc : npcs) {
        if (npc->getName() == name) {
            return npc;
        }
    }
    return nullptr;
}

// 游戏对象管理
void GameManager::registerGameObject(GameObject* obj) {
    if (!obj) return;

    std::string id = obj->getId();
    if (gameObjects.find(id) == gameObjects.end()) {
        gameObjects[id] = obj;
        CCLOG("GameObject registered: %s (%s)",
            obj->getName().c_str(), id.c_str());
    }
}

void GameManager::unregisterGameObject(GameObject* obj) {
    if (!obj) return;

    std::string id = obj->getId();
    auto it = gameObjects.find(id);
    if (it != gameObjects.end()) {
        gameObjects.erase(it);
        CCLOG("GameObject unregistered: %s (%s)",
            obj->getName().c_str(), id.c_str());
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
    for (auto& pair : gameObjects) {
        if (pair.second->getName() == name) {
            return pair.second;
        }
    }
    return nullptr;
}

// 技能系统
void GameManager::addSkillExperience(const std::string& skill, int exp) {
    if (exp <= 0) return;

    skillExp[skill] += exp;

    // 检查升级
    int currentLevel = getSkillLevel(skill);
    int expForNextLevel = currentLevel * 100;

    if (skillExp[skill] >= expForNextLevel) {
        skills[skill] = currentLevel + 1;
        skillExp[skill] -= expForNextLevel;

        CCLOG("Skill %s leveled up to %d!", skill.c_str(), skills[skill]);
    }
}

int GameManager::getSkillLevel(const std::string& skill) const {
    auto it = skills.find(skill);
    if (it != skills.end()) {
        return it->second;
    }
    return 0;
}

int GameManager::getSkillExperience(const std::string& skill) const {
    auto it = skillExp.find(skill);
    if (it != skillExp.end()) {
        return it->second;
    }
    return 0;
}

void GameManager::setSkillLevel(const std::string& skill, int level) {
    if (level < 0) level = 0;
    skills[skill] = level;
    skillExp[skill] = 0;
}

bool GameManager::hasSkill(const std::string& skill) const {
    return skills.find(skill) != skills.end();
}

// 任务系统
void GameManager::addQuest(Quest* quest) {
    if (!quest) return;

    // 检查是否已经存在
    for (auto q : activeQuests) {
        if (q->getId() == quest->getId()) {
            return;
        }
    }

    activeQuests.push_back(quest);
    CCLOG("Quest added: %s", quest->getTitle().c_str());
}

void GameManager::completeQuest(const std::string& questId) {
    for (auto it = activeQuests.begin(); it != activeQuests.end(); ++it) {
        if ((*it)->getId() == questId) {
            Quest* quest = *it;
            quest->complete();
            completedQuests.push_back(quest);
            activeQuests.erase(it);

            CCLOG("Quest completed: %s", quest->getTitle().c_str());

            // 给予奖励
            quest->giveRewards();

            return;
        }
    }
}

bool GameManager::isQuestActive(const std::string& questId) const {
    for (auto quest : activeQuests) {
        if (quest->getId() == questId) {
            return true;
        }
    }
    return false;
}

bool GameManager::isQuestCompleted(const std::string& questId) const {
    for (auto quest : completedQuests) {
        if (quest->getId() == questId) {
            return true;
        }
    }
    return false;
}

// 节日和事件
void GameManager::scheduleEvent(GameEvent* event) {
    if (!event) return;

    scheduledEvents.push_back(event);
    CCLOG("Event scheduled: %s", event->getName().c_str());
}

void GameManager::triggerFestival(const std::string& festivalName) {
    festivalFlags[festivalName] = true;
    CCLOG("Festival triggered: %s", festivalName.c_str());

    // 可以在这里添加节日特定的逻辑
}

bool GameManager::isFestivalActive(const std::string& festivalName) const {
    auto it = festivalFlags.find(festivalName);
    if (it != festivalFlags.end()) {
        return it->second;
    }
    return false;
}

void GameManager::endFestival(const std::string& festivalName) {
    festivalFlags[festivalName] = false;
    CCLOG("Festival ended: %s", festivalName.c_str());
}

// 天气系统
void GameManager::setWeather(const std::string& weather) {
    currentWeather = weather;
    CCLOG("Weather changed to: %s", weather.c_str());

    // 根据天气调整温度
    if (weather == "Sunny") {
        temperature = 25.0f;
    }
    else if (weather == "Rainy") {
        temperature = 18.0f;
    }
    else if (weather == "Snowy") {
        temperature = -5.0f;
    }
    else if (weather == "Stormy") {
        temperature = 15.0f;
    }
}

void GameManager::generateTomorrowWeather() {
    // 简单的随机天气生成
    const std::vector<std::string> possibleWeathers = {
        "Sunny", "Sunny", "Sunny", "Cloudy", "Rainy", "Stormy"
    };

    // 根据季节调整概率
    int randomIndex = rand() % possibleWeathers.size();
    tomorrowWeather = possibleWeathers[randomIndex];

    CCLOG("Tomorrow's weather will be: %s", tomorrowWeather.c_str());
}

// 经济系统
int GameManager::calculateSellPrice(Item* item) const {
    if (!item) return 0;

    int basePrice = item->getSellPrice();
    float multiplier = 1.0f;

    // 技能加成
    if (item->getType() == GameObject::Type::CROP) {
        multiplier += getSkillLevel("Farming") * 0.05f; // 每级农业技能+5%
    }

    // 品质加成（如果有）
    // multiplier += item->getQuality() * 0.1f;

    // 节日加成（如果有）
    if (isFestivalActive("HarvestFestival")) {
        multiplier += 0.2f; // 丰收节期间+20%
    }

    return static_cast<int>(basePrice * multiplier);
}

void GameManager::sellItem(Item* item, int quantity) {
    if (!item || quantity <= 0) return;

    int price = calculateSellPrice(item) * quantity;
    addMoney(price);

    CCLOG("Sold %d x %s for %d gold", quantity, item->getName().c_str(), price);
}

void GameManager::processShippingBox() {
    // 处理出货箱中的物品
    // 这里需要与Inventory系统集成
    CCLOG("Processing shipping box...");

    // 在实际实现中，这里会遍历出货箱中的所有物品并出售
}

// 保存游戏
bool GameManager::saveGame(const std::string& filename) {
    std::string saveFile = filename.empty() ? saveFileName : filename;

    CCLOG("Saving game to: %s", saveFile.c_str());

    ValueMap gameData = serializeGameData();

    // 使用UserDefault保存（Cocos2d-x的简单保存方式）
    auto userDefault = UserDefault::getInstance();

    // 保存基本数据
    userDefault->setIntegerForKey("year", currentTime.year);
    userDefault->setIntegerForKey("season", currentTime.season);
    userDefault->setIntegerForKey("day", currentTime.day);
    userDefault->setIntegerForKey("hour", currentTime.hour);
    userDefault->setIntegerForKey("minute", currentTime.minute);

    userDefault->setIntegerForKey("money", money);
    userDefault->setIntegerForKey("energy", playerEnergy);
    userDefault->setIntegerForKey("maxEnergy", maxPlayerEnergy);

    // 保存技能
    for (auto& skill : skills) {
        std::string key = "skill_" + skill.first;
        userDefault->setIntegerForKey(key.c_str(), skill.second);
    }

    userDefault->flush();

    CCLOG("Game saved successfully");
    return true;
}

// 加载游戏
bool GameManager::loadGame(const std::string& filename) {
    std::string loadFile = filename.empty() ? saveFileName : filename;

    CCLOG("Loading game from: %s", loadFile.c_str());

    auto userDefault = UserDefault::getInstance();

    // 加载基本数据
    currentTime.year = userDefault->getIntegerForKey("year", 1);
    currentTime.season = userDefault->getIntegerForKey("season", 0);
    currentTime.day = userDefault->getIntegerForKey("day", 1);
    currentTime.hour = userDefault->getIntegerForKey("hour", 6);
    currentTime.minute = userDefault->getIntegerForKey("minute", 0);

    money = userDefault->getIntegerForKey("money", 500);
    playerEnergy = userDefault->getIntegerForKey("energy", 100);
    maxPlayerEnergy = userDefault->getIntegerForKey("maxEnergy", 100);

    CCLOG("Game loaded successfully. Current time: %s", getTimeString().c_str());
    return true;
}

// 自动保存
void GameManager::autoSave() {
    CCLOG("Auto-saving game...");
    saveGame("autosave.dat");
}

// 获取保存文件列表
std::vector<std::string> GameManager::getSaveFiles() const {
    std::vector<std::string> saveFiles;

    // 在实际实现中，这里会扫描保存目录
    saveFiles.push_back("save1.dat");
    saveFiles.push_back("save2.dat");
    saveFiles.push_back("save3.dat");
    saveFiles.push_back("autosave.dat");

    return saveFiles;
}

// 注册回调
void GameManager::registerTimeCallback(const TimeChangeCallback& callback) {
    timeCallbacks.push_back(callback);
}

void GameManager::registerSeasonCallback(const SeasonChangeCallback& callback) {
    seasonCallbacks.push_back(callback);
}

void GameManager::registerStateCallback(const GameStateChangeCallback& callback) {
    stateCallbacks.push_back(callback);
}

// 季节转字符串
std::string GameManager::seasonToString(Season season) {
    switch (season) {
        case Season::SPRING: return "Spring";
        case Season::SUMMER: return "Summer";
        case Season::AUTUMN: return "Autumn";
        case Season::WINTER: return "Winter";
        default: return "Unknown";
    }
}

// 字符串转季节
Season GameManager::stringToSeason(const std::string& seasonStr) {
    if (seasonStr == "Spring") return Season::SPRING;
    if (seasonStr == "Summer") return Season::SUMMER;
    if (seasonStr == "Autumn") return Season::AUTUMN;
    if (seasonStr == "Winter") return Season::WINTER;
    return Season::SPRING; // 默认
}

// 游戏状态转字符串
std::string GameManager::gameStateToString(GameState state) {
    switch (state) {
        case GameState::TITLE_SCREEN: return "Title Screen";
        case GameState::PLAYING: return "Playing";
        case GameState::PAUSED: return "Paused";
        case GameState::DIALOGUE: return "Dialogue";
        case GameState::MENU: return "Menu";
        case GameState::GAME_OVER: return "Game Over";
        case GameState::SAVING: return "Saving";
        case GameState::LOADING: return "Loading";
        default: return "Unknown";
    }
}

// 调试功能
void GameManager::debugPrintStatus() const {
    CCLOG("=== GAME STATUS ===");
    CCLOG("Time: %s", getTimeString().c_str());
    CCLOG("Money: %d", money);
    CCLOG("Energy: %d/%d", playerEnergy, maxPlayerEnergy);
    CCLOG("Farm Tiles: %d", (int)farmTiles.size());
    CCLOG("Animals: %d", (int)animals.size());
    CCLOG("NPCs: %d", (int)npcs.size());
    CCLOG("Active Quests: %d", (int)activeQuests.size());
    CCLOG("===================");
}

void GameManager::cheatAddMoney(int amount) {
    addMoney(amount);
    CCLOG("CHEAT: Added %d money", amount);
}

void GameManager::cheatSetTime(int hour, int minute) {
    currentTime.hour = hour % 24;
    currentTime.minute = minute % 60;
    CCLOG("CHEAT: Time set to %02d:%02d", hour, minute);
}

// 私有方法实现
void GameManager::notifyTimeChanged() {
    for (auto& callback : timeCallbacks) {
        callback(currentTime);
    }
}

void GameManager::notifySeasonChanged(Season newSeason) {
    for (auto& callback : seasonCallbacks) {
        callback(newSeason);
    }
}

void GameManager::notifyStateChanged(GameState oldState, GameState newState) {
    for (auto& callback : stateCallbacks) {
        callback(oldState, newState);
    }
}

// 处理每日事件
void GameManager::processDailyEvents() {
    CCLOG("Processing daily events...");

    // 更新农场
    updateFarmTiles();

    // 更新动物
    updateAnimals();

    // 恢复玩家体力
    restoreEnergy(maxPlayerEnergy);

    // 处理出货箱
    processShippingBox();

    // 更新天气
    currentWeather = tomorrowWeather;
    generateTomorrowWeather();

    // 检查节日
    checkForFestivals();

    CCLOG("Daily events processed");
}

// 处理每小时事件
void GameManager::processHourlyEvents() {
    // 每小时恢复少量体力
    if (playerEnergy < maxPlayerEnergy) {
        restoreEnergy(5);
    }

    // 可以在这里添加其他每小时逻辑
}

// 更新天气
void GameManager::updateWeather() {
    // 简单的天气变化逻辑
    int random = rand() % 100;

    if (random < 5) { // 5% chance to change weather
        std::vector<std::string> weathers = { "Sunny", "Cloudy", "Rainy", "Stormy" };
        int index = rand() % weathers.size();
        setWeather(weathers[index]);
    }
}

// 初始化默认技能
void GameManager::initializeDefaultSkills() {
    skills["Farming"] = 1;
    skills["Mining"] = 1;
    skills["Fishing"] = 1;
    skills["Foraging"] = 1;
    skills["Combat"] = 1;

    skillExp["Farming"] = 0;
    skillExp["Mining"] = 0;
    skillExp["Fishing"] = 0;
    skillExp["Foraging"] = 0;
    skillExp["Combat"] = 0;
}

// 初始化默认节日
void GameManager::initializeDefaultFestivals() {
    festivalFlags["SpringFestival"] = false;
    festivalFlags["HarvestFestival"] = false;
    festivalFlags["WinterStar"] = false;
}

// 清空所有数据
void GameManager::clearAllData() {
    // 清理农场地块
    for (auto tile : farmTiles) {
        delete tile;
    }
    farmTiles.clear();

    // 清理动物
    for (auto animal : animals) {
        delete animal;
    }
    animals.clear();

    // 清理NPC
    for (auto npc : npcs) {
        delete npc;
    }
    npcs.clear();

    // 清理任务
    for (auto quest : activeQuests) {
        delete quest;
    }
    activeQuests.clear();

    for (auto quest : completedQuests) {
        delete quest;
    }
    completedQuests.clear();

    // 清理事件
    for (auto event : scheduledEvents) {
        delete event;
    }
    scheduledEvents.clear();

    // 清空游戏对象映射（不删除，因为它们可能已被删除）
    gameObjects.clear();

    // 重置玩家引用
    player = nullptr;
    playerInventory = nullptr;
}

// 重置游戏
void GameManager::resetGame() {
    CCLOG("Resetting game...");

    clearAllData();

    // 重置时间
    currentTime = GameTime();

    // 重置玩家数据
    money = 500;
    playerEnergy = 100;
    maxPlayerEnergy = 100;

    // 重置技能
    initializeDefaultSkills();

    // 重置节日
    initializeDefaultFestivals();

    // 重置天气
    currentWeather = "Sunny";
    generateTomorrowWeather();

    // 重置状态
    gameState = GameState::TITLE_SCREEN;
    isGameOver = false;

    CCLOG("Game reset complete");
}

// 序列化游戏数据
ValueMap GameManager::serializeGameData() const {
    ValueMap data;

    // 时间数据
    ValueMap timeData;
    timeData["year"] = currentTime.year;
    timeData["season"] = currentTime.season;
    timeData["day"] = currentTime.day;
    timeData["hour"] = currentTime.hour;
    timeData["minute"] = currentTime.minute;
    data["time"] = timeData;

    // 玩家数据
    data["money"] = money;
    data["energy"] = playerEnergy;
    data["maxEnergy"] = maxPlayerEnergy;

    // 技能数据
    ValueMap skillData;
    for (auto& skill : skills) {
        skillData[skill.first] = skill.second;
    }
    data["skills"] = skillData;

    // 技能经验数据
    ValueMap expData;
    for (auto& exp : skillExp) {
        expData[exp.first] = exp.second;
    }
    data["skillExperience"] = expData;

    // 节日数据
    ValueMap festivalData;
    for (auto& festival : festivalFlags) {
        festivalData[festival.first] = festival.second;
    }
    data["festivals"] = festivalData;

    return data;
}

// 反序列化游戏数据
bool GameManager::deserializeGameData(const ValueMap& data) {
    // 时间数据
    if (data.find("time") != data.end()) {
        ValueMap timeData = data.at("time").asValueMap();
        currentTime.year = timeData["year"].asInt();
        currentTime.season = timeData["season"].asInt();
        currentTime.day = timeData["day"].asInt();
        currentTime.hour = timeData["hour"].asInt();
        currentTime.minute = timeData["minute"].asInt();
    }

    // 玩家数据
    if (data.find("money") != data.end()) {
        money = data.at("money").asInt();
    }

    if (data.find("energy") != data.end()) {
        playerEnergy = data.at("energy").asInt();
    }

    if (data.find("maxEnergy") != data.end()) {
        maxPlayerEnergy = data.at("maxEnergy").asInt();
    }

    // 技能数据
    if (data.find("skills") != data.end()) {
        ValueMap skillData = data.at("skills").asValueMap();
        for (auto& pair : skillData) {
            skills[pair.first] = pair.second.asInt();
        }
    }

    // 技能经验数据
    if (data.find("skillExperience") != data.end()) {
        ValueMap expData = data.at("skillExperience").asValueMap();
        for (auto& pair : expData) {
            skillExp[pair.first] = pair.second.asInt();
        }
    }

    // 节日数据
    if (data.find("festivals") != data.end()) {
        ValueMap festivalData = data.at("festivals").asValueMap();
        for (auto& pair : festivalData) {
            festivalFlags[pair.first] = pair.second.asBool();
        }
    }

    return true;
}

// 检查新的一天
void GameManager::checkForNewDay() {
    CCLOG("New day: %s", getDateString().c_str());

    // 每日自动保存
    if (currentTime.day % 5 == 0) { // 每5天自动保存一次
        autoSave();
    }
}

// 检查新季节
void GameManager::checkForNewSeason() {
    CCLOG("New season: %s", seasonToString(getCurrentSeason()).c_str());

    // 季节变化时重置一些东西
    generateTomorrowWeather();

    // 检查季节特定的节日
    Season current = getCurrentSeason();
    if (current == Season::SPRING && currentTime.day == 15) {
        triggerFestival("SpringFestival");
    }
    else if (current == Season::AUTUMN && currentTime.day == 25) {
        triggerFestival("HarvestFestival");
    }
    else if (current == Season::WINTER && currentTime.day == 25) {
        triggerFestival("WinterStar");
    }
}

// 检查节日
void GameManager::checkForFestivals() {
    // 在实际实现中，这里会根据日期检查是否有节日
    // 并触发相应的节日事件
}

// 计算技能价格加成
int GameManager::calculateSkillPriceBonus(const std::string& relevantSkill) const {
    int level = getSkillLevel(relevantSkill);
    return level * 5; // 每级技能+5%价格
}
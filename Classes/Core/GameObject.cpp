// 文件名：GameObject.cpp
// 功能：  游戏对象的实现文件，定义了游戏中各种对象的基础行为和属性。
// 作者：  Niu

#include "GameObject.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

USING_NS_CC;

// 构造函数
GameObject::GameObject()
    : id("")
    , name("Unnamed")
    , description("")
    , objectType(Type::UNDEFINED)
    , isActive(true)
    , isInteractable(true)
    , position(Vec2::ZERO)
    , size(Size(32, 32))
    , rotation(0.0f)
    , scale(1.0f)
    , level(1)
    , experience(0)
    , maxExperience(100)
    , durability(100)
    , maxDurability(100)
    , objectSprite(nullptr)
    , currentAction(nullptr) {

    generateId();
}

GameObject::GameObject(const std::string& objName, Type type)
    : id("")
    , name(objName)
    , description("")
    , objectType(type)
    , isActive(true)
    , isInteractable(true)
    , position(Vec2::ZERO)
    , size(Size(32, 32))
    , rotation(0.0f)
    , scale(1.0f)
    , level(1)
    , experience(0)
    , maxExperience(100)
    , durability(100)
    , maxDurability(100)
    , objectSprite(nullptr)
    , currentAction(nullptr) {

    generateId();
}

GameObject::~GameObject() {
    if (currentAction) {
        this->stopAction(currentAction);
        currentAction = nullptr;
    }

    if (objectSprite) {
        objectSprite->removeFromParent();
        objectSprite = nullptr;
    }
}

// 初始化
bool GameObject::init() {
    if (!Node::init()) {
        return false;
    }

    setupDefaultSprite();
    updateTransform();

    return true;
}

bool GameObject::init(const std::string& objName, Type type) {
    if (!init()) {
        return false;
    }

    name = objName;
    objectType = type;

    return true;
}

// 创建工厂方法
GameObject* GameObject::create() {
    GameObject* obj = new (std::nothrow) GameObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

GameObject* GameObject::create(const std::string& objName, Type type) {
    GameObject* obj = new (std::nothrow) GameObject(objName, type);
    if (obj && obj->init(objName, type)) {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

// 生成唯一ID
void GameObject::generateId() {
    std::stringstream ss;
    ss << "obj_";
    ss << std::hex << std::setfill('0');

    // 使用时间戳和随机数生成ID
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    ss << millis << "_";
    ss << std::setw(4) << (rand() % 0x10000);

    id = ss.str();
}

// 设置世界位置
void GameObject::setWorldPosition(const Vec2& pos) {
    position = pos;
    updateTransform();
}

// 设置位置（重写Node的方法）
void GameObject::setPosition(const Vec2& pos) {
    Node::setPosition(pos);
    position = pos;

    if (objectSprite) {
        objectSprite->setPosition(Vec2::ZERO);
    }
}

// 设置大小
void GameObject::setSize(const Size& newSize) {
    size = newSize;
    if (objectSprite) {
        objectSprite->setContentSize(size);
    }
}

// 设置旋转（重写Node的方法）
void GameObject::setRotation(float rot) {
    Node::setRotation(rot);
    rotation = rot;

    if (objectSprite) {
        objectSprite->setRotation(0); // 精灵相对于父节点不旋转
    }
}

// 设置缩放（重写Node的方法）
void GameObject::setScale(float s) {
    Node::setScale(s);
    scale = s;

    if (objectSprite) {
        objectSprite->setScale(1.0f); // 精灵使用统一的缩放
    }
}

// 设置活跃状态
void GameObject::setActive(bool active) {
    isActive = active;
    this->setVisible(active);

    if (objectSprite) {
        objectSprite->setVisible(active);
    }
}

// 设置可交互状态
void GameObject::setInteractable(bool interactable) {
    isInteractable = interactable;
}

// 设置等级
void GameObject::setLevel(int lvl) {
    if (lvl < 1) lvl = 1;
    level = lvl;

    // 每升一级，需要更多经验
    maxExperience = 100 * level;

    // 升级时恢复耐久度
    if (lvl > 1) {
        durability = maxDurability;
    }
}

// 添加经验
void GameObject::addExperience(int exp) {
    if (exp <= 0) return;

    experience += exp;

    // 检查是否可以升级
    while (canLevelUp()) {
        levelUp();
    }
}

// 检查是否可以升级
bool GameObject::canLevelUp() const {
    return experience >= maxExperience;
}

// 升级
void GameObject::levelUp() {
    level++;
    experience -= maxExperience;
    maxExperience = 100 * level;

    // 每级增加最大耐久度
    maxDurability += 20;
    durability = maxDurability;

    CCLOG("GameObject %s leveled up to level %d", name.c_str(), level);
}

// 设置耐久度
void GameObject::setDurability(int dur) {
    durability = std::max(0, std::min(maxDurability, dur));
}

// 设置最大耐久度
void GameObject::setMaxDurability(int maxDur) {
    maxDurability = std::max(1, maxDur);
    if (durability > maxDurability) {
        durability = maxDurability;
    }
}

// 减少耐久度
void GameObject::reduceDurability(int amount) {
    if (amount <= 0) return;

    durability = std::max(0, durability - amount);

    if (isBroken()) {
        CCLOG("GameObject %s is broken!", name.c_str());
        // 可以触发破坏事件
    }
}

// 修复耐久度
void GameObject::repairDurability(int amount) {
    if (amount <= 0) return;

    durability = std::min(maxDurability, durability + amount);
}

// 设置精灵
void GameObject::setSprite(Sprite* sprite) {
    if (objectSprite) {
        objectSprite->removeFromParent();
    }

    objectSprite = sprite;
    if (objectSprite) {
        this->addChild(objectSprite);
        objectSprite->setPosition(Vec2::ZERO);
        objectSprite->setContentSize(size);
        objectSprite->setVisible(isActive);
    }
}

// 使用文件设置精灵
void GameObject::setSpriteWithFile(const std::string& filename) {
    if (filename.empty()) return;

    Sprite* sprite = Sprite::create(filename);
    if (sprite) {
        setSprite(sprite);
    }
    else {
        CCLOG("Failed to create sprite from file: %s", filename.c_str());
    }
}

// 移除精灵
void GameObject::removeSprite() {
    if (objectSprite) {
        objectSprite->removeFromParent();
        objectSprite = nullptr;
    }
}

// 播放动画
void GameObject::playAnimation(Action* action) {
    if (!action) return;

    stopAnimation();
    currentAction = action;
    this->runAction(action);
}

// 停止动画
void GameObject::stopAnimation() {
    if (currentAction) {
        this->stopAction(currentAction);
        currentAction = nullptr;
    }
}

// 是否在动画中
bool GameObject::isAnimating() const {
    return currentAction != nullptr && !currentAction->isDone();
}

// 交互
void GameObject::onInteract(GameObject* interactor) {
    if (!isInteractable || !isActive) return;

    CCLOG("GameObject %s interacted with by %s",
        name.c_str(), interactor ? interactor->getName().c_str() : "unknown");

    if (onInteractCallback) {
        onInteractCallback(this);
    }
}

// 点击
void GameObject::onClick() {
    if (!isInteractable || !isActive) return;

    CCLOG("GameObject %s clicked", name.c_str());

    if (onClickCallback) {
        onClickCallback(this);
    }
}

// 鼠标悬停进入
void GameObject::onHoverEnter() {
    if (!isInteractable || !isActive) return;

    // 可以高亮显示
    if (objectSprite) {
        objectSprite->setColor(Color3B::YELLOW);
    }
}

// 鼠标悬停离开
void GameObject::onHoverExit() {
    if (!isInteractable || !isActive) return;

    // 恢复颜色
    if (objectSprite) {
        objectSprite->setColor(Color3B::WHITE);
    }
}

// 更新
void GameObject::update(float delta) {
    Node::update(delta);

    // 可以在这里添加每帧的逻辑
}

// 进入场景
void GameObject::onEnter() {
    Node::onEnter();

    // 激活物理（如果需要）
    applyPhysicsIfNeeded();
}

// 退出场景
void GameObject::onExit() {
    Node::onExit();

    // 停止动画
    stopAnimation();
}

// 碰撞检测 - 是否包含点
bool GameObject::containsPoint(const Vec2& point) const {
    Rect bbox = getBoundingBox();
    return bbox.containsPoint(point);
}

// 碰撞检测 - 是否与其他对象相交
bool GameObject::intersects(const GameObject* other) const {
    if (!other) return false;

    Rect myBox = getBoundingBox();
    Rect otherBox = other->getBoundingBox();

    return myBox.intersectsRect(otherBox);
}

// 获取边界框
Rect GameObject::getBoundingBox() const {
    Vec2 worldPos = this->convertToWorldSpace(Vec2::ZERO);
    return Rect(worldPos.x, worldPos.y, size.width * scale, size.height * scale);
}

// 序列化
ValueMap GameObject::toValueMap() const {
    ValueMap map;

    map["id"] = id;
    map["name"] = name;
    map["description"] = description;
    map["type"] = static_cast<int>(objectType);
    map["isActive"] = isActive;
    map["isInteractable"] = isInteractable;

    map["positionX"] = position.x;
    map["positionY"] = position.y;
    map["sizeWidth"] = size.width;
    map["sizeHeight"] = size.height;
    map["rotation"] = rotation;
    map["scale"] = scale;

    map["level"] = level;
    map["experience"] = experience;
    map["maxExperience"] = maxExperience;
    map["durability"] = durability;
    map["maxDurability"] = maxDurability;

    return map;
}

// 反序列化
bool GameObject::fromValueMap(const ValueMap& map) {
    // 读取基础属性
    if (map.find("id") != map.end()) {
        id = map.at("id").asString();
    }

    if (map.find("name") != map.end()) {
        name = map.at("name").asString();
    }

    if (map.find("description") != map.end()) {
        description = map.at("description").asString();
    }

    if (map.find("type") != map.end()) {
        objectType = static_cast<Type>(map.at("type").asInt());
    }

    if (map.find("isActive") != map.end()) {
        isActive = map.at("isActive").asBool();
    }

    if (map.find("isInteractable") != map.end()) {
        isInteractable = map.at("isInteractable").asBool();
    }

    // 读取变换属性
    if (map.find("positionX") != map.end() && map.find("positionY") != map.end()) {
        position.x = map.at("positionX").asFloat();
        position.y = map.at("positionY").asFloat();
    }

    if (map.find("sizeWidth") != map.end() && map.find("sizeHeight") != map.end()) {
        size.width = map.at("sizeWidth").asFloat();
        size.height = map.at("sizeHeight").asFloat();
    }

    if (map.find("rotation") != map.end()) {
        rotation = map.at("rotation").asFloat();
    }

    if (map.find("scale") != map.end()) {
        scale = map.at("scale").asFloat();
    }

    // 读取游戏逻辑属性
    if (map.find("level") != map.end()) {
        level = map.at("level").asInt();
    }

    if (map.find("experience") != map.end()) {
        experience = map.at("experience").asInt();
    }

    if (map.find("maxExperience") != map.end()) {
        maxExperience = map.at("maxExperience").asInt();
    }

    if (map.find("durability") != map.end()) {
        durability = map.at("durability").asInt();
    }

    if (map.find("maxDurability") != map.end()) {
        maxDurability = map.at("maxDurability").asInt();
    }

    // 更新显示
    updateTransform();
    setActive(isActive);

    return true;
}

// 类型转字符串
std::string GameObject::typeToString(Type type) {
    switch (type) {
        case Type::UNDEFINED: return "Undefined";
        case Type::ITEM: return "Item";
        case Type::CROP: return "Crop";
        case Type::ANIMAL: return "Animal";
        case Type::FARM_TILE: return "FarmTile";
        case Type::NPC: return "NPC";
        case Type::PLAYER: return "Player";
        case Type::BUILDING: return "Building";
        case Type::TOOL: return "Tool";
        default: return "Unknown";
    }
}

// 字符串转类型
GameObject::Type GameObject::stringToType(const std::string& typeStr) {
    if (typeStr == "Item") return Type::ITEM;
    if (typeStr == "Crop") return Type::CROP;
    if (typeStr == "Animal") return Type::ANIMAL;
    if (typeStr == "FarmTile") return Type::FARM_TILE;
    if (typeStr == "NPC") return Type::NPC;
    if (typeStr == "Player") return Type::PLAYER;
    if (typeStr == "Building") return Type::BUILDING;
    if (typeStr == "Tool") return Type::TOOL;
    return Type::UNDEFINED;
}

// 设置交互回调
void GameObject::setOnInteractCallback(const GameObjectCallback& callback) {
    onInteractCallback = callback;
}

// 设置点击回调
void GameObject::setOnClickCallback(const GameObjectCallback& callback) {
    onClickCallback = callback;
}

// 设置默认精灵
void GameObject::setupDefaultSprite() {
    if (objectSprite) return;

    // 根据类型创建不同的默认精灵
    std::string filename;
    switch (objectType) {
        case Type::ITEM:
            filename = "icon/allScreenButton.png";
            break;
        case Type::CROP:
            filename = "plants/grass.png";
            break;
        case Type::ANIMAL:
            filename = "Animal/Chicken.png";
            break;
        case Type::FARM_TILE:
            filename = "plants/grass.png";
            break;
        default:
            filename = "icon/allScreenButton.png";
    }

    setSpriteWithFile(filename);
}

// 更新变换
void GameObject::updateTransform() {
    this->Node::setPosition(position);
    this->Node::setRotation(rotation);
    this->Node::setScale(scale);

    if (objectSprite) {
        objectSprite->setContentSize(size);
    }
}

// 应用物理（如果需要）
void GameObject::applyPhysicsIfNeeded() {
    // 这里可以添加物理引擎的集成
    // 例如：if (needsPhysics) { addPhysicsBody(); }
}
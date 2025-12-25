// 文件名：GameObject.h
// 功能： 游戏对象基类，定义游戏中所有对象的通用属性和方法
// 作者： Niu
#pragma once

#include "cocos2d.h"
#include <string>
#include <memory>
#include <functional>

USING_NS_CC;

class GameObject : public Node {
public:
    // 游戏对象类型枚举
    enum class Type {
        UNDEFINED = 0,
        ITEM = 1,
        CROP = 2,
        ANIMAL = 3,
        FARM_TILE = 4,
        NPC = 5,
        PLAYER = 6,
        BUILDING = 7,
        TOOL = 8
    };

protected:
    // 基础属性
    std::string id;                 // 唯一标识符
    std::string name;               // 显示名称
    std::string description;        // 描述
    Type objectType;                // 对象类型
    bool isActive;                  // 是否活跃
    bool isInteractable;            // 是否可以交互

    // 物理属性
    Vec2 position;                  // 位置
    Size size;                      // 大小
    float rotation;                 // 旋转角度
    float scale;                    // 缩放

    // 游戏逻辑属性
    int level;                      // 等级
    int experience;                 // 经验值
    int maxExperience;              // 升级所需经验
    int durability;                 // 耐久度（如果需要）
    int maxDurability;              // 最大耐久度

    // 精灵和动画
    Sprite* objectSprite;           // 主要精灵
    Action* currentAction;          // 当前动作

public:
    // 构造函数和析构函数
    GameObject();
    GameObject(const std::string& objName, Type type);
    virtual ~GameObject();

    // 初始化方法
    virtual bool init() override;
    virtual bool init(const std::string& objName, Type type);

    // 创建工厂方法
    static GameObject* create();
    static GameObject* create(const std::string& objName, Type type);

    // 属性访问器
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    Type getType() const { return objectType; }

    void setName(const std::string& newName) { name = newName; }
    void setDescription(const std::string& desc) { description = desc; }
    void setType(Type type) { objectType = type; }

    // 位置和变换
    Vec2 getWorldPosition() const { return position; }
    void setWorldPosition(const Vec2& pos);
    void setPosition(const Vec2& pos) override;

    Size getSize() const { return size; }
    void setSize(const Size& newSize);

    float getRotation() const { return rotation; }
    void setRotation(float rot) override;

    float getScale() const { return scale; }
    void setScale(float s) override;

    // 状态管理
    bool isObjectActive() const { return isActive; }
    void setActive(bool active);

    bool isObjectInteractable() const { return isInteractable; }
    void setInteractable(bool interactable);

    // 等级和经验
    int getLevel() const { return level; }
    void setLevel(int lvl);

    int getExperience() const { return experience; }
    void addExperience(int exp);
    bool canLevelUp() const;
    void levelUp();

    // 耐久度
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    void setDurability(int dur);
    void setMaxDurability(int maxDur);
    void reduceDurability(int amount);
    void repairDurability(int amount);
    bool isBroken() const { return durability <= 0; }

    // 精灵管理
    Sprite* getSprite() const { return objectSprite; }
    void setSprite(Sprite* sprite);
    void setSpriteWithFile(const std::string& filename);
    void removeSprite();

    // 动画管理
    void playAnimation(Action* action);
    void stopAnimation();
    bool isAnimating() const;

    // 交互系统
    virtual void onInteract(GameObject* interactor);
    virtual void onClick();
    virtual void onHoverEnter();
    virtual void onHoverExit();

    // 更新和生命周期
    virtual void update(float delta) override;
    virtual void onEnter() override;
    virtual void onExit() override;

    // 碰撞检测
    virtual bool containsPoint(const Vec2& point) const;
    virtual bool intersects(const GameObject* other) const;
    virtual Rect getBoundingBox() const;

    // 序列化/反序列化
    virtual ValueMap toValueMap() const;
    virtual bool fromValueMap(const ValueMap& map);

    // 工具方法
    static std::string typeToString(Type type);
    static Type stringToType(const std::string& typeStr);

    // 事件回调
    typedef std::function<void(GameObject*)> GameObjectCallback;
    void setOnInteractCallback(const GameObjectCallback& callback);
    void setOnClickCallback(const GameObjectCallback& callback);

protected:
    // 内部方法
    virtual void generateId();
    virtual void setupDefaultSprite();
    virtual void updateTransform();

    // 回调函数
    GameObjectCallback onInteractCallback;
    GameObjectCallback onClickCallback;

private:
    // 私有辅助方法
    void applyPhysicsIfNeeded();
}; #pragma once

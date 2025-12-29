#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include <unordered_map>

class Player : public cocos2d::Sprite
{
public:
    //构造函数声明
    Player();

    //析构函数声明
    virtual ~Player();

    // 方向枚举
    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
    };

    // 创建玩家
    static Player* create();
    
    // 初始化
    virtual bool init() override;
    
    // 更新函数
    void update(float delta);
    
    // 键盘事件处理
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode);
    
    // 设置移动速度
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    float getMoveSpeed() const { return moveSpeed; }
    
    // 获取当前方向
    Direction getCurrentDirection() const { return currentDirection; }
    
    // 设置动画帧率
    void setAnimationFrameRate(float frameRate) { animationFrameRate = frameRate; }
    float getAnimationFrameRate() const { return animationFrameRate; }
    
    // 获取玩家位置（世界坐标）
    cocos2d::Vec2 getPlayerPosition() const { return getPosition(); }
    
    // 检查是否有方向键被按下
    bool isAnyKeyPressed();
    // 更新最后按下的键
    void updateLastPressedKey();
    
private:
    // 移动速度
    float moveSpeed;
    
    // 动画帧率
    float animationFrameRate;
    
    // 当前方向
    Direction currentDirection;
    
    // 移动方向向量
    cocos2d::Vec2 moveDirection;
    
    // 最后按下的方向键
    Direction lastPressedKey;
    
    // 动画相关
    cocos2d::Vector<cocos2d::SpriteFrame*> walkAnimations[4]; // 0: up, 1: down, 2: left, 3: right
    cocos2d::Animate* currentAnimation;
    cocos2d::Action* currentAnimationAction;
    
    // 站立精灵
    cocos2d::SpriteFrame* standSprites[4]; // 0: up, 1: down, 2: left, 3: right
    
    // 键盘状态
    bool isMoving;
    std::unordered_map<cocos2d::EventKeyboard::KeyCode, bool> keyStates;
    
    // 动画状态
    bool isAnimationPlaying;
    
    // 初始化动画
    void initAnimations();
    
    // 加载动画帧
    void loadWalkAnimation(Direction dir, const std::string& path, int frameCount);
    void loadStandSprite(Direction dir, const std::string& path);
    
    // 播放动画
    void playWalkAnimation(Direction dir);
    void stopAnimation();
    void showStandSprite(Direction dir);
    
    // 更新移动
    void updateMovement(float delta);
    
    // 方向转换
    int directionToIndex(Direction dir);
    Direction keyCodeToDirection(cocos2d::EventKeyboard::KeyCode keyCode);
    
    // 检查是否有移动输入
    bool hasMovementInput();
    
    // 更新移动方向
    void updateMoveDirection();
    
    // 检查资源是否有效
    bool hasValidResources();
    // 创建占位符精灵
    void createPlaceholderSprite();
};
#endif // __PLAYER_H__
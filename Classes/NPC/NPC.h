#ifndef __NPC_H__
#define __NPC_H__

#include "cocos2d.h"
USING_NS_CC;


class NPC : public cocos2d::Sprite
{
public:
    // 创建NPC的静态方法
    static NPC* create(const std::string& npcImage);

    // 初始化函数
    virtual bool init(const std::string& npcImage);

    // 设置NPC属性
    
    void setDialogue(const std::vector<std::string>& dialogue);
    void setDialogue(const std::string& dialogue);

    // 对话相关
    void startDialogue();                    // 开始对话
    void showNextDialogue();                 // 显示下一句对话
    void endDialogue();                      // 结束对话

    // 交互相关
    bool isInRange(cocos2d::Vec2 position);  // 检查玩家是否在交互范围内

    // 鼠标/触摸事件
    void onMouseDown(cocos2d::Event* event); // 鼠标右键点击

    // 获取属性
    std::string getNPCName() const { return name; }
    bool isTalking() const { return _isTalking; }

    // 设置回调函数
    void setDialogueEndCallback(const std::function<void()>& callback) {
        _dialogueEndCallback = callback;
    }

private:
    std::string name;                       // NPC名字
    std::vector<std::string> _dialogueList;  // 对话列表
    int _currentDialogueIndex;               // 当前对话索引
    bool _isTalking;                         // 是否正在对话

    // UI元素
    cocos2d::Label* _nameLabel;              // 名字标签
    cocos2d::Sprite* _headSprite;            // npc头像
    cocos2d::Label* _dialogueLabel;          // 对话文本标签
    cocos2d::Sprite* _dialogueBubble;        // 对话框


    // 回调函数
    std::function<void()> _dialogueEndCallback; // 对话结束回调

    // 私有方法
    void createUI();                         // 创建UI元素
    void updateDialogueDisplay();            // 更新对话显示
};

#endif // __NPC_H__
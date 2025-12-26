
#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"

class MainMenu : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // 新游戏按钮回调函数
    void menuNewGameCallback(cocos2d::Ref* pSender);

    // NPC测试
    void MainMenu::menuNPCTestCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainMenu);
};

#endif // __MAINMENU_SCENE_H__

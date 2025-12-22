#ifndef __MENU_MANAGER_H__
#define __MENU_MANAGER_H__

#include "cocos2d.h"

class MenuManager
{
public:
    // 获取MenuManager的单例实例
    static MenuManager* getInstance();
    
    // 释放MenuManager实例
    static void destroyInstance();
    
    // 切换到指定场景
    void switchScene(cocos2d::Scene* scene);
    
    // 切换到主菜单场景
    void goToMainMenu();
    
    // 切换到游戏场景
    void goToGameScene();
    
    // 切换到设置场景
    void goToSettingsScene();
    
private:
    // 私有构造函数，防止外部创建实例
    MenuManager();
    
    // 私有析构函数
    ~MenuManager();
    
    // 单例实例
    static MenuManager* _instance;
};

#endif // __MENU_MANAGER_H__
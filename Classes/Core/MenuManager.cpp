#include "MenuManager.h"
#include "MenuScene.h"

// 初始化静态成员变量
MenuManager* MenuManager::_instance = nullptr;

MenuManager::MenuManager()
{
}

MenuManager::~MenuManager()
{
}

MenuManager* MenuManager::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new MenuManager();
    }
    return _instance;
}

void MenuManager::destroyInstance()
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}

void MenuManager::switchScene(cocos2d::Scene* scene)
{
    if (scene)
    {
        auto director = cocos2d::Director::getInstance();

        // 检查当前是否有场景
        if (director->getRunningScene())
        {
            // 有场景则使用replaceScene切换
            director->replaceScene(cocos2d::TransitionFade::create(0.5f, scene));
        }
        else
        {
            // 没有场景则使用runWithScene
            director->runWithScene(scene);
        }
    }
}

void MenuManager::goToMainMenu()
{
    // 创建主菜单场景并切换
    auto scene = HelloWorld::createScene();
    switchScene(scene);
}

void MenuManager::goToGameScene()
{
    // 这里可以创建游戏场景并切换
    // 由于当前项目中没有游戏场景类，暂时先创建一个空场景
    auto scene = cocos2d::Scene::create();
    switchScene(scene);
}

void MenuManager::goToSettingsScene()
{
    // 这里可以创建设置场景并切换
    // 由于当前项目中没有设置场景类，暂时先创建一个空场景
    auto scene = cocos2d::Scene::create();
    switchScene(scene);
}
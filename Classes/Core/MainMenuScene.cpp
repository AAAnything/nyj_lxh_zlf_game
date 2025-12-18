#include "MainMenuScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

bool MainMenu::init()
{
    if (!Scene::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 示例：一个简单的文字标题
    auto title = Label::createWithSystemFont(
        "My Farm Game",
        "Arial",
        48
    );
    title->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height * 0.7f
    ));
    this->addChild(title);

    return true;
}



#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
@brief    The cocos2d Application.

这里的私有继承对Director隐藏了一部分的接口。
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    在此处实现导演类和场景初始化代码。
	@return true    初始化成功，程序继续运行。
	@return false   初始化失败，程序终止。
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  当应用程序移动到后台时调用
    @param  应用程序的指针
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  当应用程序重新进入前台时调用
    @param  应用程序的指针
    */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_


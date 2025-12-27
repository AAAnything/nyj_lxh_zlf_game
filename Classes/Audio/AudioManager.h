#include "cocos2d.h"
#include "SimpleAudioEngine.h"

class AudioManager {
public:
    // 单例模式获取实例
    static AudioManager* getInstance();

    // 销毁实例
    static void destroyInstance();

    // ============= 背景音乐相关 =============

    // 播放主菜单背景音乐
    void playMainMenuMusic();

    // 播放游戏场景背景音乐
    void playGameMusic();

    // 停止当前背景音乐
    void stopBackgroundMusic();

    // 设置背景音乐音量 (0.0 ~ 1.0)
    void setBackgroundMusicVolume(float volume);

    // ============= 音效相关 =============

    // 播放按钮点击音效（所有按钮统一使用）
    void playButtonClickSound();

    // 设置音效音量 (0.0 ~ 1.0)
    void setEffectsVolume(float volume);

private:
    // 私有构造函数（单例模式）
    AudioManager();
    ~AudioManager();

    // 静态单例实例
    static AudioManager* instance;

    // 音频文件路径
    struct AudioPaths {
        // 背景音乐
        std::string mainMenuMusic = "sound/welcome.wav";
        std::string gameMusic = "sound/game.wav";

        // 音效
        std::string buttonClick = "sound/button.wav";
    };

    AudioPaths audioPaths;          // 音频文件路径配置

    // 音量设置
    float backgroundMusicVolume = 1.0f;  // 背景音乐默认音量 100%
    float effectsVolume = 0.7f;          // 音效默认音量 70%

    // 当前播放的音乐类型
    std::string currentMusicType;   // "mainMenu", "game", "none"
};
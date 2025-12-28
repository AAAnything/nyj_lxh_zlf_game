#include "AudioManager.h"

USING_NS_CC;
using namespace CocosDenshion;

// 静态成员初始化
AudioManager* AudioManager::instance = nullptr;


AudioManager* AudioManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) AudioManager();
        CCASSERT(instance, "FATAL: Not enough memory for AudioManager");
    }
    return instance;
}


void AudioManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}


AudioManager::AudioManager()
    : currentMusicType("none") {

    // 初始化音频引擎音量
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(backgroundMusicVolume);
    SimpleAudioEngine::getInstance()->setEffectsVolume(effectsVolume);
}


AudioManager::~AudioManager() {
    // 停止所有音频
    SimpleAudioEngine::getInstance()->stopAllEffects();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
}


// ============= 背景音乐相关 =============


// 主菜单音乐
void AudioManager::playMainMenuMusic() {
    // 如果已经在播放主菜单音乐，不重复播放
    if (currentMusicType == "mainMenu") return;

    // 停止当前音乐
    stopBackgroundMusic();

    // 播放主菜单音乐（循环播放）
    SimpleAudioEngine::getInstance()->playBackgroundMusic(
        audioPaths.mainMenuMusic.c_str(), true);

    currentMusicType = "mainMenu";
}


// 游戏界面音乐
void AudioManager::playGameMusic() {
    // 如果已经在播放游戏音乐，不重复播放
    if (currentMusicType == "game") return;

    // 停止当前音乐
    stopBackgroundMusic();

    // 播放游戏音乐（循环播放）
    SimpleAudioEngine::getInstance()->playBackgroundMusic(
        audioPaths.gameMusic.c_str(), true);

    currentMusicType = "game";
}


// 停止音乐
void AudioManager::stopBackgroundMusic() {
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    currentMusicType = "none";
}


 // 音量调节
void AudioManager::setBackgroundMusicVolume(float volume) {
    backgroundMusicVolume = volume;
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

// ============= 音效相关 =============


// 
void AudioManager::playButtonClickSound() {
    SimpleAudioEngine::getInstance()->playEffect(audioPaths.buttonClick.c_str());
}

void AudioManager::setEffectsVolume(float volume) {
    effectsVolume = volume;
    SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}
#pragma once
#include "CropConstant.h"
#include "cocos2d.h"
USING_NS_CC;

class ParsnipCrop
{
public:
    ParsnipCrop();

    void onDayPass();      // 新的一天
    void water();          // 浇水
    bool isMature() const;

    int getCurrentTileGID() const; // 关键：告诉地块用哪个 tile

private:
    int currentDay;    // 第几天（从 1 开始）
    bool wateredToday;

    int calculateTileGID() const;
};

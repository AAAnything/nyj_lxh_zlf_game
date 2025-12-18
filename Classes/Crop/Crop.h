#pragma once

class Crop {
protected:
    int growDays;     // 成熟需要的天数
    int currentDays; // 已生长天数

public:
    Crop(int growDays);
    virtual ~Crop() = default;

    virtual void OnDayPass();     // 每过一天调用
    virtual bool IsMature() const;
};

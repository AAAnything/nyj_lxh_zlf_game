#pragma once

enum class CropType {
    Carrot,
    Corn,
    Parsnip
};

class Crop {
public:
    explicit Crop(int growDays);
    virtual ~Crop() = default;

    virtual void onDayPass();   // 新的一天
    virtual void water();       // 浇水
    virtual bool isMature() const;

    // ⭐核心：返回 TMX 的 tile GID
    virtual int getCurrentTileGID() const = 0;

protected:
    int growDays;
    int currentDay;
    bool wateredToday;
};

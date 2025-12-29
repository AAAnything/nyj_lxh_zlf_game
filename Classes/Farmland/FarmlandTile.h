#pragma once
#include <memory>
#include "Crop/Crop.h"

enum class TileState { 
    Empty, 
    Tilled, 
    Seeded, 
    Mature 
};

class FarmlandTile {
private:
    TileState state;
    std::unique_ptr<Crop> crop;
public:
    FarmlandTile();

    bool Till();                               // 耕地
    bool Plant(std::unique_ptr<Crop> newCrop);// 播种
    bool Harvest();                            // 收获
    void OnDayPass();                          // 时间推进

    TileState GetState() const;
};

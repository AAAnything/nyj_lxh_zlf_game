#include "FarmlandTile.h"

// FarmlandTile类的实现文件
FarmlandTile::FarmlandTile()
    : state(TileState::Empty) {
}

// 耕地
bool FarmlandTile::Till() {
    if (state != TileState::Empty)
        return false;

    state = TileState::Tilled;
    return true;
}

// 播种
bool FarmlandTile::Plant(std::unique_ptr<Crop> newCrop) {
    if (state != TileState::Tilled || !newCrop)
        return false;

    crop = std::move(newCrop);
    state = TileState::Seeded;
    return true;
}

// 时间推进
void FarmlandTile::OnDayPass() {
    if (state == TileState::Seeded && crop) {
        crop->onDayPass();
        if (crop->isMature()) {
            state = TileState::Mature;
        }
    }
}

// 收获
bool FarmlandTile::Harvest() {
    if (state != TileState::Mature)
        return false;

    crop.reset();
    state = TileState::Empty;
    return true;
}

// 获取当前状态
TileState FarmlandTile::GetState() const {
    return state;
}

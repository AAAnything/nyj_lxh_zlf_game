#pragma once

enum class TileState { Empty, Tilled, Seeded, Mature };

class FarmlandTile {
    TileState state;
    std::unique_ptr<Crop> crop;
};

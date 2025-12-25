#include <iostream>
#include <memory>
#include "FarmlandTile.h"
#include "Crop.h"

// 简单作物 Carrot
class Carrot : public Crop {
public:
    Carrot() : Crop(3) {} // 3天成熟
};

int main() {
    FarmlandTile tile;

    std::cout << "初始状态: " << static_cast<int>(tile.GetState()) << std::endl;

    // 耕地
    if (tile.Till())
        std::cout << "耕地成功" << std::endl;
    else
        std::cout << "耕地失败" << std::endl;

    // 播种
    if (tile.Plant(std::make_unique<Carrot>()))
        std::cout << "播种成功" << std::endl;
    else
        std::cout << "播种失败" << std::endl;

    // 模拟过3天
    for (int day = 1; day <= 3; ++day) {
        tile.OnDayPass();
        std::cout << "Day " << day << ", 状态: " << static_cast<int>(tile.GetState()) << std::endl;
    }

    // 收获
    if (tile.Harvest())
        std::cout << "收获成功" << std::endl;
    else
        std::cout << "收获失败" << std::endl;

    std::cout << "最终状态: " << static_cast<int>(tile.GetState()) << std::endl;

    return 0;
}

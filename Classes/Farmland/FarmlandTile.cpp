// 文件名：FarmlandTile.cpp
// 功能： 地块类，管理地块状态、作物生长和交互
// 作者： Niu

#include "FarmlandTile.h"
#include "../Core/GameManager.h"
#include "../Items/WateringCan.h" // 假设有浇水壶物品
#include "../Items/Fertilizer.h"  // 假设有肥料物品

USING_NS_CC;

// 构造函数
FarmlandTile::FarmlandTile()
    : GameObject()
    , state(TileState::EMPTY)
    , plantedCrop(nullptr)
    , daysSincePlanted(0)
    , hasWaterToday(false)
    , hasFertilizer(false)
    , position(Vec2::ZERO)
    , tileSize(Size(64, 64))
    , consecutiveWaterDays(0)
    , cropStress(0)
    , fertilizerBonusDays(0)
    , tileSprite(nullptr)
    , cropSprite(nullptr) {
}

FarmlandTile::FarmlandTile(const Vec2& pos, const Size& size)
    : GameObject()
    , state(TileState::EMPTY)
    , plantedCrop(nullptr)
    , daysSincePlanted(0)
    , hasWaterToday(false)
    , hasFertilizer(false)
    , position(pos)
    , tileSize(size)
    , consecutiveWaterDays(0)
    , cropStress(0)
    , fertilizerBonusDays(0)
    , tileSprite(nullptr)
    , cropSprite(nullptr) {
}

FarmlandTile::~FarmlandTile() {
    if (plantedCrop) {
        delete plantedCrop;
        plantedCrop = nullptr;
    }

    if (tileSprite) {
        tileSprite->removeFromParent();
    }

    if (cropSprite) {
        cropSprite->removeFromParent();
    }
}

// 初始化
bool FarmlandTile::init() {
    if (!GameObject::init()) {
        return false;
    }

    createDefaultSprite();
    updateTileAppearance();

    return true;
}

// 耕地
bool FarmlandTile::till() {
    if (!canTill()) {
        CCLOG("Cannot till this tile");
        return false;
    }

    state = TileState::TILLED;
    updateTileAppearance();

    CCLOG("Tile tilled at position: (%.0f, %.0f)", position.x, position.y);
    return true;
}

// 播种
bool FarmlandTile::plant(Crop* crop) {
    if (!canPlant() || !crop) {
        CCLOG("Cannot plant on this tile or invalid crop");
        return false;
    }

    // 检查季节是否合适
    Season currentSeason = GameManager::getInstance().getCurrentSeason();
    if (!crop->canGrowInSeason(currentSeason)) {
        CCLOG("Crop cannot grow in current season");
        return false;
    }

    plantedCrop = crop;
    state = TileState::PLANTED;
    daysSincePlanted = 0;
    consecutiveWaterDays = 0;
    cropStress = 0;

    updateTileAppearance();

    CCLOG("Planted %s at position: (%.0f, %.0f)",
        crop->getName().c_str(), position.x, position.y);
    return true;
}

// 浇水
bool FarmlandTile::water() {
    if (!canWater()) {
        CCLOG("Cannot water this tile");
        return false;
    }

    hasWaterToday = true;
    consecutiveWaterDays++;

    applyWaterEffects();

    state = TileState::WATERED;
    updateTileAppearance();

    CCLOG("Tile watered at position: (%.0f, %.0f)", position.x, position.y);
    return true;
}

// 施肥
bool FarmlandTile::fertilize() {
    if (!canFertilize()) {
        CCLOG("Cannot fertilize this tile");
        return false;
    }

    hasFertilizer = true;
    fertilizerBonusDays = 2; // 施肥提供2天的生长加速

    applyFertilizerEffects();

    updateTileAppearance();

    CCLOG("Tile fertilized at position: (%.0f, %.0f)", position.x, position.y);
    return true;
}

// 收获
Item* FarmlandTile::harvest() {
    if (!canHarvest() || !plantedCrop) {
        CCLOG("Cannot harvest from this tile");
        return nullptr;
    }

    // 计算产量
    int baseYield = plantedCrop->getBaseYield();
    float yieldMultiplier = 1.0f;

    // 施肥增加产量
    if (hasFertilizer) {
        yieldMultiplier *= 1.5f;
    }

    // 连续浇水增加产量
    if (consecutiveWaterDays >= 3) {
        yieldMultiplier *= 1.2f;
    }

    // 减少作物压力
    if (cropStress > 0) {
        yieldMultiplier *= std::max(0.5f, 1.0f - (cropStress * 0.1f));
    }

    int finalYield = static_cast<int>(baseYield * yieldMultiplier);
    finalYield = std::max(1, finalYield); // 至少收获1个

    // 创建收获物品
    Item* harvestedItem = plantedCrop->createHarvest(finalYield);

    // 触发收获事件
    if (harvestedItem) {
        // 这里可以通知GameManager或其他系统
        GameManager::getInstance().onCropHarvested(plantedCrop->getType(), finalYield);
    }

    // 重置地块
    clear();

    CCLOG("Harvested %d %s from tile at position: (%.0f, %.0f)",
        finalYield, plantedCrop->getName().c_str(), position.x, position.y);

    return harvestedItem;
}

// 清除地块
void FarmlandTile::clear() {
    if (plantedCrop) {
        delete plantedCrop;
        plantedCrop = nullptr;
    }

    state = TileState::EMPTY;
    daysSincePlanted = 0;
    hasWaterToday = false;
    hasFertilizer = false;
    consecutiveWaterDays = 0;
    cropStress = 0;
    fertilizerBonusDays = 0;

    updateTileAppearance();
}

// 每天调用
void FarmlandTile::onDayPassed() {
    if (!plantedCrop || state == TileState::WITHERED) {
        return;
    }

    // 更新天数
    daysSincePlanted++;

    // 检查枯萎
    checkWithering();
    if (state == TileState::WITHERED) {
        updateTileAppearance();
        return;
    }

    // 如果有作物，处理生长
    if (plantedCrop) {
        // 检查季节是否合适
        Season currentSeason = GameManager::getInstance().getCurrentSeason();
        if (!plantedCrop->canGrowInSeason(currentSeason)) {
            // 在错误季节停止生长并增加压力
            cropStress += 2;
        }
        else {
            // 正常生长
            int growthDays = 1;

            // 如果今天浇过水，正常生长
            if (hasWaterToday) {
                // 连续浇水有加成
                if (consecutiveWaterDays >= 2) {
                    growthDays += 1; // 额外生长1天
                }
            }
            else {
                // 没浇水，增加压力
                cropStress++;
            }

            // 施肥加成
            if (fertilizerBonusDays > 0) {
                growthDays += 1;
                fertilizerBonusDays--;
            }

            // 应用生长
            for (int i = 0; i < growthDays; i++) {
                plantedCrop->grow();
            }

            // 检查是否成熟
            if (plantedCrop->isMature()) {
                state = TileState::MATURE;
            }
        }
    }

    // 重置每日浇水状态
    if (!hasWaterToday) {
        consecutiveWaterDays = 0; // 重置连续浇水天数
    }
    hasWaterToday = false;

    updateTileAppearance();
}

// 重置每日状态
void FarmlandTile::resetDailyState() {
    hasWaterToday = false;
}

// 状态检查方法
bool FarmlandTile::canTill() const {
    return state == TileState::EMPTY;
}

bool FarmlandTile::canPlant() const {
    return state == TileState::TILLED && !plantedCrop;
}

bool FarmlandTile::canWater() const {
    if (state == TileState::WITHERED) return false;
    return (state == TileState::PLANTED || state == TileState::WATERED) &&
        plantedCrop != nullptr &&
        !hasWaterToday;
}

bool FarmlandTile::canFertilize() const {
    if (state == TileState::WITHERED) return false;
    return (state == TileState::PLANTED || state == TileState::WATERED) &&
        plantedCrop != nullptr &&
        !hasFertilizer;
}

bool FarmlandTile::canHarvest() const {
    return state == TileState::MATURE && plantedCrop != nullptr;
}

bool FarmlandTile::isWithered() const {
    return state == TileState::WITHERED;
}

// 设置位置
void FarmlandTile::setPosition(const Vec2& pos) {
    position = pos;
    if (tileSprite) {
        tileSprite->setPosition(pos);
    }
    if (cropSprite) {
        cropSprite->setPosition(pos);
    }
}

// 更新显示
void FarmlandTile::updateTileAppearance() {
    updateTileSprite();
    updateCropSprite();
}

// 创建默认精灵
void FarmlandTile::createDefaultSprite() {
    if (!tileSprite) {
        tileSprite = Sprite::create();
        tileSprite->setPosition(position);
        tileSprite->setContentSize(tileSize);
        tileSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    }

    if (!cropSprite) {
        cropSprite = Sprite::create();
        cropSprite->setPosition(position);
        cropSprite->setContentSize(Size(tileSize.width * 0.8f, tileSize.height * 0.8f));
        cropSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
        cropSprite->setVisible(false);
    }
}

// 更新地块精灵
void FarmlandTile::updateTileSprite() {
    if (!tileSprite) return;

    std::string imageName = getTileImageName();

    if (!imageName.empty()) {
        // 如果有特定贴图，使用它
        Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(imageName);
        if (texture) {
            tileSprite->setTexture(texture);
        }
    }
    else {
        // 否则使用颜色表示
        Color4F color = getTileColor();
        tileSprite->setColor(Color3B(color.r * 255, color.g * 255, color.b * 255));
        tileSprite->setOpacity(color.a * 255);
    }
}

// 更新作物精灵
void FarmlandTile::updateCropSprite() {
    if (!cropSprite) return;

    if (plantedCrop && state != TileState::EMPTY && state != TileState::TILLED) {
        std::string imageName = getCropImageName();

        if (!imageName.empty()) {
            Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(imageName);
            if (texture) {
                cropSprite->setTexture(texture);
                cropSprite->setVisible(true);

                // 根据生长阶段调整大小和透明度
                if (plantedCrop) {
                    float scale = 0.3f + (plantedCrop->getGrowthProgress() * 0.7f);
                    cropSprite->setScale(scale);

                    if (state == TileState::WITHERED) {
                        cropSprite->setColor(Color3B::GRAY);
                    }
                    else {
                        cropSprite->setColor(Color3B::WHITE);
                    }
                }
            }
        }
    }
    else {
        cropSprite->setVisible(false);
    }
}

// 获取地块贴图名称
std::string FarmlandTile::getTileImageName() const {
    switch (state) {
        case TileState::EMPTY:
            return "plants/grass.png"; // 默认草地
        case TileState::TILLED:
            return "tools/hoe.png"; // 耕地贴图
        case TileState::PLANTED:
        case TileState::WATERED:
            return ""; // 使用颜色
        case TileState::MATURE:
            return ""; // 使用颜色
        case TileState::WITHERED:
            return ""; // 使用颜色
        default:
            return "";
    }
}

// 获取作物贴图名称
std::string FarmlandTile::getCropImageName() const {
    if (!plantedCrop) return "";

    // 根据作物类型和生长阶段返回贴图
    switch (plantedCrop->getType()) {
        case CropType::CARROT:
            return "food/carrot.png";
        case CropType::CORN:
            return "food/corn.png";
        case CropType::DOGBANE:
            return "food/parsnip.png"; // 假设使用这个
        default:
            return "";
    }
}

// 获取地块颜色
Color4F FarmlandTile::getTileColor() const {
    switch (state) {
        case TileState::EMPTY:
            return Color4F(0.2f, 0.8f, 0.2f, 0.3f); // 浅绿色
        case TileState::TILLED:
            return Color4F(0.6f, 0.4f, 0.2f, 0.8f); // 棕色
        case TileState::PLANTED:
            return Color4F(0.6f, 0.4f, 0.2f, 0.8f); // 棕色
        case TileState::WATERED:
            return Color4F(0.4f, 0.3f, 0.1f, 0.9f); // 深棕色（湿润）
        case TileState::MATURE:
            return Color4F(0.7f, 0.5f, 0.3f, 0.8f); // 浅棕色
        case TileState::WITHERED:
            return Color4F(0.3f, 0.3f, 0.3f, 0.6f); // 灰色
        default:
            return Color4F::WHITE;
    }
}

// 检查枯萎
void FarmlandTile::checkWithering() {
    if (!plantedCrop) return;

    // 检查作物压力
    if (cropStress >= 5) {
        state = TileState::WITHERED;
        return;
    }

    // 检查是否超过最大生长时间（成熟后太久不收获）
    int maxDays = plantedCrop->getMatureDays() + 5; // 成熟后最多5天
    if (daysSincePlanted > maxDays) {
        state = TileState::WITHERED;
    }
}

// 应用肥料效果
void FarmlandTile::applyFertilizerEffects() {
    if (!plantedCrop) return;

    // 施肥立即促进生长
    plantedCrop->grow();
    plantedCrop->grow(); // 额外生长2天

    CCLOG("Fertilizer applied to crop: %s", plantedCrop->getName().c_str());
}

// 应用浇水效果
void FarmlandTile::applyWaterEffects() {
    if (!plantedCrop) return;

    // 浇水促进生长
    plantedCrop->grow();

    // 如果连续浇水，额外效果
    if (consecutiveWaterDays >= 2) {
        plantedCrop->grow(); // 额外生长1天
    }

    // 减少作物压力
    cropStress = std::max(0, cropStress - 1);

    CCLOG("Water applied to crop: %s (consecutive days: %d)",
        plantedCrop->getName().c_str(), consecutiveWaterDays);
}

// 获取状态字符串
std::string FarmlandTile::getStatusString() const {
    std::string status;

    switch (state) {
        case TileState::EMPTY:
            status = "Empty";
            break;
        case TileState::TILLED:
            status = "Tilled";
            break;
        case TileState::PLANTED:
            status = "Planted";
            if (plantedCrop) {
                status += " - " + plantedCrop->getName();
            }
            break;
        case TileState::WATERED:
            status = "Watered";
            if (plantedCrop) {
                status += " - " + plantedCrop->getName();
            }
            break;
        case TileState::MATURE:
            status = "Ready to Harvest";
            if (plantedCrop) {
                status += " - " + plantedCrop->getName();
            }
            break;
        case TileState::WITHERED:
            status = "Withered";
            if (plantedCrop) {
                status += " - " + plantedCrop->getName();
            }
            break;
        default:
            status = "Unknown";
    }

    return status;
}

// 序列化
ValueMap FarmlandTile::toValueMap() const {
    ValueMap map;

    map["state"] = static_cast<int>(state);
    map["daysSincePlanted"] = daysSincePlanted;
    map["hasWaterToday"] = hasWaterToday;
    map["hasFertilizer"] = hasFertilizer;
    map["consecutiveWaterDays"] = consecutiveWaterDays;
    map["cropStress"] = cropStress;
    map["fertilizerBonusDays"] = fertilizerBonusDays;
    map["positionX"] = position.x;
    map["positionY"] = position.y;

    if (plantedCrop) {
        map["crop"] = plantedCrop->toValueMap();
    }

    return map;
}

// 反序列化
bool FarmlandTile::fromValueMap(const ValueMap& map) {
    // 清除现有作物
    if (plantedCrop) {
        delete plantedCrop;
        plantedCrop = nullptr;
    }

    // 读取基本状态
    if (map.find("state") != map.end()) {
        state = static_cast<TileState>(map.at("state").asInt());
    }

    if (map.find("daysSincePlanted") != map.end()) {
        daysSincePlanted = map.at("daysSincePlanted").asInt();
    }

    if (map.find("hasWaterToday") != map.end()) {
        hasWaterToday = map.at("hasWaterToday").asBool();
    }

    if (map.find("hasFertilizer") != map.end()) {
        hasFertilizer = map.at("hasFertilizer").asBool();
    }

    if (map.find("consecutiveWaterDays") != map.end()) {
        consecutiveWaterDays = map.at("consecutiveWaterDays").asInt();
    }

    if (map.find("cropStress") != map.end()) {
        cropStress = map.at("cropStress").asInt();
    }

    if (map.find("fertilizerBonusDays") != map.end()) {
        fertilizerBonusDays = map.at("fertilizerBonusDays").asInt();
    }

    if (map.find("positionX") != map.end() && map.find("positionY") != map.end()) {
        position.x = map.at("positionX").asFloat();
        position.y = map.at("positionY").asFloat();
    }

    // 读取作物
    if (map.find("crop") != map.end()) {
        ValueMap cropMap = map.at("crop").asValueMap();
        // 这里需要根据作物类型创建相应的作物对象
        // 暂时留空，需要CropFactory支持
    }

    updateTileAppearance();
    return true;
}
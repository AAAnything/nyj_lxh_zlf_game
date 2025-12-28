#include "CollectItem.h"

CollectItem* CollectItem::create(const std::string& name, const std::string& sprite,
    CollectType type, int baseValue) {
    CollectItem* item = new CollectItem();
    if (item && item->init(name, sprite)) {
        item->autorelease();
        item->collectType = type;
        item->sellPrice = baseValue;

        // 根据类型设置默认工具和点击次数
        switch (type) {
            case CollectType::WOOD:
                item->requiredTool = "axe";
                item->requiredClicks = 2;
                break;
            case CollectType::STONE:
                item->requiredTool = "pickaxe";
                item->requiredClicks = 2;
                break;
            case CollectType::GRASS:
                item->requiredTool = "sickle";
                item->requiredClicks = 2;
                break;
            case CollectType::TREE:
                item->requiredTool = "axe";
                item->requiredClicks = 5;
                break;
            case CollectType::SHELL:
                item->requiredTool = "";  // 空字符串表示无需工具
                item->requiredClicks = 1;
                break;
        }

        return item;
    }

    delete item;
    return nullptr;
}

bool CollectItem::init(const std::string& name, const std::string& sprite) {
    this->name = name;
    this->spriteName = sprite;
    return true;
}
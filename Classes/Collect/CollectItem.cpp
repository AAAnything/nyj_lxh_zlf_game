#include "CollectItem.h"
#include "cocos2d.h"

USING_NS_CC;

CollectItem* CollectItem::create(const std::string& name, const std::string& sprite,
    CollectType type, int baseValue) {
    CollectItem* item = new CollectItem();
    if (item && item->init(name, sprite)) {
        item->autorelease();
        item->collectType = type;
        item->setSellPrice(baseValue);  // 设置基础价值

        // 根据类型设置默认参数
        switch (type) {
            case CollectType::WOOD:
                item->requiredTool = "axe";
                item->requiredClicks = 1;  // 木头需要点击1次
                break;
            case CollectType::STONE:
                item->requiredTool = "pickaxe";
                item->requiredClicks = 1;  // 石头需要点击1次
                break;
            case CollectType::GRASS:
                item->requiredTool = "sickle";  // 草可以用镰刀，但斧子也可以
                item->requiredClicks = 1;       // 草需要点击1次
                break;
            case CollectType::TREE:
                item->requiredTool = "axe";
                item->requiredClicks = 5;       // 树需要点击5次
                break;
        }

        return item;
    }
    CC_SAFE_DELETE(item);
    return nullptr;
}
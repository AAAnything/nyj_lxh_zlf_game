#include "Core/GameObject.h"
#include "Core/Item.h"

// 采集物品类型枚举
enum class CollectType {
    WOOD,       // 木头
    STONE,      // 石头  
    GRASS,      // 草
    TREE        // 树（大树，产更多木材）
};

// 采集物品类
// 继承自Item，可以直接放入背包系统
class CollectItem : public Item {
public:
    // 创建采集物品的工厂方法
    // @param name: 物品名称
    // @param sprite: 图标路径
    // @param type: 采集类型
    // @param baseValue: 基础价值
    static CollectItem* create(const std::string& name, const std::string& sprite,
        CollectType type, int baseValue);

    // 获取采集类型
    CollectType getCollectType() const { return collectType; }

    // 获取所需工具类型
    std::string getRequiredTool() const { return requiredTool; }

    // 获取需要点击次数
    int getRequiredClicks() const { return requiredClicks; }

    // 设置所需工具类型
    void setRequiredTool(const std::string& tool) { requiredTool = tool; }

    // 设置需要点击次数
    void setRequiredClicks(int clicks) { requiredClicks = clicks; }

private:
    CollectType collectType;      // 采集类型
    std::string requiredTool;   // 所需工具：axe/pickaxe/sickle
    int requiredClicks;         // 需要点击次数
};
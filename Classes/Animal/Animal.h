// 文件名： Animal.h
// 功能：  动物类的定义文件，负责动物的属性和行为逻辑。
// 作者：  Niu

#pragma once
#include "cocos2d.h"
#include "Item/Item.h"
USING_NS_CC;

// 动物视觉状态枚举
enum class AnimalVisualState
{
	Normal,
	ProductReady
};

class Animal : public cocos2d::Sprite
{
public:
	// 创建动物的静态方法
	static Animal* create(const std::string& animalImage);
	// 初始化函数
	virtual bool init(const std::string& animalImage);
	// 设置动物属性
	void setName(const std::string& name) { _name = name; } // 动物名字
	// 动物视觉状态
	AnimalVisualState _visualState = AnimalVisualState::Normal;
	// 动物行为
	void play();                            // 玩耍
	void updateStatus(float dt);            // 更新状态
	void Animal::showHeart(); // 爱心动效
	void Animal::startIdleMove(); // 空闲自动移动
	void Animal::stopIdleMove(); // 中断移动

	// 收获产品
	ItemType harvestProduct();
	
private:
	std::string _name;       // 动物名字
	float _lastPetTime = -100.f;  // 上一次被抚摸的时间
	int harvestTime; // 收获时间间隔（秒）
	int timeSinceLastHarvest; // 自上次收获以来的时间（秒）
	bool _productReady;	// 产品是否准备好
	// 更新动物外观或状态
	void updateAnimalAppearance();
protected:
	ItemType _productType = ItemType::None;

};

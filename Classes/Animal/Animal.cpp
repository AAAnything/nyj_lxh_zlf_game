// 文件名： Animal.cpp
// 功能：  动物类的实现文件，负责动物的属性和行为逻辑。
// 作者：  Niu

#include "Animal.h"


// 创建动物
Animal* Animal::create(const std::string& animalImage, const AnimalType animalType)
{
	Animal* animal = new (std::nothrow) Animal();
	animal->type = animalType;
	if (animal && animal->init(animalImage))
	{
		animal->autorelease();
		return animal;
	}
	CC_SAFE_DELETE(animal);
	return nullptr;
}

// 初始化
bool Animal::init(const std::string& animalImage)
{
	// 调用父类Sprite的init
	if (!Sprite::initWithFile(animalImage))
	{
		// 如果图片加载失败，使用默认图片
		if (!Sprite::initWithFile("Animal\Chicken.png"))
		{
			CCLOG("Error : Loading pictures failed: %s", animalImage.c_str());
			return false;
		}
	}
	// 初始化变量
	name = "Animal";
	// 启用定时更新状态
	this->schedule(CC_SCHEDULE_SELECTOR(Animal::updateStatus), 0.2f); // 每0.2秒更新一次状态
	return true;
}

// 开启/关闭自动移动
void Animal::startIdleMove()
{
	if (productReady) return;              // 可收获时禁止踱步
	if (getActionByTag(IDLE_MOVE_TAG)) return;

	auto moveLeft = MoveBy::create(1.5f, Vec2(-10, 0));
	auto moveRight = MoveBy::create(1.5f, Vec2(10, 0));

	auto seq = Sequence::create(moveLeft, moveRight, nullptr);
	auto loop = RepeatForever::create(seq);
	loop->setTag(IDLE_MOVE_TAG);

	runAction(loop);
}

void Animal::stopIdleMove()
{
	this->stopActionByTag(IDLE_MOVE_TAG);
	this->setPositionX(roundf(this->getPositionX())); // 防止浮点残留抖动
}

// 玩耍
void Animal::play()
{
	// 创建跳跃动作
	auto jumpUp = MoveBy::create(0.2f, Vec2(0, 20));
	auto jumpDown = MoveBy::create(0.2f, Vec2(0, -20));
	auto jumpSequence = Sequence::create(jumpUp, jumpDown, jumpUp, jumpDown, nullptr);
	this->runAction(jumpSequence);
	lastPetTime = Director::getInstance()->getTotalFrames();

	// 头顶冒爱心
	stopIdleMove();
	showHeart(); 
	startIdleMove();
}

// 视觉效果：头顶冒爱心
void Animal::showHeart()
{

	// 防止疯狂刷爱心
	if (this->getChildByName("pet_heart"))
		return;

	auto heart = Sprite::create("ui/heart.png");
	heart->setName("pet_heart");

	// 位置：动物头顶
	heart->setPosition(Vec2(
		this->getContentSize().width / 2,
		this->getContentSize().height + 10
	));

	heart->setScale(0.04f); // 大小设置为原图的0.04倍
	heart->setOpacity(0);

	this->addChild(heart, 10); // 对象，图层层数

	// 动画：上浮 + 渐显 + 消失
	auto fadeIn = FadeIn::create(0.15f); // 渐入
	auto moveUp = MoveBy::create(0.6f, Vec2(0, 20)); // 上移
	auto fadeOut = FadeOut::create(0.3f); // 淡出

	auto seq = Sequence::create(
		Spawn::create(fadeIn, moveUp, nullptr),
		fadeOut,
		RemoveSelf::create(),
		nullptr
	);

	heart->runAction(seq);
}

// 生产时更改外观
void Animal::updateAnimalAppearance()
{
	if (productReady && visualState != AnimalVisualState::ProductReady)
	{
		this->stopActionByTag(1001);// 停止之前的动作

		// 创建闪烁动作
		auto tintToGold = TintTo::create(0.4f, 255, 200, 100);
		auto tintBack = TintTo::create(0.4f, 255, 255, 255);
		auto seq = Sequence::create(tintToGold, tintBack, nullptr);
		// 无限闪烁
		auto loop = RepeatForever::create(seq);
		loop->setTag(1001); // 给“产品准备好闪烁”打个标签
		// 运行动作
		this->runAction(loop);
		visualState = AnimalVisualState::ProductReady; // 更新视觉状态
	}
	else if (!productReady && visualState != AnimalVisualState::Normal)
	{
		this->stopActionByTag(1001);// 停止之前的动作
		this->setColor(Color3B::WHITE); // 恢复原色
		visualState = AnimalVisualState::Normal; // 更新视觉状态
	}
}

void Animal::updateStatus(float dt)
{
	// ===== 生产计时 =====
	if (!productReady)
	{
		timeSinceLastHarvest += dt;

		if (timeSinceLastHarvest >= PRODUCT_INTERVAL)
		{
			productReady = true;
			stopIdleMove();          // 可收获 → 停止踱步
		}
	}

	// ===== 踱步逻辑（只在普通状态）=====
	if (!productReady)
	{
		// 距离上次抚摸超过 2 秒
		if (Director::getInstance()->getTotalFrames() - lastPetTime > 120)
		{
			startIdleMove();
		}
	}

	updateAnimalAppearance();
}
	

// 收获产品
ItemType Animal::harvestProduct()
{
	if (!productReady)
		return ItemType::None;

	productReady = false;
	updateStatus(1);
	timeSinceLastHarvest = 0.0f;   // ⭐关键：重置生产计时

	stopIdleMove();
	stopActionByTag(PRODUCT_FLASH_TAG);
	setColor(Color3B::WHITE);

	switch (type)
	{
		case AnimalType::Chicken: return ItemType::Egg;
		case AnimalType::Cow:     return ItemType::Milk;
		case AnimalType::Sheep:   return ItemType::Wool;
		default:                  return ItemType::None;
	}
}

bool Animal::canHarvest() const
{
	return productReady;
}
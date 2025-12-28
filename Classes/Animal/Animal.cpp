// 文件名： Animal.cpp
// 功能：  动物类的实现文件，负责动物的属性和行为逻辑。
// 作者：  Niu

#include "Animal.h"
#include "cocos2d.h"
USING_NS_CC;

constexpr int ACTION_IDLE = 2001;


// 创建动物
Animal* Animal::create(const std::string& animalImage)
{
	Animal* animal = new (std::nothrow) Animal();
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
			CCLOG("错误：无法加载动物图片: %s", animalImage.c_str());
			return false;
		}
	}
	// 初始化变量
	_name = "Animal";
	// 启用定时更新状态
	this->schedule(CC_SCHEDULE_SELECTOR(Animal::updateStatus), 0.2f); // 每0.2秒更新一次状态
	return true;
}

// 开启/关闭自动移动
void Animal::startIdleMove()
{
	// 防止重复添加
	if (this->getActionByTag(ACTION_IDLE))
		return;

	auto moveForward = MoveBy::create(2.0f, Vec2(10, 0));
	auto moveBack = MoveBy::create(2.0f, Vec2(-10, 0));

	auto easeForward = EaseSineInOut::create(moveForward);
	auto easeBack = EaseSineInOut::create(moveBack);

	auto seq = Sequence::create(easeForward, easeBack, nullptr);
	auto loop = RepeatForever::create(seq);

	loop->setTag(ACTION_IDLE);
	this->runAction(loop);
}
void Animal::stopIdleMove()
{
	this->stopActionByTag(ACTION_IDLE);
	this->setPositionX(roundf(this->getPositionX())); // 防止浮点残留抖动
}

// 玩耍
void Animal::play()
{
	// 靠近动物时鼠标左键点击触发，动物向上跳两下
	// 鼠标右键检测逻辑在场景管理类中实现，这里只实现跳跃动作
	// 创建跳跃动作
	auto jumpUp = MoveBy::create(0.2f, Vec2(0, 20));
	auto jumpDown = MoveBy::create(0.2f, Vec2(0, -20));
	auto jumpSequence = Sequence::create(jumpUp, jumpDown, jumpUp, jumpDown, nullptr);
	this->runAction(jumpSequence);
	_lastPetTime = Director::getInstance()->getTotalFrames();

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
	if (_productReady && _visualState != AnimalVisualState::ProductReady)
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
		_visualState = AnimalVisualState::ProductReady; // 更新视觉状态
	}
	else if (!_productReady && _visualState != AnimalVisualState::Normal)
	{
		this->stopActionByTag(1001);// 停止之前的动作
		this->setColor(Color3B::WHITE); // 恢复原色
		_visualState = AnimalVisualState::Normal; // 更新视觉状态
	}
}

void Animal::updateStatus(float dt)
{
	// 示例：暂时用测试值
	if (!_productReady)
	{
		// 距离上次抚摸超过 2 秒
		if (Director::getInstance()->getTotalFrames() - _lastPetTime > 120)
		{
			startIdleMove();
		}
	}
	else
	{
		stopIdleMove();
	}

	updateAnimalAppearance();
}

// 收获产品
ItemType Animal::harvestProduct()
{
	if (!_productReady)
		return ItemType::None;

	_productReady = false;

	// 更新外观或状态
	updateStatus(0);
	updateAnimalAppearance();

	return _productType;
}
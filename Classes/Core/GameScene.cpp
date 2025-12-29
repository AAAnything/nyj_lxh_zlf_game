#include"GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

Scene* GameScene::createSceneWithMap(const std::string& tmxFile, const Vec2& playerPos)
{
    GameScene* scene = new (std::nothrow) GameScene();
    if (scene && scene->initWithMap(tmxFile, playerPos))
    {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return nullptr;
}

bool GameScene::init()
{
    return initWithMap("Maps/farmSpring/farmMap.tmx", Vec2(0, 0));
}

bool GameScene::initWithMap(const std::string& tmxFile, const Vec2& playerPos)
{
    if (!Scene::init())
        return false;

    viewSize = Director::getInstance()->getVisibleSize();
    _currentMapFile = tmxFile;

    tileMap = TMXTiledMap::create(tmxFile);

    if (tileMap)
    {
        mapSize = Size(tileMap->getMapSize().width * tileMap->getTileSize().width,
            tileMap->getMapSize().height * tileMap->getTileSize().height);

        mapPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

        float scaleX = viewSize.width / mapSize.width;
        float scaleY = viewSize.height / mapSize.height;
        float minScale = std::min(scaleX, scaleY);
        float desiredScale = minScale * 1.5f;
        tileMap->setScale(desiredScale);

        scaledMapSize = Size(mapSize.width * desiredScale, mapSize.height * desiredScale);

        tileMap->setAnchorPoint(Vec2(0.5f, 0.5f));
        tileMap->setPosition(Vec2(viewSize.width / 2, viewSize.height / 2));

        this->addChild(tileMap, 0);
    }
    else
    {
        CCLOG("Failed to load tile map: %s", tmxFile.c_str());
        return false;
    }

    player = Player::create();
    if (player)
    {
        if (playerPos != Vec2::ZERO)
        {
            player->setPosition(playerPos);
        }
        else
        {
            player->setPosition(Vec2(mapSize.width / 2, mapSize.height / 2));
        }
        
        player->setMoveSpeed(150.0f);
        player->setAnimationFrameRate(10.0f);
        
        tileMap->addChild(player, 10); 
    }
    else
    {
        CCLOG("Failed to create player!");
        return false;
    }

    moveSpeed = 200.0f;

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    scheduleUpdate();

    return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = true;
    
    if (player && player->getParent())
    {
        player->onKeyPressed(keyCode);
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = false;
    
    if (player && player->getParent())
    {
        player->onKeyReleased(keyCode);
    }
}

void GameScene::update(float delta)
{
    if (player)
    {
        player->update(delta);
        updateCamera();
        checkTeleportPoints();
    }
}

void GameScene::updateCamera()
{
    if (!player || !tileMap) return;
    
    Vec2 playerPos = player->getPlayerPosition();
    mapPosition = playerPos;
    
    float visibleMapWidth = viewSize.width / tileMap->getScale();
    float visibleMapHeight = viewSize.height / tileMap->getScale();
    
    float minX = visibleMapWidth / 2;
    float maxX = mapSize.width - visibleMapWidth / 2;
    float minY = visibleMapHeight / 2;
    float maxY = mapSize.height - visibleMapHeight / 2;
    
    minX = std::max(0.0f, minX);
    maxX = std::min(mapSize.width, maxX);
    minY = std::max(0.0f, minY);
    maxY = std::min(mapSize.height, maxY);
    
    if (mapSize.width > viewSize.width)
    {
        mapPosition.x = clampf(mapPosition.x, minX, maxX);
    }
    else
    {
        mapPosition.x = mapSize.width / 2;
    }
    
    if (mapSize.height > viewSize.height)
    {
        mapPosition.y = clampf(mapPosition.y, minY, maxY);
    }
    else
    {
        mapPosition.y = mapSize.height / 2;
    }
    
    tileMap->setPosition(Vec2(viewSize.width / 2 - (mapPosition.x - mapSize.width / 2) * tileMap->getScale(),
                             viewSize.height / 2 - (mapPosition.y - mapSize.height / 2) * tileMap->getScale()));
}

void GameScene::checkTeleportPoints()
{
    if (!tileMap || !player) return;
    
    TMXObjectGroup* teleportGroup = tileMap->getObjectGroup("teleport");
    if (!teleportGroup) return;
    
    Vec2 playerPos = player->getPlayerPosition();
    Vec2 mapPos = tileMap->getPosition();
    float scale = tileMap->getScale();
    
    if (!teleportGroup) {
        // 建议添加一个只显示一次的错误日志，防止刷屏
        static bool hasLoggedError = false;
        if (!hasLoggedError) {
            CCLOG("Error: Object group 'teleport' not found in TMX file!");
            hasLoggedError = true;
        }
        return;
    }
    
    auto& objects = teleportGroup->getObjects();
    for (const auto& obj : objects)
    {
        ValueMap dict = obj.asValueMap();
        
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        CCLOG("传送区域: x=%.2f, y=%.2f, w=%.2f, h=%.2f", x, y, width, height);
        
        Rect zoneRect(x, y, width, height);
        
        if (isPlayerInTeleportZone(zoneRect))
        {
            CCLOG(">>> 检测到传送触发！准备切换地图 <<<");
            
            std::string targetMapName = dict["targetMap"].asString();
            std::string targetMap;
            if (targetMapName.empty()) {
                CCLOG("错误：targetMap 属性为空！");
                return;
            }
            if (targetMapName == "forestMap.tmx") 
                targetMap = "Maps/forest/" + targetMapName;
            else if (targetMapName == "farmMap.tmx") 
                targetMap = "Maps/farmSpring/" + targetMapName;
            else if (targetMapName == "beachMap.tmx")
                targetMap = "Maps/beach/" + targetMapName;
            else 
                targetMap = targetMapName;
            
            float targetX = dict["targetX"].asFloat();
            
            float targetY;
            Value targetYVal = dict["targetY"];
            if (!targetYVal.isNull())
            {
                std::string targetYStr = targetYVal.asString();
                if (targetYStr.find("mapSize/4*3") != std::string::npos)
                {
                    targetY = mapSize.height / 4 * 3;
                }
                else if (targetYStr.find("mapSize") != std::string::npos)
                {
                    targetY = mapSize.height / 2;
                }
                else
                {
                    targetY = targetYVal.asFloat();
                }
            }
            else
            {
                targetY = mapSize.height / 2;
            }
            
            CCLOG("目标地图: %s", targetMap.c_str());
            CCLOG("目标位置: (%.2f, %.2f)", targetX, targetY);
            
            MenuManager::getInstance()->switchToGameSceneWithMap(targetMap, Vec2(targetX, targetY));
            break;
        }
    }
}

bool GameScene::isPlayerInTeleportZone(const Rect& zoneRect)
{
    if (!player) return false;
    
    Vec2 playerPos = player->getPlayerPosition();
    
    float mapScale = tileMap->getScale();
    Vec2 playerOnMap;
    
    float playerWidth = 32.0f;
    float playerHeight = 48.0f;
    
    Rect playerRect(playerPos.x - playerWidth / 2, 
                   playerPos.y - playerHeight / 2, 
                   playerWidth, playerHeight);
    
    return playerRect.intersectsRect(zoneRect);
}

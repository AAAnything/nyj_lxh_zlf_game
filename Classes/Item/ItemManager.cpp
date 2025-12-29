// ItemManager.cpp
#include "ItemManager.h"
// Initialize static instance
ItemManager* ItemManager::instance = nullptr;

ItemManager::ItemManager() {
    // Private constructor
}

ItemManager::~ItemManager() {
    cleanup();
}

ItemManager* ItemManager::getInstance() {
    if (!instance) {
        instance = new ItemManager();
    }
    return instance;
}

void ItemManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

bool ItemManager::initialize() {
    // Load all items from resources
    loadItemsFromResources();
    return true;
}

Texture2D* ItemManager::loadTexture(const std::string& texturePath) {
    // Check if texture already exists
    auto it = textureMap.find(texturePath);
    if (it != textureMap.end()) {
        return it->second;
    }
    
    // Load texture
    Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
    if (texture) {
        textureMap[texturePath] = texture;
    }
    
    return texture;
}

void ItemManager::loadItemsFromResources() {
    // Get the path to the tools folder
    std::string toolsPath = FileUtils::getInstance()->fullPathForFilename("tools");
    
    // Iterate through all .png files in the tools folder
    for (const auto& entry : std::filesystem::directory_iterator(toolsPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            std::string filePath = entry.path().string();
            std::string filename = entry.path().filename().string();
            // Extract item name by removing .png extension
            std::string itemName = filename.substr(0, filename.find(".png"));
            
            // Create item ID (using filename without extension)
            std::string itemId = itemName;
            
            // Create item instance
            Item* item = Item::create(itemId, itemName, "tools/" + filename);
            
            // Special handling for kettle items
            if (itemName == "kettle") {
                item->setState(0); // Empty state
            } else if (itemName == "kettle5") {
                item->setState(1); // Full state
                // We'll handle this specially in processKettleItems()
            }
            
            // Add item to map
            itemMap[itemId] = item;
        }
    }
    
    // Process kettle items specially
    processKettleItems();
}

void ItemManager::processKettleItems() {
    // Check if kettle and kettle5 exist
    auto kettleIt = itemMap.find("kettle");
    auto kettle5It = itemMap.find("kettle5");
    
    if (kettleIt != itemMap.end() && kettle5It != itemMap.end()) {
        // Set the same ID for both kettles (they're the same item with different states)
        Item* kettle5 = kettle5It->second;
        kettle5->setId("kettle");
        kettle5->setName("kettle");
        
        // Update the map to use the same ID for both states
        // We'll keep both in the map for now, but they'll have the same ID
    }
}

Item* ItemManager::createItem(const std::string& itemId) {
    // Find the item prototype
    auto it = itemMap.find(itemId);
    if (it == itemMap.end()) {
        CCLOG("Item not found: %s", itemId.c_str());
        return nullptr;
    }
    
    // Create a new instance of the item
    Item* prototype = it->second;
    Item* newItem = Item::create(prototype->getId(), prototype->getName(), prototype->getTexturePath());
    newItem->setMaxStackSize(prototype->getMaxStackSize());
    newItem->setIsUsable(prototype->getIsUsable());
    newItem->setPrice(prototype->getPrice());
    newItem->setState(prototype->getState());
    
    return newItem;
}

Item* ItemManager::getItemById(const std::string& itemId) const {
    auto it = itemMap.find(itemId);
    if (it != itemMap.end()) {
        return it->second;
    }
    return nullptr;
}

Texture2D* ItemManager::getTexture(const std::string& texturePath) {
    // Check if texture already exists
    auto it = textureMap.find(texturePath);
    if (it != textureMap.end()) {
        return it->second;
    }
    
    // Load texture if it doesn't exist
    return loadTexture(texturePath);
}

void ItemManager::cleanup() {
    // Release all items
    for (auto& pair : itemMap) {
        pair.second->release();
    }
    itemMap.clear();
    
    // Textures are managed by the texture cache, so we don't need to release them
    textureMap.clear();
}

std::vector<std::string> ItemManager::getAllItemIds() const {
    std::vector<std::string> itemIds;
    for (const auto& pair : itemMap) {
        itemIds.push_back(pair.first);
    }
    return itemIds;
}
#ifndef __ITEM_H__
#define __ITEM_H__

#include <string>

class Item {
public:
    virtual ~Item() {}

    virtual std::string getName() const = 0;

    virtual std::string getSpriteName() const = 0;

    virtual int getSellPrice() const = 0;

    virtual void setSellPrice(int price) = 0;

    virtual std::string getItemType() const = 0;

    virtual std::string getDescription() const { return ""; }

    virtual bool isStackable() const { return true; }

    virtual int getMaxStack() const { return 99; }

    virtual int getQuality() const { return 1; }
};

#endif // __ITEM_H__
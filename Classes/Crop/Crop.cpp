#include "Crop.h"

Crop::Crop(int days)
    : growDays(days), currentDay(1), wateredToday(false)
{
}

void Crop::water()
{
    wateredToday = true;
}

void Crop::onDayPass()
{
    if (currentDay < growDays)
        currentDay++;

    wateredToday = false;
}

bool Crop::isMature() const
{
    return currentDay >= growDays;
}

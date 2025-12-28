#include "ParsnipCrop.h"


ParsnipCrop::ParsnipCrop()
    : currentDay(1), wateredToday(false)
{
}

void ParsnipCrop::water()
{
    wateredToday = true;
}

void ParsnipCrop::onDayPass()
{
    if (currentDay < 4)
        currentDay++;

    wateredToday = false; // 新一天重置
}

bool ParsnipCrop::isMature() const
{
    return currentDay >= 4;
}

int ParsnipCrop::getCurrentTileGID() const
{
    return calculateTileGID();
}

int ParsnipCrop::calculateTileGID() const
{
    switch (currentDay)
    {
        case 1:
            return wateredToday ? PARSNIP_STAGE_FIRST_WATERED
                : PARSNIP_STAGE_FIRST;
        case 2:
            return wateredToday ? PARSNIP_STAGE_SECOND_WATERED
                : PARSNIP_STAGE_SECOND;
        case 3:
            return wateredToday ? PARSNIP_STAGE_THIRD_WATERED
                : PARSNIP_STAGE_THIRD;
        case 4:
        default:
            return PARSNIP_STAGE_FOURTH;
    }
}

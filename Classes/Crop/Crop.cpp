#include "Crop.h"

Crop::Crop(int days)
    : growDays(days), currentDays(0) {
}

void Crop::OnDayPass() {
    if (currentDays < growDays)
        currentDays++;
}

bool Crop::IsMature() const {
    return currentDays >= growDays;
}

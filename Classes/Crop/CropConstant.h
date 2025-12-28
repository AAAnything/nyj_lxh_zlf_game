#pragma once

#include "cocos2d.h"
#include <string>
#include <array>
#include <unordered_map>

// Crop
constexpr int PARSNIP_STAGE_FIRST = 5; // day 1
constexpr int PARSNIP_STAGE_FIRST_WATERED = 7; // day 1 watered
constexpr int PARSNIP_STAGE_SECOND = 9; // day 2
constexpr int PARSNIP_STAGE_SECOND_WATERED = 11; // day 2 watered
constexpr int PARSNIP_STAGE_THIRD = 13; // day 3
constexpr int PARSNIP_STAGE_THIRD_WATERED = 16; // day 3 WATERED
constexpr int PARSNIP_STAGE_FOURTH = 18; // day 4: 4 days to mature

constexpr int CORN_STAGE_FIRST = 23; // day 1
constexpr int CORN_STAGE_FIRST_WATERED = 25; // day 1 WATERED
constexpr int CORN_STAGE_SECOND = 29;// day 2, 3
constexpr int CORN_STAGE_SECOND_WATERED = 30;// day 2, 3 WATERED
constexpr int CORN_STAGE_THIRD = 34;// day 4, 5
constexpr int CORN_STAGE_THIRD_WATERED = 35;// day 4, 5 WATERED
constexpr int CORN_STAGE_FOURTH = 32; // day 6: 6 days in total

constexpr int CARROT_STAGE_FIRST = 37; // day 1
constexpr int CARROT_STAGE_FIRST_WATERED = 38; // day 1 WATERED
constexpr int CARROT_STAGE_SECOND = 41;  // day 2
constexpr int CARROT_STAGE_SECOND_WATERED = 44;  // day 2 WATERED
constexpr int CARROT_STAGE_THIRD = 46;  // day 3 MATURE

//SEED
constexpr char DOGBANE_SEED[] = "tools/dogbaneSeed.png";
constexpr char CARROT_SEED[] = "tools/carrotSeed.png";
constexpr char CORN_SEED[] = "tools/cornSeed.png";
constexpr char FERTILIZER[] = "tools/fertilizer.png";

// DateManage
const std::unordered_map<std::string, int>
SEASONINDEX = { {"Spring", 0}, {"Summer", 1}, {"Fall", 2}, {"Winter", 3} }; // 季节的索�?
const std::string SEASONNAME[] = { "Spring", "Summer", "Fall", "Winter" };  // 季节的名�?
constexpr int DAYSINSEASON = 28;											// 每个季节的天�?
constexpr int DAYSINYEAR = 112;												// 每年的天数（一�?季，每季28天）
constexpr int DAYSINWEEK = 7;												// 每周的天�?
constexpr char SPRING_CONVERSATION[] = "Celebrate the arrival of Spring with games, food, and fun!\n";
constexpr char SUMMER_CONVERSATION[] = "The hot days of Summer are here! Time for the beach!\n";
constexpr char FALL_CONVERSATION[] = "Let's picking up the falling leaves!\n";
constexpr char WINTER_CONVERSATION[] = "Merry Christmas and Happy Birthday to levi!\n";

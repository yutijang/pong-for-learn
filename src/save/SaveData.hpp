#pragma once

#include <string>

// Dữ liêu save thật
struct SaveData {
    std::string currentLevel;

    float playerX{};
    float playerY{};

    int remainingTime{};
};

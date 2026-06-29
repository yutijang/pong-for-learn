#pragma once

#include <cstdint>
#include <ctime>
#include <string>

enum class SaveSlotState : std::uint8_t { Empty, Valid, Corrupted };

// Phục vụ cho UI
struct SaveSlotInfo {
    // Định danh slot save.
    int slotId{};

    SaveSlotState state{SaveSlotState::Empty};

    // Cho biết slot có dữ liệu hay chưa.
    // Ví dụ:
    // Slot 1: Có save
    // Slot 2: Trống
    // Slot 3: Có save
    bool occupied{};

    // Tên hiển thị section
    // Ví dụ:
    // Before Final Boss
    // Chapter 1
    // Dungeon Entrance
    // std::string saveName;

    // Level/map đang lưu.
    // Ví dụ:
    // currentLevel = "dungeon01";
    // currentLevel = "level1";
    std::string currentLevel;

    // Thời điểm save gần nhất.
    // Ví dụ: 2026-06-24 16:30
    // Menu có thể hiển thị:
    // Slot 1
    // Level1
    // Saved: 2026-06-24 16:30
    std::time_t lastSaveTime{};
};

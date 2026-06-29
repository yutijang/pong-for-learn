#pragma once

#include <cstdint>
#include <expected>
#include <raylib.h>

struct SpawnPoint {
    float x{};
    float y{};
};

struct PaddleConfig {
    float width{};
    float height{};
    float speed{};
};

struct BallConfig {
    float radius{};
    float speed{};
};

// LevelData chỉ chứa dữ liệu cấu hình,
// còn Entity chỉ sử dụng dữ liệu đã được cấu hình cho nó.
struct LevelData {
    PaddleConfig player;
    PaddleConfig enemy;
    BallConfig ball;

    int winningScore{};
};

enum class LoadError : std::uint8_t { OpenFileFailed, FileStructWrong };
using LoadResult = std::expected<LevelData, LoadError>;

struct LevelResources {
    Font const& scoreFont;
    Font const& countdownFont;
};

enum class LevelEvent : std::uint8_t { WallHit, PaddleHit, WhenScored, GameSetWin };

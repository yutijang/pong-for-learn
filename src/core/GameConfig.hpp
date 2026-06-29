#pragma once

// #include <cstdint>

#include <raylib.h>

namespace GameConfig {

inline constexpr int WINDOW_WIDTH{1280};
inline constexpr int WINDOW_HEIGHT{720};
inline constexpr char const* TITLE{"Pong"};
inline constexpr int FPS{60};

namespace Text {

inline constexpr char const* PAUSED{"PAUSED"};
inline constexpr char const* RESUME{"Resume"};
inline constexpr char const* BACK_TO_MAIN_MENU{"Back To Main Menu"};

} // namespace Text

namespace ArenaConstants {

inline constexpr float const WALL_PADDING_H{50.0F};
inline constexpr float const WALL_PADDING_V{30.0F};
// Padding from the guide line to the horizontal wall edge
inline constexpr float const GUIDE_LINE_PADDING{80.0F};
inline constexpr int const GUIDE_LINE_WIDTH{8};
inline constexpr int const GUIDE_LINE_SPACESIZE{2};
inline constexpr float const WALL_THICKNESS{10.0F};

inline constexpr Color const GUIDE_LINE_COLOR{.r = 97, .g = 111, .b = 121, .a = 255};
inline constexpr Color const WALL_COLOR{.r = 97, .g = 111, .b = 121, .a = 255};

// Score board
inline constexpr float const SCOREBOARD_ROUNDNESS{1.0F}; // Value 0.0F -> 1.0F
inline constexpr int const SCOREBOARD_SEGMENTS{8};
inline constexpr int const SCOREBOARD_BORDER_THICKNESS{3};
inline constexpr Color const SCOREBOARD_BORDER_COLOR{.r = 141, .g = 252, .b = 251, .a = 255};
inline constexpr Color const SCOREBOARD_BG_COLOR{.r = 106, .g = 112, .b = 189, .a = 255};

inline constexpr float const SCOREBOARD_PADDING_TOP{20.0F};
inline constexpr float const SCOREBOARD_WIDTH{120.0F};
inline constexpr float const SCOREBOARD_HEIGHT{40.0F};
inline constexpr Rectangle const SCOREBOARD_SIZE{.x = (WINDOW_WIDTH / 2.0F) -
                                                      (SCOREBOARD_WIDTH / 2.0F),
                                                 .y = SCOREBOARD_PADDING_TOP,
                                                 .width = SCOREBOARD_WIDTH,
                                                 .height = SCOREBOARD_HEIGHT};

inline constexpr Rectangle const ARENA_COURT{
    .x = 50.0F,
    .y = (SCOREBOARD_PADDING_TOP * 2.0F) + SCOREBOARD_HEIGHT,
    .width = WINDOW_WIDTH - 100.0F,
    .height = WINDOW_HEIGHT - (((SCOREBOARD_PADDING_TOP * 2.0F) + SCOREBOARD_HEIGHT) * 2.0F)};
inline constexpr int const ARENA_BORDER_THICKNESS{2};
inline constexpr Color const ARENA_BORDER_COLOR{DARKBLUE};
inline constexpr Color const ARENA_BG_COLOR{.r = 99, .g = 180, .b = 255, .a = 255};

inline constexpr float const WALL_WIDTH{ARENA_COURT.width - (WALL_PADDING_H * 2.0F)};

inline constexpr float const GUIDE_LINE_HEIGHT{
    (ARENA_COURT.height - (WALL_PADDING_V * 2.0F) - (WALL_THICKNESS * 2.0F))};

// Offset from horizontal border relative to arena court
inline constexpr float const COURT_HORIZONTAL_EDGE_OFFSET{65.0F};

inline constexpr float const TOP_LIMIT{ARENA_COURT.y + WALL_PADDING_V + WALL_THICKNESS};
inline constexpr float const BOTTOM_LIMIT{ARENA_COURT.y + ARENA_COURT.height - WALL_PADDING_V -
                                          WALL_THICKNESS};

} // namespace ArenaConstants

// enum class ResetTextPos : std::uint8_t { TopLeft, TopRight, BottomRight, BottomLeft };
// constexpr float RESET_TEXT_PADDING{20.0F};

}; // namespace GameConfig

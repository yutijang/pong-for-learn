#pragma once

#include <cstdint>

enum class SceneId : std::uint8_t { None, Menu, Settings, Gameplay, GameOver, Credits, Exit };

struct SceneRequest {
    SceneId target{SceneId::None};
};

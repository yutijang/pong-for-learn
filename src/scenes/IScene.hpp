#pragma once

#include "SceneRequest.hpp"

#include <optional>

class IScene {
  public:
    virtual ~IScene() = default;
    virtual void processInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;

    virtual void onEnter() {}

    virtual void onExit() {}

    [[nodiscard]]
    virtual std::optional<SceneRequest> consumeSceneRequest() = 0;
};

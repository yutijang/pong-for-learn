#pragma once

#include "world/LevelData.hpp"

#include <functional>
#include <utility>

class ILevel {
  public:
    virtual ~ILevel() = default;

    virtual void processInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;

    virtual void onEnter() {}

    virtual void onExit() {}

    void onLevelEvent(std::function<void(LevelEvent)> callback) {
        m_onLevelEvent = std::move(callback);
    }

  protected:
    // Tiện ích phát event thay vì gọi:
    // if (m_onEventChangedCallback) {
    //     m_onEventChangedCallback(LevelEvent::GameSetWin);
    // }
    void emitLevelEvent(LevelEvent event) {
        if (m_onLevelEvent) {
            m_onLevelEvent(event);
        }
    }

  private:
    std::function<void(LevelEvent)> m_onLevelEvent;
};

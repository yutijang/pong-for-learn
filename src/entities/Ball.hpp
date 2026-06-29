#pragma once

#include "entities/IEntity.hpp"
#include "world/LevelData.hpp"

#include <raylib.h>
#include <raymath.h>

class Ball final : public IEntity {
  public:
    Ball() = default;

    void update(float deltaTime) override;
    void draw() const override;

    void reset(SpawnPoint const& spawnPoint) noexcept {
        m_center = {.x = spawnPoint.x, .y = spawnPoint.y};
        m_velocity = {.x = 0.0F, .y = 0.0F};
    }

    [[nodiscard]] Vector2 center() const noexcept {
        return m_center;
    }

    [[nodiscard]] Vector2 velocity() const noexcept {
        return m_velocity;
    }

    [[nodiscard]] float radius() const noexcept {
        return m_radius;
    }

    [[nodiscard]] float moveSpeed() const noexcept {
        return m_moveSpeed;
    }

    [[nodiscard]] float left() const noexcept {
        return m_center.x - m_radius;
    }

    [[nodiscard]] float right() const noexcept {
        return m_center.x + m_radius;
    }

    [[nodiscard]] float top() const noexcept {
        return m_center.y - m_radius;
    }

    [[nodiscard]] float bottom() const noexcept {
        return m_center.y + m_radius;
    }

    void setCenter(Vector2 center) noexcept {
        m_center = center;
    }

    void setVelocity(Vector2 velocity) noexcept {
        m_velocity = velocity;
    }

    void setMoveSpeed(float moveSpeed) noexcept {
        m_moveSpeed = moveSpeed;
    }

    void setRadius(float radius) noexcept {
        m_radius = radius;
    }

    void setColor(Color color) noexcept {
        m_color = color;
    }

    void launch() noexcept;

  private:
    Vector2 m_center{};
    Vector2 m_velocity{};
    float m_moveSpeed{};
    float m_radius{};
    Color m_color{RAYWHITE};
};

#include "entities/IEntity.hpp"
#include "world/LevelData.hpp"

#include <algorithm>
#include <raylib.h>

class Player final : public IEntity {
  public:
    Player() = default;

    void processInput();

    void update(float deltaTime) override;
    void draw() const override;

    void reset(SpawnPoint const& spawnPosition) noexcept {
        m_position = {.x = spawnPosition.x, .y = spawnPosition.y};
        m_velocity = {.x = 0.0F, .y = 0.0F};
    }

    [[nodiscard]] Vector2 position() const noexcept {
        return m_position;
    }

    [[nodiscard]] Vector2 velocity() const noexcept {
        return m_velocity;
    }

    [[nodiscard]] Rectangle bounds() const noexcept {
        return {.x = m_position.x, .y = m_position.y, .width = m_width, .height = m_height};
    }

    [[nodiscard]] float moveSpeed() const noexcept {
        return m_moveSpeed;
    }

    void setPosition(Vector2 position) noexcept {
        m_position = position;
    }

    void setVelocity(Vector2 velocity) noexcept {
        m_velocity = velocity;
    }

    void setSize(float width, float height) noexcept {
        m_width = width;
        m_height = height;
    }

    void setMoveSpeed(float moveSpeed) noexcept {
        m_moveSpeed = moveSpeed;
    }

    void setColor(Color color) noexcept {
        m_color = color;
    }

    void clampY(float minY, float maxY) noexcept {
        m_position.y = std::clamp(m_position.y, minY, maxY);
    }

  private:
    Vector2 m_position{};
    Vector2 m_velocity{};
    float m_moveSpeed{};
    float m_width{};
    float m_height{};
    Color m_color{MAROON};
};

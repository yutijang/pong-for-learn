#include "entities/Enemy.hpp"

#include "core/GameConfig.hpp"
#include "entities/Ball.hpp"

#include <algorithm>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

namespace {
// khoảng sai số để tránh Paddle rung liên tục khi Ball gần trùng tâm.
constexpr float const DEAD_ZONE = 5.0F;
constexpr float const ARENA_CENTER_Y{
    (GameConfig::ArenaConstants::TOP_LIMIT + GameConfig::ArenaConstants::BOTTOM_LIMIT) / 2.0F};
} // namespace

void Enemy::processAI(Ball const& ball) {
    if (ball.center().x > GameConfig::WINDOW_WIDTH / 2.0F) {
        m_velocity.y = 0.0F;
        return;
    }

    float const paddleCenterY = m_position.y + (m_height / 2.0F);

    // Sau khi đánh bóng, di chuyển chậm về tâm của sân
    // lấy trục y của tâm làm điểm đến (so sánh)
    if (ball.velocity().x > 0.0F) {
        float const delta = ARENA_CENTER_Y - paddleCenterY;

        float velocity = delta * 1.5F;
        velocity = std::clamp(velocity, -m_moveSpeed, m_moveSpeed);
        m_velocity.y = velocity;
    } else {
        // di chuyển theo tâm bóng
        float const delta = ball.center().y - paddleCenterY;

        if (delta < -DEAD_ZONE) {
            m_velocity.y = -m_moveSpeed;
        } else if (delta > DEAD_ZONE) {
            m_velocity.y = m_moveSpeed;
        } else if (std::abs(delta) <= DEAD_ZONE) {
            m_velocity.y = 0;
        }
    }
}

void Enemy::update(float deltaTime) {
    m_position = Vector2Add(m_position, Vector2Scale(m_velocity, deltaTime));
}

void Enemy::draw() const {
    DrawRectangleV(m_position, {.x = m_width, .y = m_height}, m_color);
}

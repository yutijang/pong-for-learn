#include "entities/Ball.hpp"

#include <cmath>
#include <raylib.h>
#include <raymath.h>

void Ball::update(float deltaTime) {
    m_center = Vector2Add(m_center, Vector2Scale(m_velocity, deltaTime));
}

void Ball::draw() const {
    DrawCircleV(m_center, m_radius, m_color);
}

void Ball::launch() noexcept {
    // Tạo vector hướng (direction) (unit vector) ngẫu nhiên, [-1, 1]
    // tạo hướng ngẫu nhiên trái/phải và giới hạn góc hẹp về 2 phía paddle
    // tránh hướng chỉ đi lên/xuống đến top/bottom wall
    int launchAngleDeg{};
    if (int const valRanLeftOrRight = GetRandomValue(0, 1); valRanLeftOrRight == 0) {
        // giảm trường hợp ball đi thẳng đến paddle
        if (int const reduceToPaddle = GetRandomValue(0, 1); reduceToPaddle == 0) {
            launchAngleDeg = GetRandomValue(-45, -15);
        } else {
            launchAngleDeg = GetRandomValue(15, 45);
        }
    } else {
        if (int const reduceToPaddle = GetRandomValue(0, 1); reduceToPaddle == 0) {
            launchAngleDeg = GetRandomValue(-225, -195);
        } else {
            launchAngleDeg = GetRandomValue(195, 225);
        }
    }
    // chuyển sang radian
    float const angleRad = static_cast<float>(launchAngleDeg) * DEG2RAD;
    // tạo vector hướng
    Vector2 direction{.x = std::cos(angleRad), .y = std::sin(angleRad)};

    // Tạo vận tốc (velocity) cuối cùng (velocity = speed * direction)
    m_velocity = Vector2Scale(direction, m_moveSpeed);
}

#include "world/GameLevel.hpp"

#include "core/GameConfig.hpp"
#include "ui/Label.hpp"
#include "world/LevelData.hpp"

#include <format>
#include <raylib.h>
#include <raymath.h>
#include <string>

namespace {
using GameConfig::WINDOW_HEIGHT;
using GameConfig::WINDOW_WIDTH;
using GameConfig::ArenaConstants::ARENA_BG_COLOR;
using GameConfig::ArenaConstants::ARENA_BORDER_COLOR;
using GameConfig::ArenaConstants::ARENA_BORDER_THICKNESS;
using GameConfig::ArenaConstants::ARENA_COURT;
using GameConfig::ArenaConstants::BOTTOM_LIMIT;
using GameConfig::ArenaConstants::COURT_HORIZONTAL_EDGE_OFFSET;
using GameConfig::ArenaConstants::GUIDE_LINE_COLOR;
using GameConfig::ArenaConstants::GUIDE_LINE_HEIGHT;
using GameConfig::ArenaConstants::GUIDE_LINE_PADDING;
using GameConfig::ArenaConstants::GUIDE_LINE_SPACESIZE;
using GameConfig::ArenaConstants::GUIDE_LINE_WIDTH;
using GameConfig::ArenaConstants::SCOREBOARD_BG_COLOR;
using GameConfig::ArenaConstants::SCOREBOARD_BORDER_COLOR;
using GameConfig::ArenaConstants::SCOREBOARD_BORDER_THICKNESS;
using GameConfig::ArenaConstants::SCOREBOARD_ROUNDNESS;
using GameConfig::ArenaConstants::SCOREBOARD_SEGMENTS;
using GameConfig::ArenaConstants::SCOREBOARD_SIZE;
using GameConfig::ArenaConstants::TOP_LIMIT;
using GameConfig::ArenaConstants::WALL_COLOR;
using GameConfig::ArenaConstants::WALL_PADDING_H;
using GameConfig::ArenaConstants::WALL_PADDING_V;
using GameConfig::ArenaConstants::WALL_THICKNESS;
using GameConfig::ArenaConstants::WALL_WIDTH;

constexpr float const WALL_LEFT_EDGE{ARENA_COURT.x + WALL_PADDING_H};
constexpr float const WALL_RIGHT_EDGE{ARENA_COURT.x + ARENA_COURT.width - WALL_PADDING_H};

constexpr float const ANGLE_OFFSET{8.0F};
constexpr float const COUNTDOWN_POS_Y{140.0F};
} // namespace

GameLevel::GameLevel(LevelData const& levelData, LevelResources const& resource)
    : m_levelData(levelData), m_resource(resource) {
    m_player.setSize(m_levelData.player.width, m_levelData.player.height);
    m_player.setMoveSpeed(m_levelData.player.speed);

    m_enemy.setSize(m_levelData.enemy.width, m_levelData.enemy.height);
    m_enemy.setMoveSpeed(m_levelData.enemy.speed);

    m_ball.setRadius(m_levelData.ball.radius);
    m_ball.setMoveSpeed(m_levelData.ball.speed);

    m_scoreLbl.emplace(Label::createCenteredLabel(m_resource.scoreFont, "0 - 0", 19.0F));

    m_countdownLbl.emplace(
        Label::createCenteredLabel(m_resource.countdownFont, "3", COUNTDOWN_POS_Y));
}

void GameLevel::processInput() {
    m_player.processInput();
    m_enemy.processAI(m_ball);
}

void GameLevel::update(float deltaTime) {
    if (m_state == LevelState::Countdown) {
        float const previousTimer = m_countdownTimer;

        m_countdownTimer -= deltaTime;

        if (m_countdownTimer > 1.0F) {
            m_countdownLbl->setText(std::to_string(static_cast<int>(m_countdownTimer)));
            float const exactNumCenterX = Label::calculateCenteredX(
                m_resource.countdownFont, std::to_string(static_cast<int>(m_countdownTimer)));
            m_countdownLbl->setPosition({.x = exactNumCenterX, .y = COUNTDOWN_POS_Y});
        }

        if (previousTimer > 1.0F && m_countdownTimer <= 1.0F) {
            m_countdownLbl->setColor(MAGENTA);
            m_countdownLbl->setText("GO!");

            float const exactCenterX = Label::calculateCenteredX(m_resource.countdownFont, "GO!");
            m_countdownLbl->setPosition({.x = exactCenterX, .y = COUNTDOWN_POS_Y});
        }

        if (m_countdownTimer <= 0.0F) {
            m_state = LevelState::Playing;
            m_countdownTimer = 4.0F;
        }

        return;
    }

    m_player.update(deltaTime);
    m_enemy.update(deltaTime);
    m_ball.update(deltaTime);

    m_player.clampY(TOP_LIMIT, BOTTOM_LIMIT - m_levelData.player.height);
    m_enemy.clampY(TOP_LIMIT, BOTTOM_LIMIT - m_levelData.enemy.height);

    handleWallCollision();
    handlePaddleCollision();
    handleGoal();
}

void GameLevel::draw() const {
    drawArena();

    m_player.draw();
    m_enemy.draw();
    m_ball.draw();

    drawScoreBoard();
    m_scoreLbl->draw();

    if (m_state == LevelState::Countdown) {
        m_countdownLbl->draw();
    }
}

void GameLevel::onEnter() {
    // set Players and Ball spawn position
    m_player.reset(
        {.x = ARENA_COURT.x + ARENA_COURT.width - COURT_HORIZONTAL_EDGE_OFFSET -
              m_levelData.player.width,
         .y = (ARENA_COURT.height / 2.0F) + ARENA_COURT.y - (m_levelData.player.height / 2.0F)});

    m_enemy.reset(
        {.x = ARENA_COURT.x + COURT_HORIZONTAL_EDGE_OFFSET,
         .y = (ARENA_COURT.height / 2.0F) + ARENA_COURT.y - (m_levelData.enemy.height / 2.0F)});

    m_ball.reset({.x = WINDOW_WIDTH / 2.0F, .y = WINDOW_HEIGHT / 2.0F});

    m_ball.launch();
}

void GameLevel::onExit() {}

void GameLevel::drawArena() {
    // Draw level background
    DrawRectangleGradientV(0,
                           0,
                           WINDOW_WIDTH,
                           WINDOW_HEIGHT,
                           {.r = 146, .g = 215, .b = 215, .a = 255},
                           {.r = 146, .g = 171, .b = 214, .a = 255});

    // Draw Arena background
    drawArenaCourt();

    // Draw top wall
    drawWall(WallPosition::Top);

    drawCenterSectionArenaCourt();

    // Draw border line
    drawGuideLineBorder(GuidelinePosition::Left);
    drawGuideLineBorder(GuidelinePosition::Right);

    // Draw bottom wall
    drawWall(WallPosition::Bottom);
}

void GameLevel::drawWall(WallPosition position) {
    float posY{};

    if (position == WallPosition::Top) {
        posY = ARENA_COURT.y + WALL_PADDING_V;
    } else {
        posY = ARENA_COURT.y + ARENA_COURT.height - WALL_PADDING_V - WALL_THICKNESS;
    }

    float const posX = ARENA_COURT.x + WALL_PADDING_H;

    DrawRectangleV({.x = posX, .y = posY}, {.x = WALL_WIDTH, .y = WALL_THICKNESS}, WALL_COLOR);
}

void GameLevel::drawGuideLineBorder(GuidelinePosition position) {
    float posXStart{};

    if (position == GuidelinePosition::Left) {
        posXStart = ARENA_COURT.x + GUIDE_LINE_PADDING;
    } else {
        posXStart = ARENA_COURT.x + ARENA_COURT.width - GUIDE_LINE_PADDING;
    }

    float const posYStart{ARENA_COURT.y + WALL_PADDING_V + WALL_THICKNESS};

    float const posYEnd{posYStart + GUIDE_LINE_HEIGHT};

    DrawLineDashed({.x = posXStart, .y = posYStart},
                   {.x = posXStart, .y = posYEnd},
                   GUIDE_LINE_WIDTH,
                   GUIDE_LINE_SPACESIZE,
                   GUIDE_LINE_COLOR);
}

void GameLevel::drawScoreBoard() {
    DrawRectangleRounded(
        SCOREBOARD_SIZE, SCOREBOARD_ROUNDNESS, SCOREBOARD_SEGMENTS, SCOREBOARD_BG_COLOR);
    DrawRectangleRoundedLinesEx(SCOREBOARD_SIZE,
                                SCOREBOARD_ROUNDNESS,
                                SCOREBOARD_SEGMENTS,
                                SCOREBOARD_BORDER_THICKNESS,
                                SCOREBOARD_BORDER_COLOR);
}

void GameLevel::drawArenaCourt() {
    DrawRectangleRec(ARENA_COURT, ARENA_BG_COLOR);
    DrawRectangleLinesEx(ARENA_COURT, ARENA_BORDER_THICKNESS, ARENA_BORDER_COLOR);
}

void GameLevel::drawCenterSectionArenaCourt() {
    constexpr float const startPosX{(WINDOW_WIDTH / 2.0F)};
    constexpr Color const color{.r = 25, .g = 25, .b = 25, .a = 15};

    constexpr float const radius{100.0F};
    constexpr float const thickness{20.0F};
    constexpr float const innerRadius{radius - (thickness / 2.0F)};
    constexpr float const outerRadius{radius + (thickness / 2.0F)};
    constexpr int const segments{0};

    constexpr Vector2 const center{.x = WINDOW_WIDTH / 2.0F,
                                   .y = (ARENA_COURT.height / 2.0F) + ARENA_COURT.y};

    // Các mốc tọa độ giới hạn sân
    float const arenaTopY = ARENA_COURT.y + WALL_PADDING_V + WALL_THICKNESS;

    float const arenaBottomY = ARENA_COURT.y + ARENA_COURT.height - WALL_PADDING_V - WALL_THICKNESS;

    // Tính toán các mốc tọa độ va chạm
    float const circleOuterTopY = (center.y - outerRadius);
    float const circleInnerTopY = (center.y - innerRadius);
    float const circleInnerBottomY = (center.y + innerRadius);
    float const circleOuterBottomY = (center.y + outerRadius);

    // Từ đỉnh sân đến biên ngoài phía trên của vòng tròn
    DrawLineEx(
        {.x = startPosX, .y = arenaTopY}, {.x = startPosX, .y = circleOuterTopY}, 18.0F, color);

    // Từ biên trong phía trên đến biên trong phía dưới của vòng tròn
    DrawLineEx({.x = startPosX, .y = circleInnerTopY},
               {.x = startPosX, .y = circleInnerBottomY},
               18.0F,
               color);

    // Từ biên ngoài phía dưới của vòng tròn đến wall
    DrawLineEx({.x = startPosX, .y = circleOuterBottomY},
               {.x = startPosX, .y = arenaBottomY},
               18.0F,
               color);

    // Vẽ vòng tròn giữa sân
    DrawRing(center, innerRadius, outerRadius, 0.0F, 360.0F, segments, color);
}

void GameLevel::handleGoal() {
    if (m_ball.right() < 0) {
        playerScored();
    } else if (m_ball.left() > WINDOW_WIDTH) {
        enemyScored();
    }
}

void GameLevel::updateScoreLabel() {
    m_scoreLbl->setText(std::format("{} - {}", m_enemyScore, m_playerScore));
}

void GameLevel::playerScored() {
    ++m_playerScore;

    if (m_playerScore == m_levelData.winningScore) {
        resetInfoAfterWinning();

        m_state = LevelState::Countdown;

        emitLevelEvent(LevelEvent::GameSetWin);
    } else {
        emitLevelEvent(LevelEvent::WhenScored);
    }

    updateScoreLabel();

    m_ball.reset({.x = WINDOW_WIDTH / 2.0F, .y = WINDOW_HEIGHT / 2.0F});

    m_ball.launch();
}

void GameLevel::enemyScored() {
    ++m_enemyScore;

    if (m_enemyScore == m_levelData.winningScore) {
        resetInfoAfterWinning();

        m_state = LevelState::Countdown;

        emitLevelEvent(LevelEvent::GameSetWin);
    } else {
        emitLevelEvent(LevelEvent::WhenScored);
    }

    updateScoreLabel();

    m_ball.reset({.x = WINDOW_WIDTH / 2.0F, .y = WINDOW_HEIGHT / 2.0F});

    m_ball.launch();
}

void GameLevel::handleWallCollision() {
    bool isCollision{};

    // Xử lý va chạm top wall
    if (m_ball.top() <= TOP_LIMIT && m_ball.left() <= WALL_RIGHT_EDGE &&
        m_ball.right() >= WALL_LEFT_EDGE) {
        m_ball.setCenter({.x = m_ball.center().x, .y = TOP_LIMIT + m_ball.radius()});
        m_ball.setVelocity({.x = m_ball.velocity().x, .y = -m_ball.velocity().y});

        isCollision = true;
    }
    // Xử lý va chạm bottom wall
    else if (m_ball.bottom() >= BOTTOM_LIMIT && m_ball.left() <= WALL_RIGHT_EDGE &&
             m_ball.right() >= WALL_LEFT_EDGE) {
        m_ball.setCenter({.x = m_ball.center().x, .y = BOTTOM_LIMIT - m_ball.radius()});
        m_ball.setVelocity({.x = m_ball.velocity().x, .y = -m_ball.velocity().y});

        isCollision = true;
    }

    if (isCollision) {
        emitLevelEvent(LevelEvent::WallHit);
    }
}

void GameLevel::handlePaddleCollision() {
    handlePaddleCollisionImpl(m_player.bounds(), true);
    handlePaddleCollisionImpl(m_enemy.bounds(), false);
}

void GameLevel::handlePaddleCollisionImpl(Rectangle paddleBounds, bool isPlayer) {
    if (!CheckCollisionCircleRec(m_ball.center(), m_ball.radius(), paddleBounds)) {
        return;
    }

    float const ballRadius{m_ball.radius()};
    Vector2 const ballCenter{m_ball.center()};

    m_ball.setCenter({.x = isPlayer ? paddleBounds.x - ballRadius
                                    : paddleBounds.x + paddleBounds.width + ballRadius,
                      .y = ballCenter.y});

    float const oneThirdPaddle{paddleBounds.height / 3.0F};
    float const paddleTopAreaEnd{paddleBounds.y + oneThirdPaddle};
    float const paddleMiddleAreaEnd{paddleBounds.y + (2 * oneThirdPaddle)};

    Vector2 const currentDirection = Vector2Normalize(m_ball.velocity());
    Vector2 const reflectedDirection{.x = -currentDirection.x, .y = currentDirection.y};

    if (ballCenter.y <= paddleTopAreaEnd) {
        // Vùng trên
        Vector2 const newDirection = Vector2Rotate(reflectedDirection, ANGLE_OFFSET * DEG2RAD);
        m_ball.setVelocity(Vector2Scale(newDirection, m_ball.moveSpeed()));
    } else if (ballCenter.y <= paddleMiddleAreaEnd) {
        // Vùng giữa - không thay đổi góc
        m_ball.setVelocity({.x = -m_ball.velocity().x, .y = m_ball.velocity().y});
    } else {
        // Vùng dưới
        Vector2 const newDirection = Vector2Rotate(reflectedDirection, ANGLE_OFFSET * DEG2RAD);
        m_ball.setVelocity(Vector2Scale(newDirection, m_ball.moveSpeed()));
    }

    emitLevelEvent(LevelEvent::PaddleHit);
}

void GameLevel::resetInfoAfterWinning() {
    m_playerScore = 0;
    m_enemyScore = 0;

    m_countdownLbl->setText("");
    m_countdownLbl->setColor(RAYWHITE);
    float const exactCenterX = Label::calculateCenteredX(
        m_resource.countdownFont, std::to_string(static_cast<int>(m_countdownTimer)));
    m_countdownLbl->setPosition({.x = exactCenterX, .y = COUNTDOWN_POS_Y});
}

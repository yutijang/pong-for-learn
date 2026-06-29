#pragma once

#include "entities/Ball.hpp"
#include "entities/Enemy.hpp"
#include "entities/Player.hpp"
#include "ui/Label.hpp"
#include "world/ILevel.hpp"
#include "world/LevelData.hpp"

#include <cstdint>
#include <optional>
#include <raylib.h>

class GameLevel final : public ILevel {
  public:
    explicit GameLevel(LevelData const& levelData, LevelResources const& resource);

    void processInput() override;

    void update(float deltaTime) override;
    void draw() const override;

    void onEnter() override;
    void onExit() override;

  private:
    enum class GuidelinePosition : std::uint8_t { Left, Right };
    enum class WallPosition : std::uint8_t { Top, Bottom };
    enum class LevelState : std::uint8_t { Playing, Countdown };

    static void drawArena();
    static void drawArenaCourt();
    static void drawCenterSectionArenaCourt();
    static void drawScoreBoard();

    static void drawWall(WallPosition position);
    static void drawGuideLineBorder(GuidelinePosition position);

    void handleWallCollision();

    void handlePaddleCollision();
    void handlePaddleCollisionImpl(Rectangle paddleBounds, bool isPlayer);

    void handleGoal();

    void updateScoreLabel();
    void playerScored();
    void enemyScored();

    void resetInfoAfterWinning();

    Player m_player;
    Enemy m_enemy;
    Ball m_ball;
    std::optional<Label> m_scoreLbl;

    LevelResources m_resource;

    LevelData m_levelData;
    int m_playerScore{};
    int m_enemyScore{};

    float m_countdownTimer{4.0F};
    std::optional<Label> m_countdownLbl;
    LevelState m_state{LevelState::Countdown};
};

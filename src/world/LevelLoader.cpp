#include "world/LevelLoader.hpp"

#include "utility/utilities.hpp"
#include "world/LevelData.hpp"

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

LoadResult LevelLoader::load(std::filesystem::path const& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return std::unexpected(LoadError::OpenFileFailed);
    }

    std::ifstream read(filePath);
    if (!read.is_open()) {
        return std::unexpected{LoadError::OpenFileFailed};
    }

    LevelData cfg;

    std::string line;
    while (std::getline(read, line)) {
        if (line.empty() || line.starts_with('#')) {
            continue;
        }

        auto const pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string_view const key = utility::strings::trim(std::string_view{line}.substr(0, pos));
        std::string_view const value =
            utility::strings::trim(std::string_view{line}.substr(pos + 1));

        auto checkExp = applyConfig(cfg, key, value);
        if (!checkExp) {
            return std::unexpected{checkExp.error()};
        }
    }

    return cfg;
}

std::expected<void, LoadError>
    LevelLoader::applyConfig(LevelData& cfg, std::string_view key, std::string_view value) {
    auto const parseTo = [&](auto& targetMember) -> std::expected<void, LoadError> {
        if (!utility::parsing::parseValue(value, targetMember)) {
            // Logger
            return std::unexpected{LoadError::FileStructWrong};
        }
        return {};
    };

    if (key == "winning_score") {
        return parseTo(cfg.winningScore);
    }

    if (key == "ball_radius") {
        return parseTo(cfg.ball.radius);
    }

    if (key == "ball_speed") {
        return parseTo(cfg.ball.speed);
    }

    if (key == "player_width") {
        return parseTo(cfg.player.width);
    }

    if (key == "player_height") {
        return parseTo(cfg.player.height);
    }

    if (key == "player_move_speed") {
        return parseTo(cfg.player.speed);
    }

    if (key == "enemy_width") {
        return parseTo(cfg.enemy.width);
    }

    if (key == "enemy_height") {
        return parseTo(cfg.enemy.height);
    }

    if (key == "enemy_move_speed") {
        return parseTo(cfg.enemy.speed);
    }

    return std::unexpected{LoadError::FileStructWrong};
}

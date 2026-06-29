#include "SaveSerializer.hpp"

#include "SaveData.hpp"
#include "SaveError.hpp"
#include "SaveResult.hpp"
#include "utility/utilities.hpp"

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

LoadSaveResult SaveSerializer::load(std::filesystem::path const& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return std::unexpected{SaveError::FileNotFound};
    }

    std::ifstream read{filePath};
    if (!read.is_open()) {
        return std::unexpected{SaveError::OpenFileFailed};
    }

    SaveData saveData;

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

        auto checkExpec = applyData(saveData, key, value);
        if (!checkExpec) {
            return std::unexpected{checkExpec.error()};
        }
    }

    return saveData;
}

SaveResult SaveSerializer::save(std::filesystem::path const& filePath, SaveData const& saveData) {
    std::error_code ec;

    std::filesystem::create_directories(filePath.parent_path(), ec);

    if (ec) {
        return std::unexpected{SaveError::WriteFailed};
    }

    // overwrite file exists
    std::ofstream write{filePath};
    if (!write.is_open()) {
        return std::unexpected{SaveError::WriteFailed};
    }

    write << "currentLevel=" << saveData.currentLevel << '\n';
    write << "playerX=" << saveData.playerX << '\n';
    write << "playerY=" << saveData.playerY << '\n';
    write << "remainingTime=" << saveData.remainingTime << '\n';

    if (!write.good()) {
        return std::unexpected{SaveError::WriteFailed};
    }

    return {};
}

SaveResult SaveSerializer::applyData(SaveData& sav, std::string_view key, std::string_view value) {
    auto const parseTo = [&](auto& targetMember) -> SaveResult {
        if (!utility::parsing::parseValue(value, targetMember)) {
            return std::unexpected{SaveError::InvalidFileFormat};
        }
        return {};
    };

    if (key == "currentLevel") {
        return parseTo(sav.currentLevel);
    }

    if (key == "playerX") {
        return parseTo(sav.playerX);
    }

    if (key == "playerY") {
        return parseTo(sav.playerY);
    }

    if (key == "remainingTime") {
        return parseTo(sav.remainingTime);
    }

    return std::unexpected{SaveError::InvalidFileFormat};
}

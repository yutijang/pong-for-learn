#pragma once

#include "world/LevelData.hpp"

#include <expected>
#include <filesystem>
#include <string_view>

class LevelLoader {
  public:
    [[nodiscard]]
    static LoadResult load(std::filesystem::path const& filePath);

  private:
    static std::expected<void, LoadError>
        applyConfig(LevelData& cfg, std::string_view key, std::string_view value);
};

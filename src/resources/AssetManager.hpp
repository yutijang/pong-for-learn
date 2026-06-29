#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <raylib.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class AssetManager {
  public:
    enum class AssetType : std::uint8_t { Font, Sound, Music };

    struct AssetDesc {
        std::filesystem::path path;
        AssetType type; // Khi ref về 0, cần biết giải phóng bằng UnloadFont hay UnloadSound
        int fontBaseSize;
    };

    struct AssetState {
        std::size_t refCount{0};
    };

    bool buildRegistry(std::filesystem::path const& groupsDirectory);

    bool loadAssetGroup(std::string_view groupId);
    void unloadAssetGroup(std::string_view groupId);

    [[nodiscard]] bool hasGroup(std::string_view groupId) const;

    [[nodiscard]] Font const* font(std::string_view assetId) const;
    [[nodiscard]] Sound const* sound(std::string_view assetId) const;

    [[nodiscard]] Music* music(std::string_view assetId);
    [[nodiscard]] Music const* music(std::string_view assetId) const;

    // Debug
    [[nodiscard]] std::size_t refCount(std::filesystem::path const& path) const;

    void unloadAll();

  private:
    // Helper
    enum class LineStatus : std::uint8_t { Success, Skip, CriticalError };
    LineStatus buildItemLines(std::string const& line, std::vector<std::string>& assetIds);

    // assetId phải là tên định danh duy nhất
    // assetId -> AssetDesc (asset metadata)
    std::unordered_map<std::string, AssetDesc> m_assetRegistry;
    // groupId -> danh sách assetId
    std::unordered_map<std::string, std::vector<std::string>> m_groups;

    std::unordered_map<std::filesystem::path, AssetState> m_assetStates;

    std::unordered_map<std::filesystem::path, std::unordered_map<int, Font>> m_fonts;
    std::unordered_map<std::filesystem::path, Sound> m_sounds;
    std::unordered_map<std::filesystem::path, Music> m_musics;
};

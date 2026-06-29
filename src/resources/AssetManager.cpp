#include "AssetManager.hpp"

#include "utility/utilities.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <raylib.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

bool AssetManager::buildRegistry(std::filesystem::path const& groupsDirectory) {
    if (!std::filesystem::exists(groupsDirectory)) {
        // Logger reserved
        return false;
    }

    if (!std::filesystem::is_directory(groupsDirectory)) {
        // Logger reserved
        return false;
    }

    m_assetRegistry.clear();
    m_groups.clear();

    for (auto const& entry : std::filesystem::directory_iterator(groupsDirectory)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }

        if (entry.path().extension() != ".cfg") {
            continue;
        }

        std::ifstream file{entry.path()};
        if (!file.is_open()) {
            continue;
        }

        // Get groupId by filename
        std::string const groupId = entry.path().stem().string();
        auto& assetIds = m_groups[groupId];

        // one line sample: menu_font=font:assets/fonts/roboto.ttf:32
        std::string line;
        while (std::getline(file, line)) {
            auto const status = buildItemLines(line, assetIds);
            switch (status) {
                case LineStatus::Success      : break;
                case LineStatus::Skip         : continue;
                case LineStatus::CriticalError: return false;
            }
        }
    }

    return true;
}

AssetManager::LineStatus AssetManager::buildItemLines(std::string const& line,
                                                      std::vector<std::string>& assetIds) {
    std::string_view const lineView{line};

    auto const equalPos = lineView.find('=');
    if (equalPos == std::string_view::npos) {
        return LineStatus::Skip;
    }

    // assetId: [menu_font]
    std::string_view const assetIdView = utility::strings::trim(lineView.substr(0, equalPos));
    if (assetIdView.empty()) {
        // Logger reserved
        return LineStatus::CriticalError;
    }

    // [font:assets/fonts/roboto.ttf:32]
    std::string_view const rhs = utility::strings::trim(lineView.substr(equalPos + 1));

    auto const colonPos = rhs.find(':');
    if (colonPos == std::string_view::npos) {
        return LineStatus::Skip;
    }

    // [font]
    std::string_view const typeStr = rhs.substr(0, colonPos);
    // [assets/fonts/roboto.ttf:32]
    std::string_view const pathStr = rhs.substr(colonPos + 1);
    if (pathStr.empty()) {
        // Logger reserved
        return LineStatus::CriticalError;
    }

    AssetType type{};
    int fontBaseSize{32};
    std::string_view actualPathStr = pathStr;

    if (typeStr == "font") {
        type = AssetType::Font;

        auto const fontSizeColonPos = pathStr.find_last_of(':');
        if (fontSizeColonPos != std::string_view::npos) {
            actualPathStr = pathStr.substr(0, fontSizeColonPos);

            std::string_view const sizeStr = pathStr.substr(fontSizeColonPos + 1);
            if (!sizeStr.empty()) {
                fontBaseSize = std::stoi(std::string{sizeStr});
            }
        }
    } else if (typeStr == "sound") {
        type = AssetType::Sound;
    } else if (typeStr == "music") {
        type = AssetType::Music;
    } else {
        return LineStatus::Skip;
    }

    // build AssetDesc metadata
    AssetDesc desc{
        .path = std::filesystem::path{actualPathStr},
        .type = type,
        .fontBaseSize = fontBaseSize,
    };

    std::string assetId{assetIdView};

    // assetId phải duy nhất trên toàn bộ dự án
    auto [it, inserted] = m_assetRegistry.emplace(assetId, std::move(desc));
    if (!inserted) {
        // Logger reserved
        return LineStatus::CriticalError;
    }

    assetIds.emplace_back(std::move(assetId));

    return LineStatus::Success;
}

bool AssetManager::loadAssetGroup(std::string_view groupId) {
    // Tra cứu groupId từ m_groups để lấy danh sách assetId
    auto const groupIt = m_groups.find(std::string{groupId});
    if (groupIt == m_groups.end()) {
        return false;
    }

    // Lấy danh sách assetId
    auto const& assetIds = groupIt->second;

    for (auto const& assetId : assetIds) {
        // Tra registry để lấy metadata AssetDesc bằng assetId
        auto const descIt = m_assetRegistry.find(assetId);
        if (descIt == m_assetRegistry.end()) {
            return false;
        }

        auto const& desc = descIt->second;
        auto const pathStr = desc.path.string();

        switch (desc.type) {
            case AssetType::Font: {
                auto& sizeMap = m_fonts[desc.path];

                if (!sizeMap.contains(desc.fontBaseSize)) {
                    auto fontObj = LoadFontEx(pathStr.c_str(), desc.fontBaseSize, nullptr, 0);
                    if (!IsFontValid(fontObj)) {
                        return false;
                    }

                    SetTextureFilter(fontObj.texture, TEXTURE_FILTER_BILINEAR);

                    sizeMap.emplace(desc.fontBaseSize, fontObj);
                }

                auto& state = m_assetStates[desc.path];
                ++state.refCount;

                break;
            }
            case AssetType::Sound: {
                auto& state = m_assetStates[desc.path];

                if (state.refCount > 0) {
                    ++state.refCount;
                    continue;
                }

                auto soundObj = LoadSound(pathStr.c_str());
                if (!IsSoundValid(soundObj)) {
                    return false;
                }

                m_sounds.emplace(desc.path, soundObj);

                ++state.refCount;

                break;
            }
            case AssetType::Music: {
                auto& state = m_assetStates[desc.path];

                if (state.refCount > 0) {
                    ++state.refCount;
                    continue;
                }

                auto musicObj = LoadMusicStream(pathStr.c_str());
                if (!IsMusicValid(musicObj)) {
                    return false;
                }

                m_musics.emplace(desc.path, musicObj);

                ++state.refCount;

                break;
            }
        }
    }

    return true;
}

void AssetManager::unloadAssetGroup(std::string_view groupId) {
    auto const groupIt = m_groups.find(std::string{groupId});
    if (groupIt == m_groups.end()) {
        return;
    }

    // Lấy danh sách assetId
    auto const& assetIds = groupIt->second;

    for (auto const& assetId : assetIds) {
        auto const descIt = m_assetRegistry.find(assetId);
        if (descIt == m_assetRegistry.end()) {
            continue;
        }

        auto const& desc = descIt->second;

        auto const stateIt = m_assetStates.find(desc.path);
        if (stateIt == m_assetStates.end()) {
            continue;
        }
        auto& state = stateIt->second;

        if (state.refCount == 0) {
            continue;
        }

        --state.refCount;

        if (state.refCount > 0) {
            continue;
        }

        switch (desc.type) {
            case AssetType::Font: {
                auto const fontIt = m_fonts.find(desc.path);
                if (fontIt == m_fonts.end()) {
                    continue;
                }

                auto& sizeMap = fontIt->second;

                for (auto const& [size, fontObj] : sizeMap) {
                    UnloadFont(fontObj);
                }

                m_fonts.erase(fontIt);

                break;
            }
            case AssetType::Sound: {
                auto const soundIt = m_sounds.find(desc.path);
                if (soundIt == m_sounds.end()) {
                    continue;
                }

                UnloadSound(soundIt->second);
                m_sounds.erase(soundIt);

                break;
            }
            case AssetType::Music: {
                auto const musicIt = m_musics.find(desc.path);
                if (musicIt == m_musics.end()) {
                    continue;
                }

                UnloadMusicStream(musicIt->second);
                m_musics.erase(musicIt);

                break;
            }
        }

        m_assetStates.erase(stateIt);
    }
}

bool AssetManager::hasGroup(std::string_view groupId) const {
    auto const groupIt = m_groups.find(std::string{groupId});
    return groupIt != m_groups.end();
}

Font const* AssetManager::font(std::string_view assetId) const {
    // Tra cứu path (AssetDesc) trong registry bằng assetId
    auto const assetIt = m_assetRegistry.find(std::string{assetId});
    if (assetIt == m_assetRegistry.end()) {
        // Logger reserved
        return nullptr;
    }

    auto const& desc = assetIt->second;

    // Guard: Kiểm tra type của asset tìm thấy có đúng type::Font hay không
    if (desc.type != AssetType::Font) {
        // Logger reserved
        return nullptr;
    }

    auto const pathIt = m_fonts.find(desc.path);
    if (pathIt == m_fonts.end()) {
        return nullptr;
    }

    auto const& sizeMap = pathIt->second;
    auto const fontIt = sizeMap.find(desc.fontBaseSize);
    if (fontIt == sizeMap.end()) {
        return nullptr;
    }

    return &fontIt->second;
}

Sound const* AssetManager::sound(std::string_view assetId) const {
    auto const assetIt = m_assetRegistry.find(std::string{assetId});
    if (assetIt == m_assetRegistry.end()) {
        // Logger reserved
        return nullptr;
    }

    if (assetIt->second.type != AssetType::Sound) {
        // Logger reserved
        return nullptr;
    }

    auto const soundIt = m_sounds.find(assetIt->second.path);
    if (soundIt == m_sounds.end()) {
        // Logger reserved
        return nullptr;
    }

    return &soundIt->second;
}

Music* AssetManager::music(std::string_view assetId) {
    auto const assetIt = m_assetRegistry.find(std::string{assetId});
    if (assetIt == m_assetRegistry.end()) {
        return nullptr;
    }

    if (assetIt->second.type != AssetType::Music) {
        return nullptr;
    }

    auto const musicIt = m_musics.find(assetIt->second.path);
    if (musicIt == m_musics.end()) {
        return nullptr;
    }

    return &musicIt->second;
}

Music const* AssetManager::music(std::string_view assetId) const {
    auto const assetIt = m_assetRegistry.find(std::string{assetId});
    if (assetIt == m_assetRegistry.end()) {
        // Logger reserved
        return nullptr;
    }

    if (assetIt->second.type != AssetType::Music) {
        return nullptr;
    }

    auto const musicIt = m_musics.find(assetIt->second.path);
    if (musicIt == m_musics.end()) {
        return nullptr;
    }

    return &musicIt->second;
}

std::size_t AssetManager::refCount(std::filesystem::path const& path) const {
    auto const it = m_assetStates.find(path);
    if (it == m_assetStates.end()) {
        return 0;
    }

    return it->second.refCount;
}

void AssetManager::unloadAll() {
    for (auto& [path, sizeMap] : m_fonts) {
        for (auto& [size, fontObj] : sizeMap) {
            UnloadFont(fontObj);
        }
    }
    m_fonts.clear();

    for (auto const& item : m_sounds) {
        UnloadSound(item.second);
    }
    m_sounds.clear();

    for (auto const& item : m_musics) {
        UnloadMusicStream(item.second);
    }
    m_musics.clear();

    m_assetStates.clear();
}

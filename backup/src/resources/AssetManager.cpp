#include "AssetManager.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <istream>
#include <raylib.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

bool AssetManager::buildRegistry(std::filesystem::path const& groupsDirectory) {
    if (!std::filesystem::exists(groupsDirectory)) {
        return false;
    }

    if (!std::filesystem::is_directory(groupsDirectory)) {
        return false;
    }

    m_assetRegistry.clear();
    m_assetToGroups.clear();
    m_groupToAssets.clear();

    try {
        for (auto const& entry : std::filesystem::directory_iterator(groupsDirectory)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            if (entry.path().extension().string() != ".cfg") {
                continue;
            }

            std::ifstream file(entry.path());
            if (!file.is_open()) {
                continue;
            }

            std::string const groupId = entry.path().stem().string();

            std::string line;
            while (std::getline(file, line)) {
                auto const eqPos = line.find('=');
                if (eqPos == std::string::npos) {
                    continue;
                }

                std::string const assetId = line.substr(0, eqPos);
                std::string const rhs = line.substr(eqPos + 1);

                auto const typePos = rhs.find(':');
                if (typePos == std::string::npos) {
                    continue;
                }

                std::string const typeStr = rhs.substr(0, typePos);
                std::string const pathStr = rhs.substr(typePos + 1);

                AssetType type{};
                if (typeStr == "font") {
                    type = AssetType::Font;
                } else if (typeStr == "sound") {
                    type = AssetType::Sound;
                } else {
                    continue;
                }

                AssetDesc desc{.path = pathStr, .type = type};

                m_assetRegistry.emplace(assetId, std::move(desc));
                m_assetToGroups[assetId].insert(groupId);
                m_groupToAssets[groupId].insert(assetId);
            }
        }
    } catch (...) {
        return false;
    }

    return true;
}

AssetManager::AssetDesc const* AssetManager::getAsset(std::string_view assetId) const {
    auto const it = m_assetRegistry.find(std::string(assetId));
    if (it == m_assetRegistry.end()) {
        return nullptr;
    }

    return &it->second;
}

std::vector<std::string> const* AssetManager::getGroup(std::string_view groupId) const {
    auto const it = m_groupToAssets.find(std::string(groupId));
    if (it == m_groupToAssets.end()) {
        return nullptr;
    }

    static thread_local std::vector<std::string> tmp;
    tmp.clear();
    tmp.reserve(it->second.size());

    for (auto const& a : it->second) {
        tmp.push_back(a);
    }

    return &tmp;
}

bool AssetManager::hasAsset(std::string_view assetId) const {
    return m_assetRegistry.contains(std::string(assetId));
}

AssetManager::AssetType AssetManager::getAssetType(std::string_view assetId) const {
    auto const it = m_assetRegistry.find(std::string(assetId));
    if (it == m_assetRegistry.end()) {
        return AssetType{};
    }

    return it->second.type;
}

bool AssetManager::buildRegistryFromStream(std::istream& input) {
    m_assetToGroups.clear();
    m_groupToAssets.clear();
    m_errors.clear();

    std::string currentGroup;
    std::string line;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            currentGroup = line.substr(1, line.size() - 2);
            continue;
        }

        if (currentGroup.empty()) {
            m_errors.push_back(AssetLoadError::InvalidFormat);
            return false;
        }

        std::string const& assetId = line;

        m_groupToAssets[currentGroup].insert(assetId);
        m_assetToGroups[assetId].insert(currentGroup);
    }

    return true;
}

bool AssetManager::isAssetInGroup(std::string_view assetId, std::string_view groupId) const {
    auto const it = m_assetToGroups.find(std::string(assetId));
    if (it == m_assetToGroups.end()) {
        return false;
    }

    return it->second.contains(std::string(groupId));
}

std::vector<std::string> AssetManager::getGroupsOfAsset(std::string_view assetId) const {
    std::vector<std::string> result;

    auto const it = m_assetToGroups.find(std::string(assetId));
    if (it == m_assetToGroups.end()) {
        return result;
    }

    result.reserve(it->second.size());

    for (auto const& g : it->second) {
        result.push_back(g);
    }

    return result;
}

std::size_t AssetManager::assetCount() const {
    return m_assetToGroups.size();
}

std::size_t AssetManager::groupCount() const {
    return m_groupToAssets.size();
}

std::size_t AssetManager::groupSize(std::string_view groupId) const {
    auto const it = m_groupToAssets.find(std::string(groupId));
    if (it == m_groupToAssets.end()) {
        return 0;
    }

    return it->second.size();
}

std::vector<AssetManager::AssetLoadError> const& AssetManager::getErrors() const {
    return m_errors;
}

bool AssetManager::validateConsistency() const {
    for (auto const& [asset, groups] : m_assetToGroups) {
        for (auto const& g : groups) {
            auto const it = m_groupToAssets.find(g);
            if (it == m_groupToAssets.end()) {
                return false;
            }

            if (!it->second.contains(asset)) {
                return false;
            }
        }
    }

    for (auto const& [group, assets] : m_groupToAssets) {
        for (auto const& a : assets) {
            auto const it = m_assetToGroups.find(a);
            if (it == m_assetToGroups.end()) {
                return false;
            }

            if (!it->second.contains(group)) {
                return false;
            }
        }
    }

    return true;
}

bool AssetManager::unloadGroup(std::string_view groupId) {
    auto const gIt = m_groupToAssets.find(std::string(groupId));
    if (gIt == m_groupToAssets.end()) {
        return false;
    }

    auto const assets = std::move(gIt->second);
    m_groupToAssets.erase(gIt);

    for (auto const& assetId : assets) {
        auto const aIt = m_assetToGroups.find(assetId);
        if (aIt == m_assetToGroups.end()) {
            continue;
        }

        aIt->second.erase(std::string(groupId));

        if (aIt->second.empty()) {
            m_assetToGroups.erase(aIt);
        }
    }

    return true;
}

bool AssetManager::isAssetActive(std::string_view assetId) const {
    auto const it = m_assetToGroups.find(std::string(assetId));
    if (it == m_assetToGroups.end()) {
        return false;
    }

    return !it->second.empty();
}

void AssetManager::reset() {
    for (auto& [id, handle] : m_runtimeAssets) {
        handle.destroy();
    }

    m_runtimeAssets.clear();
}

bool AssetManager::loadAsset(std::string_view assetId) {
    auto const id = std::string(assetId);

    auto const regIt = m_assetRegistry.find(id);
    if (regIt == m_assetRegistry.end()) {
        return false;
    }

    auto const activeIt = m_assetToGroups.find(id);
    if (activeIt == m_assetToGroups.end() || activeIt->second.empty()) {
        return false;
    }

    AssetDesc const& desc = regIt->second;

    if (desc.type == AssetType::Font) {
        if (m_runtimeAssets.contains(id)) {
            return true;
        }

        Font font = LoadFont(desc.path.string().c_str());
        AssetHandle handle{};
        handle.type = AssetType::Font;
        handle.font = font;
        handle.loaded = true;

        m_runtimeAssets.emplace(std::string(id), std::move(handle));
        return true;
    }

    if (desc.type == AssetType::Sound) {
        if (m_runtimeAssets.contains(id)) {
            return true;
        }

        Sound sound = LoadSound(desc.path.string().c_str());
        AssetHandle handle{};
        handle.type = AssetType::Sound;
        handle.sound = sound;
        handle.loaded = true;

        m_runtimeAssets.emplace(std::string(id), std::move(handle));
        return true;
    }

    return false;
}

bool AssetManager::containsAsset(std::string_view assetId) const {
    return m_assetRegistry.contains(std::string(assetId));
}

bool AssetManager::isLoaded(std::string_view assetId) const {
    auto const it = m_runtimeAssets.find(assetId);
    if (it == m_runtimeAssets.end()) {
        return false;
    }

    return it->second.loaded;
}

AssetManager::AssetDesc const* AssetManager::assetDesc(std::string_view assetId) const {
    auto const it = m_assetRegistry.find(std::string(assetId));
    if (it == m_assetRegistry.end()) {
        return nullptr;
    }

    return &it->second;
}

bool AssetManager::unloadInternal(std::string_view assetId) {
    auto it = m_runtimeAssets.find(std::string(assetId));
    if (it == m_runtimeAssets.end()) {
        return false;
    }

    it->second.destroy();

    m_runtimeAssets.erase(it);

    return true;
}

bool AssetManager::unloadAsset(std::string_view assetId) {
    return unloadInternal(assetId);
}

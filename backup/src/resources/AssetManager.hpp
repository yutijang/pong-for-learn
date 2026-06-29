#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <istream>
#include <raylib.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class AssetManager {
  public:
    enum class AssetType : std::uint8_t {
        Font,
        Sound,
    };

    struct AssetDesc {
        std::filesystem::path path;
        AssetType type;
    };

    enum class AssetLoadError : std::uint8_t {
        InvalidFormat,
        UnknownGroup,
        EmptyAssetId,
    };

    struct AssetHandle {
        AssetType type{};
        bool loaded{false};

        union {
            Font font{};
            Sound sound;
        };

        AssetHandle() = default;

        ~AssetHandle() {
            destroy();
        }

        void destroy() {
            if (!loaded) {
                return;
            }

            if (type == AssetType::Font) {
                UnloadFont(font);
            } else if (type == AssetType::Sound) {
                UnloadSound(sound);
            }

            loaded = false;
        }

        AssetHandle(AssetHandle const&) = delete;
        AssetHandle& operator=(AssetHandle const&) = delete;

        AssetHandle(AssetHandle&& other) noexcept {
            type = other.type;
            loaded = other.loaded;

            if (type == AssetType::Font) {
                font = other.font;
            } else {
                sound = other.sound;
            }

            other.loaded = false;
        }

        AssetHandle& operator=(AssetHandle&& other) noexcept {
            if (this != &other) {
                destroy();

                type = other.type;
                loaded = other.loaded;

                if (type == AssetType::Font) {
                    font = other.font;
                } else {
                    sound = other.sound;
                }

                other.loaded = false;
            }
            return *this;
        }
    };

    struct StringHash {
        using is_transparent = void;

        std::size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }

        std::size_t operator()(std::string const& s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
    };

    struct StringEq {
        using is_transparent = void;

        bool operator()(std::string_view a, std::string_view b) const noexcept {
            return a == b;
        }
    };

    using AssetId = std::string;
    using OwnerId = std::string;
    using GroupId = std::string;

    bool buildRegistry(std::filesystem::path const& groupsDirectory);

    bool loadAsset(std::string_view assetId);
    bool unloadAsset(std::string_view assetId);

    bool addOwner(std::string_view ownerId, std::string_view assetId);
    bool removeOwner(std::string_view ownerId, std::string_view assetId);

    bool loadAssetGroup(std::string_view groupId);
    bool unloadAssetGroup(std::string_view groupId);

    void unloadAll();

    [[nodiscard]] bool containsAsset(std::string_view assetId) const;
    [[nodiscard]] bool isLoaded(std::string_view assetId) const;

    [[nodiscard]] AssetDesc const* assetDesc(std::string_view assetId) const;

    bool buildRegistryFromStream(std::istream& input);
    [[nodiscard]] std::size_t assetCount() const;
    [[nodiscard]] std::size_t groupCount() const;
    [[nodiscard]] bool isAssetInGroup(std::string_view assetId, std::string_view groupId) const;
    [[nodiscard]] bool validateConsistency() const;
    [[nodiscard]] std::vector<std::string> getGroupsOfAsset(std::string_view assetId) const;
    [[nodiscard]] std::size_t groupSize(std::string_view groupId) const;

    bool unloadGroup(std::string_view groupId);
    [[nodiscard]] bool isAssetActive(std::string_view assetId) const;
    void reset();
    bool unloadInternal(std::string_view assetId);
    [[nodiscard]] AssetType getAssetType(std::string_view assetId) const;
    [[nodiscard]] AssetDesc const* getAsset(std::string_view assetId) const;

  private:
    [[nodiscard]] std::vector<std::string> const* getGroup(std::string_view groupId) const;
    [[nodiscard]] bool hasAsset(std::string_view assetId) const;

    [[nodiscard]] std::vector<AssetLoadError> const& getErrors() const;

    std::unordered_map<AssetId, AssetDesc> m_assetRegistry;

    std::unordered_map<std::string, std::unordered_set<std::string>> m_assetToGroups;
    std::unordered_map<std::string, std::unordered_set<std::string>> m_groupToAssets;
    std::vector<AssetLoadError> m_errors;
    std::unordered_map<AssetId, AssetHandle, StringHash, StringEq> m_runtimeAssets;
};

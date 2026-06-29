#include "resources/AssetManager.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <sstream>

TEST_CASE("AssetManager - buildRegistryFromStream basic graph", "[AssetManager]") {
    AssetManager am;

    std::istringstream input(R"(
[groupA]
a1
a2

[groupB]
a2
a3
)");

    REQUIRE(am.buildRegistryFromStream(input) == true);

    REQUIRE(am.assetCount() == 3);
    REQUIRE(am.groupCount() == 2);

    REQUIRE(am.isAssetInGroup("a1", "groupA") == true);
    REQUIRE(am.isAssetInGroup("a2", "groupA") == true);
    REQUIRE(am.isAssetInGroup("a2", "groupB") == true);
    REQUIRE(am.isAssetInGroup("a3", "groupB") == true);
}

TEST_CASE("AssetManager - group size correctness", "[AssetManager]") {
    AssetManager am;

    std::istringstream input(R"(
[groupA]
a1
a2
a3
)");

    am.buildRegistryFromStream(input);

    REQUIRE(am.groupSize("groupA") == 3);
}

TEST_CASE("AssetManager - asset to groups mapping", "[AssetManager]") {
    AssetManager am;

    std::istringstream input(R"(
[groupA]
a1

[groupB]
a1
)");

    am.buildRegistryFromStream(input);

    auto groups = am.getGroupsOfAsset("a1");

    REQUIRE(groups.size() == 2);
}

TEST_CASE("AssetManager - consistency validation", "[AssetManager]") {
    AssetManager am;

    std::istringstream input(R"(
[groupA]
a1
a2
)");

    am.buildRegistryFromStream(input);

    REQUIRE(am.validateConsistency() == true);
}

TEST_CASE("AssetManager - empty input", "[AssetManager]") {
    AssetManager am;

    std::istringstream input("");

    am.buildRegistryFromStream(input);

    REQUIRE(am.assetCount() == 0);
    REQUIRE(am.groupCount() == 0);
}

TEST_CASE("AssetManager - buildRegistryFromStream basic parse") {
    AssetManager am;

    std::istringstream input(R"(
[ui]
button
icon

[audio]
click
boom
)");

    REQUIRE(am.buildRegistryFromStream(input) == true);

    REQUIRE(am.isAssetInGroup("button", "ui"));
    REQUIRE(am.isAssetInGroup("icon", "ui"));
    REQUIRE(am.isAssetInGroup("click", "audio"));
    REQUIRE(am.isAssetInGroup("boom", "audio"));
}

TEST_CASE("AssetManager - group and asset count") {
    AssetManager am;

    std::istringstream input(R"(
[ui]
button
icon
icon

[audio]
click
)");

    am.buildRegistryFromStream(input);

    REQUIRE(am.groupCount() == 2);
    REQUIRE(am.assetCount() == 3);
}

TEST_CASE("AssetManager - validate consistency") {
    AssetManager am;

    std::istringstream input(R"(
[ui]
button

[audio]
button
)");

    am.buildRegistryFromStream(input);

    REQUIRE(am.validateConsistency() == true);
}

TEST_CASE("AssetManager - unload group removes mapping") {
    AssetManager am;

    std::istringstream input(R"(
[ui]
button
icon
)");

    am.buildRegistryFromStream(input);

    REQUIRE(am.unloadGroup("ui") == true);
    REQUIRE(am.isAssetInGroup("button", "ui") == false);
}

TEST_CASE("AssetManager - loadAsset rejects unknown asset") {
    AssetManager am;

    REQUIRE(am.loadAsset("not_exist") == false);
}

TEST_CASE("AssetManager - buildRegistry loads menu.cfg correctly") {
    AssetManager am;

    std::filesystem::path testDir = std::filesystem::current_path() / "../../assets/groups";

    REQUIRE(am.buildRegistry(testDir) == true);

    REQUIRE(am.containsAsset("label_font") == true);
    REQUIRE(am.getAssetType("label_font") == AssetManager::AssetType::Font);

    auto const* desc = am.getAsset("label_font");
    REQUIRE(desc != nullptr);

    REQUIRE(desc->path == "assets/fonts/UVNTinTucHepThem_B.TTF");
}

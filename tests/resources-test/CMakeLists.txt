# Tạo executable cho test
add_executable(AssetManager-tests
    test-AssetManager.cpp
)

# Liên kết với notes-core và Catch2
target_link_libraries(AssetManager-tests
    PRIVATE
        core
        raylib
        Catch2::Catch2WithMain  # target có sẵn main() của Catch2 v3
)

target_include_directories(AssetManager-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
    SYSTEM PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/raylib/include"        
)

catch_discover_tests(AssetManager-tests)

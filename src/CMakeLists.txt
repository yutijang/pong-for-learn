add_executable(${PROJECT_NAME}
    main.cpp
)

add_library(core
    core/Game.cpp
    entities/Player.cpp
    entities/Enemy.cpp
    entities/Ball.cpp
    resources/AssetManager.cpp
    resources/AudioManager.cpp
    scenes/GameplayScene.cpp
    scenes/MenuScene.cpp
    world/GameLevel.cpp
    world/LevelLoader.cpp
    ui/Label.cpp
    ui/MenuList.cpp
    ui/Button.cpp
    ui/PauseOverlay.cpp
    save/SaveSerializer.cpp
    save/SaveManager.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
    SYSTEM PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/raylib/include"
)

target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/raylib/include"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        core
        project_warnings
        raylib
)

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/raylib/bin/raylib.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

cmake_minimum_required(VERSION 3.31)
project(MyProject VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set general output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# Set folder junction root/assets -> bin/assets
set(ASSET_SOURCE_DIR "${PROJECT_SOURCE_DIR}/assets")
set(ASSET_LINK_DIR   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

if(EXISTS "${ASSET_LINK_DIR}")
    file(REMOVE_RECURSE "${ASSET_LINK_DIR}")
endif()

message(STATUS "Creating assets junction")

execute_process(
    COMMAND
        powershell
        -NoProfile
        -ExecutionPolicy Bypass
        -Command
        "New-Item -ItemType Junction -Path '${ASSET_LINK_DIR}' -Target '${ASSET_SOURCE_DIR}' | Out-Null"
    COMMAND_ERROR_IS_FATAL ANY
)
# -----------------------------------------------------------

# --- Add global flags --------------------------------------
add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE

    # Clang/GCC-style warnings (match clang, GCC, clang-cl)
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror
        -Wall
        -Wextra
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Wcast-align
        -Wnull-dereference
        -Wdouble-promotion
        -Wimplicit-fallthrough
        -Wundef
        -Wswitch-enum
        -Wno-unused-function
    >

    # clang-cl (expanding)
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${MSVC}>>:
        /WX
        /W4
        /EHsc
        /permissive-
        /utf-8
        /Zc:__cplusplus

        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-switch-default
        -Wno-nrvo

        -Wno-padded

        -Wno-exit-time-destructors
    >
    # xử lý "_autogen", cần xem xét
    # -Wno-unsafe-buffer-usage, -Wno-global-constructors

    # MSVC cl.exe only
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX
        /W4
        /EHsc
        /permissive-
        /utf-8
        /Zc:__cplusplus
        /Zc:preprocessor
    >

    $<$<NOT:$<BOOL:${MSVC}>>:
        --target=x86_64-pc-linux-gnu
    >

    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${MSVC}>>:
        --target=x86_64-pc-windows-msvc
    >
)
# ------------------------------------------------------------

add_library(raylib SHARED IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/libs/raylib/lib/raylibdll.lib"
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/raylib/bin/raylib.dll"
)

add_subdirectory(src)

# Tests
# if(BUILD_TESTING)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

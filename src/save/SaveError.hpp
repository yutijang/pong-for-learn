#pragma once

#include <cstdint>

enum class SaveError : std::uint8_t {
    InvalidSlot,
    SlotEmpty,

    FileNotFound,
    OpenFileFailed,
    InvalidFileFormat,
    WriteFailed
};

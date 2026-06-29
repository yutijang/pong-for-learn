#pragma once

#include "SaveData.hpp"
#include "SaveError.hpp"

#include <expected>

using LoadSaveResult = std::expected<SaveData, SaveError>;
using SaveResult = std::expected<void, SaveError>;

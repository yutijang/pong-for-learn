#include "SaveManager.hpp"

#include "SaveData.hpp"
#include "SaveError.hpp"
#include "SaveResult.hpp"
#include "SaveSerializer.hpp"
#include "SaveSlotInfo.hpp"

#include <chrono>
#include <expected>
#include <filesystem>
#include <string>
#include <system_error>
#include <utility>

LoadSaveResult SaveManager::loadSlot(int slotId) const {
    auto const* const info = slotInfo(slotId);

    if (info == nullptr) {
        return std::unexpected{SaveError::InvalidSlot};
    }

    if (!info->occupied) {
        return std::unexpected{SaveError::SlotEmpty};
    }

    return SaveSerializer::load(slotPath(slotId));
}

SaveResult SaveManager::saveSlot(int slotId, SaveData const& saveData) {
    if (slotId < 1 || slotId > MAX_MANUAL_SAVE_SLOTS) {
        return std::unexpected{SaveError::InvalidSlot};
    }

    auto const result = SaveSerializer::save(slotPath(slotId), saveData);
    if (!result) {
        return result;
    }

    refreshSlots();

    return {};
}

LoadSaveResult SaveManager::loadAutoSave() const {
    auto const path = autoSavePath();

    if (!std::filesystem::exists(path)) {
        return std::unexpected{SaveError::FileNotFound};
    }

    return SaveSerializer::load(path);
}

SaveResult SaveManager::saveAutoSave(SaveData const& saveData) {
    return SaveSerializer::save(autoSavePath(), saveData);
}

bool SaveManager::deleteSlot(int slotId) {
    auto const pathToDel = slotPath(slotId);

    std::error_code ec;

    bool const removed = std::filesystem::remove(pathToDel, ec);

    if (!removed || !ec) {
        return false;
    }

    refreshSlots();

    return true;
}

void SaveManager::refreshSlots() {
    // Dọn dẹp trước khi build m_slots cache
    m_slots.clear();
    m_slots.reserve(MAX_MANUAL_SAVE_SLOTS);

    // Duyệt toàn bộ manual slot
    for (int slotId{1}; slotId <= MAX_MANUAL_SAVE_SLOTS; ++slotId) {
        auto const path = slotPath(slotId);

        SaveSlotInfo info;

        info.slotId = slotId;
        info.occupied = std::filesystem::exists(path);

        if (info.occupied) {
            auto result = SaveSerializer::load(path);

            if (result) {
                info.state = SaveSlotState::Valid;
                info.currentLevel = result->currentLevel;
            } else {
                info.state = SaveSlotState::Corrupted;
            }

            auto const writeTime = std::filesystem::last_write_time(path);
            auto const systemTime = std::chrono::clock_cast<std::chrono::system_clock>(writeTime);

            info.lastSaveTime = std::chrono::system_clock::to_time_t(systemTime);
        }

        m_slots.push_back(std::move(info));
    }
}

SaveSlotInfo const* SaveManager::slotInfo(int slotId) const {
    for (auto const& slot : m_slots) {
        if (slot.slotId == slotId) {
            return &slot;
        }
    }

    return nullptr;
}

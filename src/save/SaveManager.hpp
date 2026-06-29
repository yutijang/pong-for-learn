#pragma once

#include "SaveData.hpp"
#include "SaveResult.hpp"
#include "SaveSlotInfo.hpp"

#include <filesystem>
#include <format>
#include <span>
#include <utility>
#include <vector>

class SaveManager {
  public:
    // Số lượng save slot thủ công tối đa mà game hỗ trợ.
    // Không bao gồm autosave.
    static constexpr int MAX_MANUAL_SAVE_SLOTS{3};

    /*
     * Khởi tạo SaveManager với thư mục chứa toàn bộ file save.
     *
     * Ví dụ:
     * saves/
     *     autosave.sav
     *     slot1.sav
     *     slot2.sav
     *     slot3.sav
     */
    explicit SaveManager(std::filesystem::path saveDirectory)
        : m_saveDirectory(std::move(saveDirectory)) {
        refreshSlots();
    }

    /*
     * Trả về danh sách metadata của toàn bộ save slot đã được cache trong m_slots.
     *
     * Không đọc filesystem.
     * Không copy dữ liệu.
     * Chỉ cung cấp view chỉ đọc.
     */
    [[nodiscard]]
    std::span<SaveSlotInfo const> slots() const {
        return m_slots;
    }

    /*
     * Đọc dữ liệu save từ save slot chỉ định.
     *
     * Ví dụ:
     * slotId = 2
     *
     * → đọc:
     * slot2.sav
     *
     * → trả về:
     * SaveData
     */
    [[nodiscard]]
    LoadSaveResult loadSlot(int slotId) const;

    /*
     * Ghi SaveData vào save slot chỉ định.
     *
     * Đồng thời cập nhật metadata:
     * - occupied
     * - saveName
     * - currentLevel
     * - lastSaveTime
     */
    [[nodiscard]]
    SaveResult saveSlot(int slotId, SaveData const& saveData);

    /*
     * Đọc dữ liệu từ autosave.
     *
     * Ví dụ:
     * autosave.sav
     */
    [[nodiscard]]
    LoadSaveResult loadAutoSave() const;

    /*
     * Ghi SaveData vào autosave.
     * Không ảnh hưởng các manual save slot.
     */
    [[nodiscard]]
    SaveResult saveAutoSave(SaveData const& saveData);

    /*
     * Xóa file save của slot chỉ định.
     *
     * Ví dụ:
     * slot2.sav
     *
     * Sau khi xóa:
     * occupied = false
     */
    bool deleteSlot(int slotId);

    /*
     * Đồng bộ cache m_slots trong bộ nhớ
     * với trạng thái các file save trên đĩa.
     *
     * Quét thư mục save.
     *
     * Đọc metadata của:
     * - slot1
     * - slot2
     * - slot3
     *
     * và xây dựng lại m_slots.
     *
     * Thường được gọi khi:
     * - mở menu Save/Load
     * - sau khi save
     * - sau khi delete
     */
    void refreshSlots();

    /*
     * Tìm metadata của một slot cụ thể trong m_slots.
     *
     * Ví dụ:
     * slotInfo(2)
     *
     * → trả về metadata của slot2
     * → nullptr nếu không tồn tại.
     */
    [[nodiscard]]
    SaveSlotInfo const* slotInfo(int slotId) const;

  private:
    /*
     * Sinh đường dẫn file save tương ứng với slot.
     *
     * Ví dụ:
     * slotPath(2)
     *
     * → saves/slot2.sav
     */
    [[nodiscard]]
    std::filesystem::path slotPath(int slotId) const {
        return m_saveDirectory / std::format("slot{}.sav", slotId);
    }

    /*
     * Sinh đường dẫn file autosave.
     *
     * Ví dụ:
     * → saves/autosave.sav
     */
    [[nodiscard]]
    std::filesystem::path autoSavePath() const {
        return m_saveDirectory / "autosave.sav";
    }

    std::filesystem::path m_saveDirectory;
    std::vector<SaveSlotInfo> m_slots;
};

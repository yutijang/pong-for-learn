#pragma once

#include "SaveData.hpp"
#include "SaveResult.hpp"

#include <filesystem>
#include <string_view>

class SaveSerializer {
  public:
    /*
     * Ghi SaveData xuống file save tại đường dẫn chỉ định.
     *
     * Trách nhiệm:
     * - Mở file.
     * - Chuyển SaveData thành định dạng lưu trữ.
     * - Ghi dữ liệu xuống đĩa.
     *
     * Không quản lý slot.
     * Không quản lý autosave.
     * Không quản lý metadata.
     *
     * SaveSerializer::save("saves/slot1.sav", saveData);
     */
    [[nodiscard]]
    static SaveResult save(std::filesystem::path const& filePath, SaveData const& saveData);

    /*
     * Đọc file save tại đường dẫn chỉ định và khôi phục SaveData.
     *
     * Trách nhiệm:
     * - Mở file.
     * - Phân tích nội dung file.
     * - Tạo SaveData từ dữ liệu đọc được.
     * - Trả về kết quả hoặc lỗi.
     *
     * auto result = SaveSerializer::load("saves/slot1.sav");
     */
    [[nodiscard]]
    static LoadSaveResult load(std::filesystem::path const& filePath);

  private:
    static SaveResult applyData(SaveData& sav, std::string_view key, std::string_view value);
};

#pragma once

#include <charconv>
#include <concepts>
#include <string>
#include <string_view>
#include <system_error>

namespace utility::strings {

static constexpr std::string_view trim(std::string_view src) noexcept {
    // Tìm ký tự đầu tiên không phải khoảng trắng
    auto const start = src.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string_view::npos) {
        return {}; // Chuỗi toàn khoảng trắng
    }

    // Tìm ký tự cuối cùng không phải khoảng trắng
    auto const end = src.find_last_not_of(" \t\n\r\f\v");

    return src.substr(start, end - start + 1);
}

} // namespace utility::strings

namespace utility::parsing {

// Overload for numerics
// Convert text to value with T type
template<typename T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]]
bool parseValue(std::string_view text, T& value) noexcept {
    auto const* first = text.data();
    auto const* last = text.data() + text.size();

    T temp;

    auto const result = std::from_chars(first, last, temp);

    if (result.ec != std::errc{}) {
        return false;
    }

    if (result.ptr != last) {
        return false;
    }

    value = temp;

    return true;
}

// Overload for bool
[[nodiscard]]
static bool parseValue(std::string_view text, bool& value) noexcept {
    if (text == "true") {
        value = true;
        return true;
    }

    if (text == "false") {
        value = false;
        return true;
    }

    return false;
}

// Overload for str::string
[[nodiscard]]
static bool parseValue(std::string_view text, std::string& value) {
    value.assign(text);
    return true;
}

} // namespace utility::parsing

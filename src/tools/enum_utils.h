#ifndef TOOLS_ENUM_UTILS_H
#define TOOLS_ENUM_UTILS_H
#include <string>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace tools::enum_utils {

template <typename EnumT>
inline std::string valueToString(EnumT value)
{
    if constexpr (std::is_enum_v<EnumT>) {
        return fmt::format("{}", static_cast<typename std::underlying_type<EnumT>::type>(value));
    } else {
        // throw std::invalid_argument("Provided type is not an enum");
        return "";
    }
}

template <typename EnumT>
inline EnumT valueFromString(const std::string& str)
{
    if constexpr (std::is_enum_v<EnumT>) {
        try {
            return static_cast<EnumT>(std::stoi(str));
        } catch (const std::invalid_argument&) {
            // throw std::invalid_argument(fmt::format("Cannot convert '{}' to enum type '{}'", str, typeid(EnumT).name()));
            return EnumT{};
        }
    } else {
        // throw std::invalid_argument("Provided type is not an enum");
        return EnumT{};
    }
}

// tools to convert enum item name to string and vice versa
// #define ENUM_NAME(item) do {  } while (0);

} // namespace tools::enum_utils

#endif // TOOLS_ENUM_UTILS_H

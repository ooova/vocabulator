#ifndef UI_TOOLS_LOCALE_H
#define UI_TOOLS_LOCALE_H

#include "spdlog/spdlog.h"

#include <span>
#include <string>
#include <map>

namespace ui::tools {

enum class Language {
    kNumbers,
    kMathSymbols,
    kSpecSymbols,
    kRU,
    kEN,
    kBG,
    kDE,
    kFR,
};

// const std::array char_set_{locale::Language::kRU, locale::Language::kEN};

class Locale {
public:
    static std::string translateInterface(std::string const& english) {
        if (locale_.contains(english)) {
            return locale_[english];
        }
        return english;
    }

    /**
     * throw GlobalError if language is not found (or not supported)
     */
    static std::string getAlphabet(std::span<Language const> const langs) {
        std::string alphabet{};

        for (auto const lang : langs) {
            alphabet += alphabets_.at(lang);
            // spdlog::trace("{}: {}", __FUNCTION__, alphabets_.at(lang));
        }

        return alphabet;
    }

private:
    static inline std::map<std::string/*en*/, std::string/*other*/> locale_ {
        {"next", "следующее"},
        {"previous", "предыдущее"}
        // {"next", "N"},
        // {"previous", "P"}
    };

    static inline std::map<Language, std::string> alphabets_ {
        {Language::kNumbers, "0123456789"},
        {Language::kMathSymbols, "()*+-/<=>"},
        {Language::kSpecSymbols, " !\"#$%&\',.:;?@[\\]^_`{|}~"},
        {Language::kRU, "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя"},
        {Language::kEN, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
        {Language::kBG, ""},
        {Language::kDE, ""},
        {Language::kFR, ""},
    };
};

} // namespace ui::tools

#endif // UI_TOOLS_LOCALE_H

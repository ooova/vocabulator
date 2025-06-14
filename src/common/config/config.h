#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include "common/exceptions/config_error.h"

#include <map>
#include <string>
#include <filesystem>

namespace common {

enum class ConfigId {
    // int ----------------------------------------------------------
    // layout config ------------------------------------------------
    kWindowWidth,
    kWindowHeight,
    kWindowMargin,
    kButtonWidth,
    kButtonHeight,
    kButtonFontSize,
    kElementMargin,
    kCardWidth,
    kCardHeight,
    kCardFontSize,
    kTextBoxVocabularyStatisticsWidth,
    kTextBoxVocabularyStatisticsHeight,
    kTextBoxVocabularyStatisticsFontSize,
    kTextBoxWordStatisticsWidth,
    kTextBoxWordStatisticsHeight,
    kTextBoxWordStatisticsFontSize,
    kStatusMessageTimer,
    // vocabulary config --------------------------------------------
    kRetentionRateForKnownWord,

    // float ---------------------------------------------------------
    // layout config ------------------------------------------------
    kScaleFactor,

    // hex ---------------------------------------------------------
    kWindowBackgroundColor,

    // string ------------------------------------------------------
    kFontPath,
    kVocabularyPathMd,
    kVocabularyPathJson,
    kDefaultServer,
    kDefaultPort,
    kDefaultTarget,
    kDefaultMethod,

    // bool -------------
    kWindowResizable,
};

enum class ConfigType {
    kInt,
    // kHex,
    kString,
    kBool,
    // kFloat, // if needed
    // kDouble, // if needed
    // kVector, // if needed
    // kMap, // if needed
};

// template <typename ValueT>
struct ConfigValue {
    std::string name;
    ConfigType type;
    std::string value;
    std::string default_value;
};


// singleton class to manage configuration values
class Config {
private:
    Config();

    // Disable copy and move constructors
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;
public:
    static Config& instance() {
        static Config instance;
        return instance;
    }
    ~Config() = default;

    template <typename ValueT>
    ValueT getValue(ConfigId id) const {
        auto it = values_.find(id);
        if (it == values_.end()) {
            throw ConfigError("Config value not found: " + std::to_string(static_cast<int>(id)));
        }
        try {
            const auto value = it->second.value.empty() ? it->second.default_value : it->second.value;
            return parseValue<ValueT>(value);
        } catch (const std::exception& e) {
            throw ConfigError("Failed to parse config value: " + it->second.name + " - " + e.what());
        }
    }

    template <typename ValueT>
    void setValue(ConfigId id, const ValueT& value) {
        auto it = values_.find(id);
        if (it == values_.end()) {
            throw ConfigError("Config value not found: " + std::to_string(static_cast<int>(id)));
        }
        it->second.value = formatValue(value);
    }

    void setConfigFilePath(std::filesystem::path const& file_path) {
        config_file_path_ = file_path;
    }
    void loadFromFile(std::filesystem::path const& file_path = {});
    void saveToFile(std::filesystem::path const& file_path = {}) const;

    std::string toString() const;

private:
    std::filesystem::path config_file_path_;
    std::map<ConfigId, ConfigValue> values_;

    template <typename ValueT>
    ValueT parseValue(const std::string& value) const;

    template <typename ValueT>
    std::string formatValue(const ValueT& value) const;
};

// parseValue
template <>
int Config::parseValue<int>(const std::string& value) const;
template <>
unsigned int Config::parseValue<unsigned int>(const std::string& value) const;
template <>
std::string Config::parseValue<std::string>(const std::string& value) const;
template <>
bool Config::parseValue<bool>(const std::string& value) const;

// formatValue
template <>
std::string Config::formatValue<int>(const int& value) const;
template <>
std::string Config::formatValue<unsigned int>(const unsigned int& value) const;
template <>
std::string Config::formatValue<std::string>(const std::string& value) const;
template <>
std::string Config::formatValue<bool>(const bool& value) const;

}  // namespace common

#endif  // COMMON_CONFIG_H

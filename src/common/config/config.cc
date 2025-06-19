#include "config.h"

#include "spdlog/spdlog.h"
#include "tools/string_utils.h"

#include <fstream>

namespace common {

Config::Config()
{
    // Initialize default values

    // int ---------------
    values_[ConfigId::kWindowWidth] = {"kWindowWidth", ConfigType::kInt, "", "800"};
    values_[ConfigId::kWindowHeight] = {"kWindowHeight", ConfigType::kInt, "", "450"};
    values_[ConfigId::kWindowMargin] = {"kWindowMargin", ConfigType::kInt, "", "10"};
    values_[ConfigId::kButtonWidth] = {"kButtonWidth", ConfigType::kInt, "", "100"};
    values_[ConfigId::kButtonHeight] = {"kButtonHeight", ConfigType::kInt, "", "30"};
    values_[ConfigId::kButtonFontSize] = {"kButtonFontSize", ConfigType::kInt, "", "18"};
    values_[ConfigId::kInputFontSize] = {"kInputFontSize", ConfigType::kInt, "", "18"};
    values_[ConfigId::kElementMargin] = {"kElementMargin", ConfigType::kInt, "", "5"};
    values_[ConfigId::kCardWidth] = {"kCardWidth", ConfigType::kInt, "", "400"};
    values_[ConfigId::kCardHeight] = {"kCardHeight", ConfigType::kInt, "", "300"};
    values_[ConfigId::kCardFontSize] = {"kCardFontSize", ConfigType::kInt, "", "28"};
    values_[ConfigId::kTextBoxVocabularyStatisticsWidth] = {"kTextBoxVocabularyStatisticsWidth", ConfigType::kInt, "", "200"};
    values_[ConfigId::kTextBoxVocabularyStatisticsHeight] = {"kTextBoxVocabularyStatisticsHeight", ConfigType::kInt, "", "200"};
    values_[ConfigId::kTextBoxVocabularyStatisticsFontSize] = {"kTextBoxVocabularyStatisticsFontSize", ConfigType::kInt, "", "20"};
    values_[ConfigId::kTextBoxWordStatisticsWidth] = {"kTextBoxWordStatisticsWidth", ConfigType::kInt, "", "300"};
    values_[ConfigId::kTextBoxWordStatisticsHeight] = {"kTextBoxWordStatisticsHeight", ConfigType::kInt, "", "100"};
    values_[ConfigId::kTextBoxWordStatisticsFontSize] = {"kTextBoxWordStatisticsFontSize", ConfigType::kInt, "", "20"};
    values_[ConfigId::kStatusMessageTimer] = {"kStatusMessageTimer", ConfigType::kInt, "", "3"};
    // vocabulary config --------------------------------------------
    // for retention rate in percentage it would be 85 (85% of words should be known)
    // for retention rate as "know" - "don't know" difference it would be 3 (3 more "know" than "don't know")
    values_[ConfigId::kRetentionRateForKnownWord] = {"kRetentionRateForKnownWord", ConfigType::kInt, "", "3"};

    // float -------------
    values_[ConfigId::kScaleFactor] = {"kScaleFactor", ConfigType::kInt, "", "1.0"};

    // hex -------------
    values_[ConfigId::kWindowBackgroundColor] = {"kWindowBackgroundColor", ConfigType::kInt, "", "0x18181800"}; // decimal: 404232192; hex: 0x18181800

    // // string -------------
    values_[ConfigId::kFontPath] = {"kFontPath", ConfigType::kString, "", "assets/fonts/Ubuntu-R.ttf"};
    values_[ConfigId::kVocabularyPathMd] = {"kVocabularyPathMd", ConfigType::kString, "", "assets/vocabulary.md"};
    values_[ConfigId::kVocabularyPathJson] = {"kVocabularyPathJson", ConfigType::kString, "", "assets/vocabulary.json"};
    values_[ConfigId::kDefaultServer] = {"kDefaultServer", ConfigType::kString, "", "localhost"};
    values_[ConfigId::kDefaultPort] = {"kDefaultPort", ConfigType::kString, "", "1234"};
    values_[ConfigId::kDefaultTarget] = {"kDefaultTarget", ConfigType::kString, "", "/v1/chat/completions"};
    values_[ConfigId::kDefaultMethod] = {"kDefaultMethod", ConfigType::kString, "", "POST"};

    // // bool -------------
    values_[ConfigId::kWindowResizable] = {"kWindowResizable", ConfigType::kBool, "", "true"};
}

void Config::loadFromFile(std::filesystem::path const& file_path)
{
    auto const path{file_path.empty() ? config_file_path_.string() : file_path.string()};
    auto file{std::ifstream(path)};
    if (!file.is_open()) {
        throw ConfigError("Failed to open config file: " + path);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        auto delimiter_pos = line.find('=');
        if (delimiter_pos == std::string::npos) {
            spdlog::warn("Skip invalid config line: {}", line);
            continue;
        }
        auto key = line.substr(0, delimiter_pos);
        auto value = line.substr(delimiter_pos + 1);
        tools::string_utils::trim(key);
        tools::string_utils::trim(value);
        auto it = std::find_if(values_.begin(), values_.end(),
                               [&key](const auto& pair) { return pair.second.name == key; });
        if (it != values_.end()) {
            it->second.value = value;
        } else {
            spdlog::warn("Unknown config key: \'{}\'; Config parameter skipped", key);
            continue;
        }
    }

    file.close();
    spdlog::info("Configuration loaded from file: {}\n{}", path, toString());
}

void Config::saveToFile(std::filesystem::path const& file_path) const
{
    auto const path{file_path.empty() ? config_file_path_.string() : file_path.string()};
    auto file{std::ofstream(path)};
    if (!file.is_open()) {
        throw ConfigError("Failed to open config file for writing: " + path);
    }

    for (const auto& [id, value] : values_) {
        file << value.name << " = " << value.value << "\n";
    }

    file.close();
    spdlog::info("Configuration saved to file: {}", path);
}

std::string Config::toString() const
{
    std::stringstream ss;
    for (const auto& [id, value] : values_) {
        ss << value.name << " = " << value.value << "\n";
    }
    return ss.str();
}

// parseValue
template <>
int Config::parseValue<int>(const std::string& value) const
{
    return std::stoi(value);
}

template <>
unsigned int Config::parseValue<unsigned int>(const std::string& value) const
{
    const auto value_lower = tools::string_utils::toLowerCase(value);
    if (value_lower.find("x") != std::string::npos) {
        return std::stoul(value_lower, nullptr, 16);
    }
    return std::stoul(value_lower);
}

template <>
float Config::parseValue<float>(const std::string& value) const
{
    return std::stof(value);
}

template <>
std::string Config::parseValue<std::string>(const std::string& value) const
{
    return value;
}

template <>
bool Config::parseValue<bool>(const std::string& value) const
{
    const auto value_lower = tools::string_utils::toLowerCase(value);
    return value == "true" || value == "1";
}

// formatValue
template <>
std::string Config::formatValue<int>(const int& value) const
{
    return std::to_string(value);
}

template <>
std::string Config::formatValue<unsigned int>(const unsigned int& value) const
{
    return std::to_string(value);
}

template <>
std::string Config::formatValue<std::string>(const std::string& value) const
{
    return value;
}

template <>
std::string Config::formatValue<bool>(const bool& value) const
{
    return value ? "true" : "false";
}

} // namespace common

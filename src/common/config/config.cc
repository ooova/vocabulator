#include "config.h"

#include "spdlog/spdlog.h"
#include "tools/string_utils.h"

#include <fstream>

namespace common {

Config::Config()
{
    // Initialize default values

    // int ---------------
    values_[ConfigId::kWindowWidth] = {"kWindowWidth", ConfigType::kInt, "", "800"}; // kWindowWidth, // {800};
    values_[ConfigId::kWindowHeight] = {"kWindowHeight", ConfigType::kInt, "", "450"}; // kWindowHeight, // {450};
    values_[ConfigId::kWindowMargin] = {"kWindowMargin", ConfigType::kInt, "", "10"}; // kWindowMargin, // {10};
    values_[ConfigId::kButtonWidth] = {"kButtonWidth", ConfigType::kInt, "", "100"}; // kButtonWidth, // {100};
    values_[ConfigId::kButtonHeight] = {"kButtonHeight", ConfigType::kInt, "", "30"}; // kButtonHeight, // {30};
    values_[ConfigId::kElementMargin] = {"kElementMargin", ConfigType::kInt, "", "5"}; // kElementMargin, // {5};
    values_[ConfigId::kCardHeight] = {"kCardHeight", ConfigType::kInt, "", "150"}; // kCardHeight, // {150};
    values_[ConfigId::kCardFontSize] = {"kCardFontSize", ConfigType::kInt, "", "28"}; // kCardFontSize, // {28};
    values_[ConfigId::kTextBoxWordStatisticsFontSize] = {"kTextBoxWordStatisticsFontSize", ConfigType::kInt, "", "18"}; // kTextBoxWordStatisticsFontSize, // {18};
    values_[ConfigId::kTextBoxWordStatisticsWidth] = {"kTextBoxWordStatisticsWidth", ConfigType::kInt, "", "300"}; // kTextBoxWordStatisticsWidth, // {300};
    values_[ConfigId::kTextBoxWordStatisticsHeight] = {"kTextBoxWordStatisticsHeight", ConfigType::kInt, "", "100"}; // kTextBoxWordStatisticsHeight, // {100};
    values_[ConfigId::kStatusMessageTimer] = {"kStatusMessageTimer", ConfigType::kInt, "", "3"}; // kStatusMessageTimer, // {3};

    // // hex -------------
    values_[ConfigId::kBackgroundColor] = {"kBackgroundColor", ConfigType::kInt, "", "0x181818"}; // kBackgroundColor, // {0x181818};

    // // string -------------
    values_[ConfigId::kFontPath] = {"kFontPath", ConfigType::kString, "", "assets/fonts/Ubuntu-R.ttf"}; // kFontPath, // {"../assets/fonts/Ubuntu-R.ttf"};
    values_[ConfigId::kVocabularyPathMd] = {"kVocabularyPathMd", ConfigType::kString, "", "assets/vocabulary.md"}; // kVocabularyPathMd, // {"../assets/vocabulary.md"};
    values_[ConfigId::kVocabularyPathJson] = {"kVocabularyPathJson", ConfigType::kString, "", "assets/vocabulary.json"}; // kVocabularyPathJson, // {"../assets/vocabulary.json"};
    values_[ConfigId::kDefaultServer] = {"kDefaultServer", ConfigType::kString, "", "localhost"}; // kDefaultServer, // {"localhost"};
    values_[ConfigId::kDefaultPort] = {"kDefaultPort", ConfigType::kString, "", "1234"}; // kDefaultPort, // {"1234"};
    values_[ConfigId::kDefaultTarget] = {"kDefaultTarget", ConfigType::kString, "", "/v1/chat/completions"}; // kDefaultTarget, // {"/v1/chat/completions"};
    values_[ConfigId::kDefaultMethod] = {"kDefaultMethod", ConfigType::kString, "", "POST"}; // kDefaultMethod, // {"POST"};

    // // bool -------------
    values_[ConfigId::kWindowResizable] = {"kWindowResizable", ConfigType::kBool, "", "true"}; // kWindowResizable, //{true};
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
    spdlog::info("Configuration loaded from file: {}", path);
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

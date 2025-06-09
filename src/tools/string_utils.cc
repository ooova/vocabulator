#include "string_utils.h"

#include <sstream>
#include <algorithm>

namespace tools::string_utils {

void removePrefix(std::string& str, char ch)
{
    if (const auto index = str.find_first_not_of(ch); index != std::string::npos) {
        str.erase(0, index);
    }
}

void removeSuffix(std::string& str, char ch)
{
    if (const auto index = str.find_last_not_of(ch); index != std::string::npos) {
        str.erase(index + 1);
    }
}

void removePrefixSpacesAndTabs(std::string& str)
{
    auto count{0UL};
    while ((count != str.size()) && ((str.at(count) == ' ') || (str.at(count) == '\t'))) {
        ++count;
    }
    str.erase(0, count);
}

void removeSuffixSpacesAndTabs(std::string& str)
{
    auto index{str.size()};
    while ((0 < index) && ((str.at(index - 1) == ' ') || (str.at(index - 1) == '\t') || (str.at(index - 1) == '\n'))) {
        --index;
    }
    if (index < str.size()) {
        str.erase(index);
    }
}

void trim(std::string& str)
{
    removePrefixSpacesAndTabs(str);
    removeSuffixSpacesAndTabs(str);
};

std::vector<std::string> split(std::string_view str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(std::string{str});
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string codepoint_to_utf8(int codepoint) {
    std::string result;
    if (codepoint <= 0x7F) {
        result += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    return result;
}

std::string toLowerCase(const std::string& str)
{
    auto value_lower = str;
    return std::transform(value_lower.begin(), value_lower.end(), value_lower.begin(),
                          [](unsigned char c) { return std::tolower(c); }), value_lower;
}

}  // namespace tools::string_utils
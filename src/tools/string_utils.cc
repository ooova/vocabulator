#include "string_utils.h"

#include <sstream>

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
    auto count{0};
    while ((count != str.size()) && ((str.at(count) == ' ') || (str.at(count) == '\t'))) {
        ++count;
    }
    str.erase(0, count);
}

void removeSuffixSpacesAndTabs(std::string& str)
{
    auto index{str.size()};
    while ((0 < index) && ((str.at(index - 1) == ' ') || (str.at(index - 1) == '\t'))) {
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

}  // namespace tools::string_utils
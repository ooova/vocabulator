#ifndef TOOLS_STRING_UTILS_H
#define TOOLS_STRING_UTILS_H

#include <string>
#include <string_view>
#include <vector>

namespace tools::string_utils {

void removePrefix(std::string& str, char ch);

void removeSuffix(std::string& str, char ch);

void removePrefixSpacesAndTabs(std::string& str);

void removeSuffixSpacesAndTabs(std::string& str);

void trim(std::string& str);

std::vector<std::string> split(std::string_view str, char delimiter);

std::string codepoint_to_utf8(int codepoint);

}  // namespace tools::string_utils

#endif  // TOOLS_STRING_UTILS_H

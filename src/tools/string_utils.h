#ifndef TOOLS_STRING_UTILS_H
#define TOOLS_STRING_UTILS_H

#include <string_view>

namespace tools {

void removePrefix(std::string_view& s, char ch);

void removeSuffix(std::string_view& s, char ch);

void removePrefixSpacesAndTabs(std::string_view& s);

void removeSuffixSpacesAndTabs(std::string_view& s);

}  // namespace tools

#endif  // TOOLS_STRING_UTILS_H

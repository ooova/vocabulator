#include "string_utils.h"

namespace tools {

void removePrefix(std::string_view& s, char ch)
{
    while (!s.empty() && (s.front() == ch)) {
        s.remove_prefix(1);
    }
}

void removeSuffix(std::string_view& s, char ch)
{
    while (!s.empty() && (s.back() == ch)) {
        s.remove_suffix(1);
    }
}

void removePrefixSpacesAndTabs(std::string_view& s)
{
    while (!s.empty() && ((s.front() == ' ') || (s.front() == '\t'))) {
        s.remove_prefix(1);
    }
}

void removeSuffixSpacesAndTabs(std::string_view& s)
{
    while (!s.empty() && ((s.back() == ' ') || (s.back() == '\t'))) {
        s.remove_suffix(1);
    }
}

}  // namespace tools
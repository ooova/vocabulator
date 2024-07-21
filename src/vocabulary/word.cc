#include "vocabulary/word.h"

#include "common/exceptions/parsing_error.h"
#include "tools/string_utils.h"

// #include <algorithm>
#include <stdexcept>

namespace vocabulary {

Word Word::parse(std::string_view str, char item_delim, char field_delim)
{
    tools::removePrefixSpacesAndTabs(str);
    tools::removePrefix(str, field_delim);
    tools::removePrefixSpacesAndTabs(str);

    const auto end{str.find(field_delim)};
    const auto word{str.substr(0, end)};

    tools::removeSuffixSpacesAndTabs(str);
    tools::removeSuffix(str, field_delim);
    tools::removeSuffixSpacesAndTabs(str);

    if (word.find(item_delim) != std::string::npos) {
        throw ParsingError{"\'word\' can contain only one variant"};
    }

    if (word.empty()) {
        throw ParsingError{"word can not be empty string"};
    }

    str.remove_prefix(end == std::string::npos ? str.size() : end);
    return {word, Translation::parse(str, item_delim, field_delim)};
}

Word::Word(std::string_view word, Translation&& translation)
    : word_{word}
    , translation_{std::move(translation)}
{
    if (word_.empty()) {
        throw std::invalid_argument{"word can not be empty string"};
    }
    setDelimiters(translation_.delimiters().first, translation_.delimiters().second);
}

std::string Word::word() const { return word_; }

void Word::addTranslation(Translation&& translation)
{
    for (auto const& v : translation.variants()) {
        translation_.addVariant(v);
    }
    for (auto const& e : translation.examples()) {
        translation_.addExample(e);
    }
    // std::for_each(translation.variants().begin(), translation.variants().end(),
    //               [&translation_=translation_](const auto& element) {
    //               translation_.addVariant(element); });
    // std::for_each(translation.examples().begin(), translation.examples().end(),
    //               [&translation_=translation_](const auto& element) {
    //               translation_.addExample(element); });
}

Translation const& Word::translation() const { return translation_; }

std::string Word::toString() const
{
    return field_delimiter_ + word_ + field_delimiter_ + translation_.toString() +
           field_delimiter_;
}

uint8_t Word::retentionPercentage() const { return {}; }

std::vector<uint8_t> Word::toBin() const { return {}; }

void Word::setDelimiters(char item_delim, char field_delim)
{
    item_delimiter_ = item_delim;
    field_delimiter_ = field_delim;
}

}  // namespace vocabulary

#include "vocabulary/word.h"

#include "common/exceptions/parsing_error.h"
#include "spdlog/spdlog.h"
#include "tools/string_utils.h"

// #include <algorithm>
#include <stdexcept>

namespace vocabulary {

Word Word::parse(std::string_view str, char item_delim, char field_delim)
{
    auto string{std::string{str}};

    spdlog::trace("string to parse: {}", string);

    tools::string_utils::removePrefixSpacesAndTabs(string);
    tools::string_utils::removePrefix(string, field_delim);

    const auto end{string.find(field_delim)};
    auto word{string.substr(0, end)};

    tools::string_utils::trim(word);

    if (word.find(item_delim) != std::string::npos) {
        throw ParsingError{"\'word\' can contain only one variant"};
    }

    if (word.empty()) {
        throw ParsingError{"word can not be empty string"};
    }

    string.erase(0, (end == std::string::npos ? string.size() : end));
    return {word, Translation::parse(string, item_delim, field_delim)};
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

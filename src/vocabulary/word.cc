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

Word::Word(std::string_view word, Translation&& translation, int impressions_number,
           int know_number)
    : dont_know_pressed_number_{impressions_number}
    , know_pressed_number_{know_number}
    , word_{word}
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

uint8_t Word::retentionRate() const {
    if (know_pressed_number_ + dont_know_pressed_number_ == 0) {
        return 0;
    }

    return static_cast<uint8_t>(100.0f * know_pressed_number_ / (know_pressed_number_ + dont_know_pressed_number_));
}

void Word::setDelimiters(char item_delim, char field_delim)
{
    item_delimiter_ = item_delim;
    field_delimiter_ = field_delim;
}

bool Word::operator<(Word const& other) const
{
    return retentionRate() < other.retentionRate();
}

bool Word::operator==(Word const& other) const
{
    return word_ == other.word_;// && translation_ == other.translation_;
}

// json serialization and deserialization

void to_json(nlohmann::json& j, Word const& w)
{
    j = nlohmann::json{{"word", w.word()}, {"translation", w.translation()},
                       {"dont_know", w.dontKnowNumber()}, {"know", w.knowNumber()}};
}

void from_json(nlohmann::json const& j, Word& w)
{
    std::string word = j.at("word");
    Translation translation = j.at("translation");
    int dont_know_number = j.at("dont_know");
    int know_number = j.at("know");
    w = Word(word, std::move(translation), dont_know_number, know_number);
}

bool operator<(std::reference_wrapper<Word> lhs, std::reference_wrapper<Word> rhs)
{
    return lhs.get() < rhs.get();
}

bool operator==(std::reference_wrapper<Word> lhs, std::reference_wrapper<Word> rhs)
{
    return lhs.get() == rhs.get();
}

}  // namespace vocabulary

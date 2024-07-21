#include "vocabulary/translation.h"

#include <numeric>  // std::accumulate

#include "common/exceptions/parsing_error.h"
#include "spdlog/spdlog.h"
#include "tools/string_utils.h"

namespace {

std::string serializeVector(std::vector<std::string> const& values, char delimiter)
{
    if (values.empty()) {
        return {};
    }

    return std::accumulate(
        std::next(values.begin()), values.end(), values.front(),
        [delimiter](auto const& a, auto const& b) { return a + delimiter + b; });
}

std::vector<std::string> parseItems(std::string_view str, char const delimiter)
{
    auto result{std::vector<std::string>{}};

    const auto throw_if_empty = [](auto const& str) {
        if (str.empty()) {
            auto const message{"item field is empty"};
            spdlog::critical(message);
            throw ParsingError(message);
        }
    };

    while (str.size()) {
        tools::removePrefixSpacesAndTabs(str);
        auto itemEndIndex{str.find(delimiter)};
        auto str_to_save = str.substr(0, itemEndIndex);
        tools::removeSuffixSpacesAndTabs(str_to_save);
        throw_if_empty(str_to_save);
        result.emplace_back(str_to_save);
        if (itemEndIndex == std::string::npos) {
            break;
        }
        str.remove_prefix(itemEndIndex + 1);
    }

    return result;
}

}  // namespace

namespace vocabulary {

Translation Translation::parse(std::string_view str, char const item_delim,
                               char const field_delim)
{
    spdlog::trace("{}(): string to be parsed: \"{}\" string size = {}", __FUNCTION__, str,
                  str.size());

    std::vector<std::string> variants{};
    std::vector<std::string> examples{};

    tools::removePrefixSpacesAndTabs(str);
    tools::removePrefix(str, field_delim);
    tools::removePrefixSpacesAndTabs(str);

    tools::removeSuffixSpacesAndTabs(str);
    tools::removeSuffix(str, field_delim);
    tools::removeSuffixSpacesAndTabs(str);

    if (str.empty()) {
        auto const message{"parsing error: input string is empty"};
        spdlog::critical(message);
        throw ParsingError(message);
    }

    auto fieldEndIndex{str.find(field_delim)};

    variants = parseItems(str.substr(0, fieldEndIndex), item_delim);

    if (variants.empty()) {
        auto const message{"parsing error: \"variants\" of the translation is empty"};
        spdlog::critical(message);
        throw ParsingError(message);
    }

    if (fieldEndIndex != std::string::npos) {
        str.remove_prefix(fieldEndIndex + 1);
        examples = parseItems(str, item_delim);
    }

    auto msg{std::string{"parsed translation:"}};
    for (auto const& v : variants) {
        msg += ' ' + v;
    }
    msg += " |";
    for (auto const& e : examples) {
        msg += ' ' + e;
    }
    spdlog::trace("{}(): {}", __FUNCTION__, msg);

    return {variants, examples, item_delim, field_delim};
}

Translation::Translation(std::vector<std::string> const& variants,
                         std::vector<std::string> const& examples, char item_delim,
                         char field_delim)
    : variants_{variants}
    , examples_{examples}
{
    if (variants_.empty()) {
        throw std::invalid_argument{"translation variants can not be empty"};
    }
    setDelimiters(item_delim, field_delim);
}

void Translation::setDelimiters(char item_delim, char field_delim)
{
    item_delimiter_ = item_delim;
    field_delimiter_ = field_delim;
}

std::pair<char, char> Translation::delimiters() const
{
    return {item_delimiter_, field_delimiter_};
}

std::string Translation::toString() const
{
    auto examples = examplesToString();
    return variantsToString() + (examples.empty() ? "" : field_delimiter_ + examples);
}

std::string Translation::variantsToString() const
{
    return serializeVector(variants_, item_delimiter_);
}

std::string Translation::examplesToString() const
{
    return serializeVector(examples_, item_delimiter_);
}

std::vector<std::string> Translation::variants() const { return variants_; }

std::vector<std::string> Translation::examples() const { return examples_; }

std::vector<uint8_t> Translation::toBin() const { return {}; }

void Translation::addVariant(std::string_view variant)
{
    variants_.emplace_back(variant);
}

void Translation::addExample(std::string_view example)
{
    examples_.emplace_back(example);
}

}  // namespace vocabulary

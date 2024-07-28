#include "vocabulary/translation.h"

#include <algorithm>
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

void removeEmptyStrings(std::vector<std::string>& strs)
{
    strs.erase(std::remove_if(strs.begin(), strs.end(),
                              [](std::string& s) {
                                  tools::string_utils::trim(s);
                                  return s.empty();
                              }),
               strs.end());
}

}  // namespace

namespace vocabulary {

Translation Translation::parse(std::string_view str, char const item_delim,
                               char const field_delim)
{
    spdlog::trace("{}(): string to be parsed: \"{}\" string size = {}", __FUNCTION__, str,
                  str.size());

    auto s{std::string{str}};

    tools::string_utils::trim(s);
    tools::string_utils::removePrefix(s, field_delim);
    tools::string_utils::removeSuffix(s, field_delim);

    std::vector<std::string> parts = tools::string_utils::split(s, field_delim);
    removeEmptyStrings(parts);
    if (parts.empty()) {
        auto const message{"parsing error: string doesn't contain anything useful"};
        // spdlog::critical(message);
        throw ParsingError(message);
    }

    std::vector<std::string> variants =
        tools::string_utils::split(parts.at(0), item_delim);
    removeEmptyStrings(variants);

    if (variants.empty()) {
        auto const message{"parsing error: \"variants\" of the translation are empty"};
        // spdlog::critical(message);
        throw ParsingError(message);
    }

    std::vector<std::string> examples{};
    if (1 < parts.size()) {
        examples = tools::string_utils::split(parts.at(1), item_delim);
        removeEmptyStrings(examples);
    }

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

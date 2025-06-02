#ifndef VOCABULARY_TRANSLATION_H
#define VOCABULARY_TRANSLATION_H

#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <cstdint>

#include "nlohmann/json.hpp"

namespace vocabulary {

class Translation {
public:
    static char const kDefaultItemsDelimiter{';'};
    static char const kDefaultFieldsDelimiter{'|'};

    Translation() = default;  // needed for json deserialization

    /**
     * @param str Expected format | variant1; variant1; | example1; example2 |
     * @throw ParsingError if str arg is empty or fields are empty
     */
    static Translation parse(std::string_view str,
                             char item_delim = kDefaultItemsDelimiter,
                             char field_delim = kDefaultFieldsDelimiter);

    /**
     * @throw ParsingError variants parameter is empty
     */
    Translation(std::vector<std::string> const& variants,
                std::vector<std::string> const& examples = {},
                char item_delim = kDefaultItemsDelimiter,
                char field_delim = kDefaultFieldsDelimiter);
    Translation(Translation&& other) = default;
    Translation& operator=(Translation&& other) = default;
    Translation(Translation const& other) = delete;
    Translation& operator=(Translation const& other) = delete;
    // Translation(Translation const& other) = default;
    // Translation& operator=(Translation const& other) = default;

    void setDelimiters(char item_delim, char field_delim);
    std::pair<char, char> delimiters() const;

    std::string toString() const;
    std::string variantsToString() const;
    std::string examplesToString() const;
    std::vector<std::string> variants() const;
    std::vector<std::string> examples() const;
    std::vector<uint8_t> toBin() const;

    void addVariant(std::string_view variant);
    void addExample(std::string_view example);

private:
    std::vector<std::string> variants_;
    std::vector<std::string> examples_;
    inline static char item_delimiter_{kDefaultItemsDelimiter};
    inline static char field_delimiter_{kDefaultFieldsDelimiter};
};

void to_json(nlohmann::json& j, Translation const& t);
void from_json(nlohmann::json const& j, Translation& t);

}  // namespace vocabulary

#endif  // VOCABULARY_TRANSLATION_H
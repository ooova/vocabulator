#ifndef VOCABULARY_WORD_H
#define VOCABULARY_WORD_H

#include <cstdint>
#include <string>
#include <string_view>

#include "vocabulary/translation.h"

namespace vocabulary {

class Word {
public:
    static char const kDefaultItemsDelimiter{';'};
    static char const kDefaultFieldsDelimiter{'|'};
    /**
     * @throw std::invalid_argument
     */
    static Word parse(std::string_view str, char item_delim = kDefaultItemsDelimiter,
                      char field_delim = kDefaultFieldsDelimiter);

    /**
     * @throw std::invalid_argument if word is empty string
     */
    Word(std::string_view word, Translation&& translation);
    Word(Word&& other) = default;
    Word& operator=(Word&& other) = default;
    Word(Word const& other) = delete;
    Word& operator=(Word const& other) = delete;

    std::string word() const;
    void addTranslation(Translation&& translation);
    Translation const& translation() const;
    std::string toString() const;
    uint8_t retentionPercentage() const;
    std::vector<uint8_t> toBin() const;
    void setDelimiters(char item_delim, char field_delim);

private:
    int impressions_number_{};
    int know_number_{};
    std::string word_;
    Translation translation_;

    inline static char item_delimiter_{kDefaultItemsDelimiter};
    inline static char field_delimiter_{kDefaultFieldsDelimiter};
};

}  // namespace vocabulary

#endif  // VOCABULARY_WORD_H

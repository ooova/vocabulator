#ifndef VOCABULARY_WORD_H
#define VOCABULARY_WORD_H

#include <cstdint>
#include <string>
#include <string_view>

#include "vocabulary/translation.h"

#include "nlohmann/json.hpp"

namespace vocabulary {

class Word {
public:
    static char const kDefaultItemsDelimiter{';'};
    static char const kDefaultFieldsDelimiter{'|'};

    Word() = default; // needed for json deserialization

    /**
     * @throw std::invalid_argument
     */
    static Word parse(std::string_view str, char item_delim = kDefaultItemsDelimiter,
                      char field_delim = kDefaultFieldsDelimiter);

    /**
     * @throw std::invalid_argument if word is empty string
     */
    Word(std::string_view word, Translation&& translation, int impressions_number = 0,
         int know_number = 0);
    Word(Word&& other) = default;
    Word& operator=(Word&& other) = default;
    Word(Word const& other) = delete;
    Word& operator=(Word const& other) = delete;

    std::string word() const;
    void addTranslation(Translation&& translation);
    Translation const& translation() const;
    std::string toString() const;

    uint8_t retentionRate() const;
    int dontKnowNumber() const { return dont_know_pressed_number_; }
    int knowNumber() const { return know_pressed_number_; }
    void know() { ++know_pressed_number_; }
    void dontKnow() { ++dont_know_pressed_number_; }

    void setDelimiters(char item_delim, char field_delim);

    bool operator<(Word const& other);
    bool operator==(Word const& other);

private:
    int dont_know_pressed_number_{};
    int know_pressed_number_{};
    std::string word_;
    Translation translation_;

    inline static char item_delimiter_{kDefaultItemsDelimiter};
    inline static char field_delimiter_{kDefaultFieldsDelimiter};
};

void to_json(nlohmann::json& j, Word const& w);
void from_json(nlohmann::json const& j, Word& w);

bool operator<(std::reference_wrapper<Word> lhs, std::reference_wrapper<Word> rhs);

}  // namespace vocabulary

#endif  // VOCABULARY_WORD_H

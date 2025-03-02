#ifndef VOCABULARY_VOCABULARY_H
#define VOCABULARY_VOCABULARY_H

#include <filesystem>
#include <string_view>
#include <vector>

#include "common/exceptions/vocabulary_error.h"
#include "vocabulary/translation.h"
#include "vocabulary/word.h"

namespace vocabulary {

class Vocabulary final {
public:
    struct Statistic {
        size_t words_count{};
        size_t known_words_count{};
        size_t in_progress_words_count{};
        size_t new_words_count{};
    };

public:
    static char const kDefaultItemsDelimiter{';'};
    static char const kDefaultFieldsDelimiter{'|'};

    Vocabulary() = default;
    Vocabulary(std::filesystem::path const& import_from,
               char item_delim = kDefaultItemsDelimiter,
               char field_delim = kDefaultFieldsDelimiter);

    /**
     * @throw VocabularyError if word is not found
     */
    Translation const& translate(std::string_view const word);
    // void setStrategy(std::weak_ptr<LearningStrategies::Strategy> strategy);

    Statistic getStatistic() const;

    void addWord(Word&& word);
    void addWord(std::string_view const word, Translation&& translation);

    // importing and exporting of words
    void importFromFile(std::filesystem::path const& path,
                        char item_delim = kDefaultItemsDelimiter,
                        char field_delim = kDefaultFieldsDelimiter);
    void exportToFile(std::filesystem::path const& path);
    // void addWordsFromFile(std::filesystem::path const& path);

    // loading and saving of whole vocabulary
    void loadFromFile(std::filesystem::path const& path,
                      char item_delim = kDefaultItemsDelimiter,
                      char field_delim = kDefaultFieldsDelimiter){
                        (void)path;
                        (void)item_delim;
                        (void)field_delim;
                      };
    void saveToFile(std::filesystem::path const& path){ (void)path; };

    /**
     * @throw VocabularyError if there is no word to learn
     */
    Word& nextWordToLearn();

    std::vector<Word> const& words() const;

private:
    const uint8_t kRetentionPercentageForKnownWord{90};
    size_t next_word_index_{0};
    std::vector<Word> words_;
    // std::weak_ptr<LearningStrategies::Strategy> learning_strategy_;

    /**
     * @throw VocabularyError if word is not found
     */
    Word& findWord(std::string_view const word);
};

}  // namespace vocabulary

#endif  // VOCABULARY_VOCABULARY_H

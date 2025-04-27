#ifndef VOCABULARY_VOCABULARY_H
#define VOCABULARY_VOCABULARY_H

#include <filesystem>
#include <string_view>
#include <vector>
#include <optional>
#include <functional>
#include <string>

#include "common/exceptions/vocabulary_error.h"
#include "vocabulary/translation.h"
#include "vocabulary/word.h"

#include "nlohmann/json.hpp"

namespace vocabulary {

constexpr const auto kDefaultVocabularyJsonPath{std::string_view{"../assets/vocabulary.json"}};

class Vocabulary final {
public:
    using WordRef = std::optional<std::reference_wrapper<Word>>;

    struct Statistic {
        size_t words_count{};
        size_t known_words_count{};
        size_t in_progress_words_count{};
        size_t new_words_count{};
    };

public:
    static char const kDefaultItemsDelimiter{';'};
    static char const kDefaultFieldsDelimiter{'|'};
    static int const kMaxWordsToLearn{15};

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

    void importFromFile(std::filesystem::path const& path,
                        char item_delim = kDefaultItemsDelimiter,
                        char field_delim = kDefaultFieldsDelimiter);
    void exportToFile(std::filesystem::path const& path);

    void importFromJsonFile(std::filesystem::path const& path);
    void exportToJsonFile(std::filesystem::path const& path) const;

    std::vector<Word> const& words() const;

    bool addUnknownWordToBatch();
    bool removeWordFromBatch(std::string_view const word);
    WordRef wordToLearnFromBatch();
    size_t batchSize() const { return batch_.words_to_learn.size(); }

private:
    struct Batch {
        std::vector<std::string> words_to_learn{};
        bool addWord(std::string_view const word);
        bool removeWord(std::string_view const word);
        // std::string getWordToLearn();
    };

    const uint8_t kRetentionRateForKnownWord{90};
    size_t next_word_index_{0};
    std::vector<Word> words_;
    size_t last_word_added_to_batch_{0};
    // current batch of words to learn
    Batch batch_;

    // std::weak_ptr<LearningStrategies::Strategy> learning_strategy_;
    std::filesystem::path import_from_file_path_{};

    /**
     * @throw VocabularyError if word is not found
     */
    WordRef findWord(std::string_view const word);

    /**
     * @throw VocabularyError if there is no word to learn
     */
    WordRef nextRandomWordToLearn();

    WordRef nextUnknownWordToLearn();
};

}  // namespace vocabulary

#endif  // VOCABULARY_VOCABULARY_H

#ifndef VOCABULARY_VOCABULARY_H
#define VOCABULARY_VOCABULARY_H

#include <filesystem>
#include <string_view>
#include <vector>
#include <deque>
#include <functional>
#include <string>
#include <memory>

#include "common/exceptions/vocabulary_error.h"
#include "vocabulary/translation.h"
#include "vocabulary/word.h"

#include "nlohmann/json.hpp"

namespace vocabulary {

constexpr const auto kDefaultVocabularyJsonPath{std::string_view{"../assets/vocabulary.json"}};

class Vocabulary final {
public:
    using WordWeakPtr = std::weak_ptr<Word>;

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

    bool addUnknownWordToBatch();
    WordWeakPtr nextWordToLearnFromBatch();
    size_t batchSize() const { return batch_.size(); }

private:
    using Batch = std::deque<WordWeakPtr>;

    std::vector<std::shared_ptr<Word>> words_;
    Batch batch_;

    // for retention rate in percentage
    // const uint8_t kRetentionRateForKnownWord{85};
    // for retention rate as "know" - "don't know" difference
    const uint8_t kRetentionRateForKnownWord{3};

    size_t next_word_index_{0};
    size_t next_word_to_added_to_batch_{0};

    /**
     * @throw VocabularyError if word is not found
     */
    WordWeakPtr findWord(std::string_view const word);

    WordWeakPtr nextUnknownWordToLearn();

    bool addWordToBatch(std::string_view const word);
};

void to_json(nlohmann::json& j, std::vector<std::shared_ptr<Word>> const& words);
void from_json(nlohmann::json const& j, std::vector<std::shared_ptr<Word>>& words);

void to_json(nlohmann::json& j, std::deque<std::weak_ptr<Word>> const& batch);
void from_json(nlohmann::json const& j, std::deque<std::weak_ptr<Word>>& batch);

}  // namespace vocabulary

#endif  // VOCABULARY_VOCABULARY_H

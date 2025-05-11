#include "vocabulary.h"

#include <array>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <stdexcept>

#include "spdlog/spdlog.h"
#include "tools/random_number.h"

namespace vocabulary {

Vocabulary::Vocabulary(std::filesystem::path const& import_from, char item_delim,
                       char field_delim)
{
    import_from_file_path_ = import_from;
    importFromFile(import_from, item_delim, field_delim);
}

Translation const& Vocabulary::translate(std::string_view const word)
{
    auto w = findWord(word);
    if (!w) {
        auto const msg{fmt::format("{}(): word \'{}\' is not found in the vocabulary",
                                   __FUNCTION__, word)};
        throw VocabularyError(msg);
    }
    return w->get().translation();
}

// void setStrategy(std::weak_ptr<LearningStrategies::Strategy> strategy);

Vocabulary::Statistic Vocabulary::getStatistic() const
{
    auto result{Statistic{.words_count = words_.size()}};

    for (auto const& w : words_) {
        if (w.retentionRate() > kRetentionRateForKnownWord) {
            ++result.known_words_count;
        } else if (w.retentionRate() == 0) {
            ++result.new_words_count;
        } else {
            ++result.in_progress_words_count;
        }
    }
    return result;
}

void Vocabulary::addWord(std::string_view const word, Translation&& translation)
{
    addWord({word, std::move(translation)});
}

void Vocabulary::addWord(Word&& word)
{
    spdlog::trace("{}(): new word: {}", __FUNCTION__, word.toString());
    words_.push_back(std::move(word));
}

void Vocabulary::importFromFile(std::filesystem::path const& path, char item_delim,
                                char field_delim)
{
    auto path_ = path.empty() ? import_from_file_path_ : path;
    std::ifstream inputFile(path_ /* , std::ios::app */);
    if (!inputFile) {
        auto const msg{
            fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path_.string())};
        throw VocabularyError(msg);
    }

    try {
        for (std::array<char, 1024> a{'\0'}; inputFile.getline(&a[0], a.size());) {
            words_.push_back(
                Word::parse(/* std::string_view( */ a.data(), item_delim, field_delim));
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
        throw;
    }

    spdlog::info("vocabulary \'{}\' successfully imported. Words count: {}",
                 path_.string(), words_.size());
}

void Vocabulary::exportToFile(std::filesystem::path const& path)
{
    auto path_ = path.empty() ? import_from_file_path_ : path;
    std::ofstream outputFile(path_ /* , std::ios::app */);
    try {
        for (auto const& word : words_) {
            outputFile << word.toString() << '\n';
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }

    spdlog::info("vocabulary successfully exported to the file \'{}\'", path_.string());
}

void Vocabulary::importFromJsonFile(std::filesystem::path const& path)
{
    auto path_ = path.empty() ? kDefaultVocabularyJsonPath : path;
    std::ifstream inputFile(path_);
    if (!inputFile) {
        auto const msg{
            fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path_.string())};
        throw VocabularyError(msg);
    }

    try {
        auto j = nlohmann::json::parse(inputFile);

        words_ = j.at("vocabulary");
        last_word_added_to_batch_ =
            j.at("batch_to_learn").at("last_word_added_to_batch").get<size_t>();
        const auto& words_to_learn_json = j.at("batch_to_learn").at("words");
        for (auto const& w : words_to_learn_json) {
            addWordToBatch(std::string(w));
        }
    }
    catch (std::exception const& ex) {
        auto const msg{
            fmt::format("{}(): failed to parse json file \'{}\': {}", __FUNCTION__, path_.string(), ex.what())};
        throw VocabularyError(msg);
    }
}

void Vocabulary::exportToJsonFile(std::filesystem::path const& path) const
{
    auto path_ = path.empty() ? kDefaultVocabularyJsonPath : path;
    std::ofstream outputFile(path_/* , std::ios::app */);
    try {
        if (!outputFile) {
            auto const msg{
                fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path_.string())};
            throw VocabularyError(msg);
        }
        nlohmann::json j;
        j["vocabulary"] = words_;
        j["batch_to_learn"]["words"] = nlohmann::json::array();
        for (auto const& w : batch_.words_to_learn) {
            j["batch_to_learn"]["words"].push_back(w->get().word());
        }
        j["batch_to_learn"]["last_word_added_to_batch"] = last_word_added_to_batch_;
        outputFile << j.dump(4);

    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

std::vector<Word> const& Vocabulary::words() const { return words_; }

bool Vocabulary::addUnknownWordToBatch()
{
    if (batch_.words_to_learn.size() >= kMaxWordsToLearn) {
        spdlog::warn("batch to learn is full");
        return false;
    }

    auto word_added{false};
    WordRef word{std::nullopt};
    do {
        word = nextUnknownWordToLearn();
    // while (!word_added && word.has_value()) {
        word_added = batch_.addWord(word);
        // word = nextUnknownWordToLearn();
    } while (!word_added && word.has_value());

    return word_added;
}

bool Vocabulary::removeWordFromBatch(std::string_view const word)
{
    return batch_.removeWord(word);
}

// Check words in a batch and return a first word with the lowest retention rate.
// If retention rate of a word is greater then or equal to kRetentionRateForKnownWord
// then this word is considered as known and will be removed from the batch.
Vocabulary::WordRef Vocabulary::nextWordToLearnFromBatch()
{
    if (batch_.words_to_learn.empty()) {
        spdlog::warn("batch to learn is empty");
        return std::nullopt;
    }

    while ((batch_.last_word_to_learn < batch_.words_to_learn.size()) &&
           (kRetentionRateForKnownWord <=
            batch_.words_to_learn.at(batch_.last_word_to_learn)->get().retentionRate())) {
        batch_.removeWord(batch_.last_word_to_learn);
    }

    if (batch_.words_to_learn.empty()) {
        spdlog::warn("batch to learn is empty");
        return std::nullopt;
    }

    if (batch_.last_word_to_learn >= batch_.words_to_learn.size()) {
        batch_.last_word_to_learn = 0;
    }

    auto word = batch_.words_to_learn.at(batch_.last_word_to_learn++);

    spdlog::trace("word to learn: {}", word->get().word());
    return word;
    // std::set<WordRef> words_to_learn{};
    // for (auto& w : batch_.words_to_learn) {
    //     // try to find the word 'w' and check it's retention rate
    //     // if the retention rate is not enough, add the word to the set
    //     if (auto word = findWord(w); word.has_value()) {
    //         if (word->get().retentionRate() < kRetentionRateForKnownWord) {
    //             words_to_learn.insert(word.value());
    //         }
    //     }
    //     // if the retention rate is enough - remove the word form the batch
    //     else {
    //         batch_.removeWord(w);
    //     }
    // }

    // if (!words_to_learn.empty()) {
    //     auto word = words_to_learn.begin()->value();
    //     spdlog::trace("word to learn: {}", word.get().word());
    //     return word;
    // }

    // spdlog::warn("No words to learn or a batch is empty");
    // return std::nullopt;
}

// private ================================================

Vocabulary::WordRef Vocabulary::findWord(std::string_view const word)
{
    for (auto& w : words_) {
        if (w.word() == word) {
            return w;
        }
    }

    spdlog::error("{}(): word {} is not found in the vocabulary", __FUNCTION__, word);

    return std::nullopt;
}

Vocabulary::WordRef Vocabulary::nextRandomWordToLearn()
{
    if (words_.empty()) {
        spdlog::warn("Nothing to learn. Vocabulary is empty");
        return std::nullopt;
    }

    auto word_index = tools::getRandomNumber(0, words_.size() - 1);
    try {
        return words_.at(word_index);
    } catch (std::exception const& e) {
        spdlog::error("can not get random word at index {}: {}", word_index, e.what());
    }

    return std::nullopt;
}

Vocabulary::WordRef Vocabulary::nextUnknownWordToLearn()
{
    if (words_.empty()) {
        spdlog::warn("Nothing to learn. Vocabulary is empty");
        return std::nullopt;
    }

    // for (auto& w : words_) {
    //     if (w.retentionRate() < kRetentionRateForKnownWord) {
    //         return w;
    //     }
    // }

    try {
        while (last_word_added_to_batch_ < words_.size()) {
            auto& w = words_.at(last_word_added_to_batch_++);
            if (w.retentionRate() < kRetentionRateForKnownWord) {
                spdlog::trace("word \'{}\' returned from nextUnknownWordToLearn()",
                              w.word());
                return w;
            }
        }
    } catch (std::exception const& e) {
        spdlog::error("can not get random word at index {}: {}",
                      last_word_added_to_batch_, e.what());
    }

    spdlog::warn("{}(): no words to learn, you know everything! ᕕ(⌐■_■)ᕗ ♪♬",
                 __FUNCTION__);

    return std::nullopt;
}

bool Vocabulary::addWordToBatch(std::string_view const word)
{
    if (batch_.words_to_learn.size() >= kMaxWordsToLearn) {
        spdlog::warn("batch to learn is full");
        return false;
    }

    auto word_added{false};
    auto word_to_add = findWord(word);
    while (!word_added && word_to_add.has_value()) {
        word_added = batch_.addWord(word_to_add);
        word_to_add = findWord(word);
    }

    return word_added;
}

// structure Batch ========================

bool Vocabulary::Batch::addWord(WordRef word)
{
    if (!word.has_value()) {
        spdlog::warn("{}(): \'word\' argument is empty", __FUNCTION__);
        return false;
    }

    auto it = std::find_if(words_to_learn.begin(), words_to_learn.end(),
                        [&word](auto const& w) { return w.value() == word.value(); });
    if (it != words_to_learn.end()) {
        spdlog::warn("word \'{}\' is already in the batch", word->get().word());
        return false;
    }

    words_to_learn.push_back(word);
    spdlog::info("word \'{}\' added to batch", word->get().word());

    return true;
}

// bool Vocabulary::Batch::addWord(std::string_view const word)
// {
//     auto it = std::find_if(words_to_learn.begin(), words_to_learn.end(),
//                            [&word](auto const& w) { return w->get().word() == word; });

//     if (it != words_to_learn.end()) {
//         spdlog::warn("word \'{}\' is already in the batch", word);
//         return false;
//     }

//     if (auto w = findWord(word); w.has_value()) {
//         words_to_learn.push_back(w);
//         spdlog::info("word \'{}\' added to batch", w->get().word());

//         return true;
//     }

//     spdlog::error("word \'{}\' not found in vocabulary", word);
//     return false;
// }

bool Vocabulary::Batch::removeWord(std::string_view const word)
{
    auto it = std::remove_if(words_to_learn.begin(), words_to_learn.end(),
                          [&word](auto const w) { return w->get().word() == word; });
    if (it == words_to_learn.end()) {
        spdlog::warn("word {} is not found in the batch", word);
        return false;
    }

    words_to_learn.erase(it, words_to_learn.end());
    if (words_to_learn.size() <= last_word_to_learn) {
        last_word_to_learn = 0;
    }
    spdlog::info("word {} removed from batch", word);

    return true;
}

bool Vocabulary::Batch::removeWord(size_t index)
{
    if (index >= words_to_learn.size()) {
        spdlog::warn("index {} is out of range", index);
        return false;
    }

    words_to_learn.erase(std::next(words_to_learn.begin(), index));
    if (words_to_learn.size() == 0) {
        last_word_to_learn = 0;
    } else if (index < last_word_to_learn) {
        --last_word_to_learn;
    }
    spdlog::info("word at index {} removed from batch", index);

    return true;
}

}  // namespace vocabulary

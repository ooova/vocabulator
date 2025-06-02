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

Translation const& Vocabulary::translate(std::string_view const word)
{
    auto w = findWord(word).lock();
    if (!w) {
        auto const msg{fmt::format("{}(): word \'{}\' is not found in the vocabulary",
                                   __FUNCTION__, w->word())};
        throw VocabularyError(msg);
    }
    return w->translation();
}

Vocabulary::Statistic Vocabulary::getStatistic() const
{
    auto result{Statistic{.words_count = words_.size()}};

    for (auto const& w : words_) {
        if (w->retentionRate() > kRetentionRateForKnownWord) {
            ++result.known_words_count;
        }
    }
    result.in_progress_words_count = batch_.size();
    result.new_words_count = result.words_count - result.known_words_count - result.in_progress_words_count;
    return result;
}

void Vocabulary::addWord(std::string_view const word, Translation&& translation)
{
    addWord({word, std::move(translation)});
}

void Vocabulary::addWord(Word&& word)
{
    spdlog::trace("{}(): new word added to vocabulary: {}", __FUNCTION__, word.toString());
    words_.push_back(std::make_shared<Word>(std::move(word)));
}

void Vocabulary::importFromFile(std::filesystem::path const& path, char item_delim,
                                char field_delim)
{
    std::ifstream inputFile(path);
    if (!inputFile) {
        auto const msg{
            fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path.string())};
        throw VocabularyError(msg);
    }

    try {
        for (std::array<char, 1024> a{'\0'}; inputFile.getline(&a[0], a.size());) {
            words_.push_back(
                std::make_shared<Word>(
                    Word::parse(a.data(), item_delim, field_delim)
            ));
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
        throw;
    }

    spdlog::info("vocabulary \'{}\' successfully imported. Words count: {}",
        path.string(), words_.size());
}

void Vocabulary::exportToFile(std::filesystem::path const& path)
{
    std::ofstream outputFile(path);
    try {
        for (auto const& word : words_) {
            outputFile << word->toString() << '\n';
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }

    spdlog::info("vocabulary successfully exported to the file \'{}\'", path.string());
}

void Vocabulary::importFromJsonFile(std::filesystem::path const& path)
{
    std::ifstream inputFile(path);
    if (!inputFile) {
        auto const msg{
            fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path.string())};
        throw VocabularyError(msg);
    }

    try {
        auto j = nlohmann::json::parse(inputFile);

        words_ = j.at("vocabulary");
        next_word_to_added_to_batch_ =
            j.at("batch_to_learn").at("next_word_to_added_to_batch").get<size_t>();
        const auto& words_to_learn_json = j.at("batch_to_learn").at("words");
        batch_.clear();
        for (auto const& w : words_to_learn_json) {
            addWordToBatch(std::string(w));
        }
    }
    catch (std::exception const& ex) {
        auto const msg{
            fmt::format("{}(): failed to parse json file \'{}\': {}", __FUNCTION__, path.string(), ex.what())};
        throw VocabularyError(msg);
    }
}


void Vocabulary::exportToJsonFile(std::filesystem::path const& path) const
{
    std::ofstream outputFile(path);

    if (!outputFile) {
        auto const msg{
            fmt::format("{}(): failed to open \'{}\'", __FUNCTION__, path.string())};
        throw VocabularyError(msg);
    }
    try {
        nlohmann::json j;
        j["vocabulary"] = words_;
        j["batch_to_learn"]["words"] = nlohmann::json::array();
        for (auto const& word : batch_) {
            j["batch_to_learn"]["words"].push_back(word.lock()->word());
        }
        j["batch_to_learn"]["next_word_to_added_to_batch"] = next_word_to_added_to_batch_;
        outputFile << j.dump(4);

    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

bool Vocabulary::addUnknownWordToBatch()
{
    if (batch_.size() >= kMaxWordsToLearn) {
        spdlog::warn("batch to learn is full");
        return false;
    }

    auto w{nextUnknownWordToLearn()};
    if (w.expired()) {
        return false;
    }

    batch_.push_back(w);
    return true;
}

// // how to remove word from queue if it's in the middle of the queue..?
// bool Vocabulary::removeWordFromBatch(std::string_view const word)
// {

// }

Vocabulary::WordWeakPtr Vocabulary::nextWordToLearnFromBatch()
{
    if (batch_.empty()) {
        spdlog::warn("batch is empty");
        return {};
    }

    while (!batch_.empty()) {
        auto w = batch_.front();
        batch_.pop_front();
        if (auto word = w.lock()) {
            if (word->retentionRate() < kRetentionRateForKnownWord) {
                batch_.push_back(w);  // put it back to the deque
                spdlog::trace("word \'{}\' returned from nextWordToLearnFromBatch()",
                              word->word());
                return w;
            }
        }
    }

    spdlog::warn("no words to learn in the batch");
    return {};
}

// private ================================================

Vocabulary::WordWeakPtr Vocabulary::findWord(std::string_view const word)
{
    for (auto& w : words_) {
        if (w->word() == word) {
            return w;
        }
    }

    spdlog::error("{}(): word {} is not found in the vocabulary", __FUNCTION__, word);

    return {};
}

// Vocabulary::WordWeakPtr Vocabulary::nextRandomWordToLearn()
// {
//     if (words_.empty()) {
//         spdlog::warn("Nothing to learn. Vocabulary is empty");
//         return {};
//     }

//     auto word_index = tools::getRandomNumber(0, words_.size() - 1);
//     try {
//         return words_.at(word_index);
//     } catch (std::exception const& e) {
//         spdlog::error("can not get random word at index {}: {}", word_index, e.what());
//     }

//     return {};
// }

Vocabulary::WordWeakPtr Vocabulary::nextUnknownWordToLearn()
{
    if (words_.empty()) {
        spdlog::warn("Nothing to learn. Vocabulary is empty.");
        return {};
    }

    if (next_word_to_added_to_batch_ >= words_.size()) {
        spdlog::warn("All words are known or were already in batches. Reset batch counter to 0");
        next_word_to_added_to_batch_ = 0;  // reset index for next call
    }

    try {
        while (next_word_to_added_to_batch_ < words_.size()) {
            auto w = words_.at(next_word_to_added_to_batch_++);
            if (w->retentionRate() < kRetentionRateForKnownWord) {
                spdlog::trace("word \'{}\' returned from nextUnknownWordToLearn()",
                              w->word());
                return w;
            }
        }
    } catch (std::exception const& e) {
        spdlog::error("{}(): can not get next unknown word: {}", __FUNCTION__, e.what());
        return {};
    }

    spdlog::warn("{}(): no words to learn, you know everything! ᕕ(⌐■_■)ᕗ ♪♬",
                 __FUNCTION__);

    return {};
}

bool Vocabulary::addWordToBatch(std::string_view const word)
{
    if (batch_.size() >= kMaxWordsToLearn) {
        spdlog::warn("batch to learn is full");
        return false;
    }

    auto word_to_add{findWord(word)};
    if (word_to_add.expired()) {
        spdlog::warn("word \'{}\' is not found in the vocabulary", word);
        return false;
    }

    batch_.push_back(word_to_add);
    spdlog::trace("word \'{}\' added to the batch", word);

    return true;
}

void to_json(nlohmann::json& j, std::vector<std::shared_ptr<Word>> const& words)
{
    j = nlohmann::json::array();
    for (auto const& w : words) {
        j.push_back(*w);
    }
}

void from_json(nlohmann::json const& j, std::vector<std::shared_ptr<Word>>& words)
{
    words.clear();
    for (auto const& w : j) {
        words.push_back(std::make_shared<Word>(w));
    }
}

// void to_json(nlohmann::json& j, std::deque<std::weak_ptr<Word>> const& batch)
// {
//     j = nlohmann::json::array();
//     for (auto const& w : batch) {
//         if (auto word = w.lock()) {
//             j.push_back(*word);
//         } else {
//             j.push_back(nullptr);  // or some placeholder for weak_ptr
//         }
//     }
// }

// void from_json(nlohmann::json const& j, std::deque<std::weak_ptr<Word>>& batch)
// {
    // batch.clear();
    // for (auto const& w : j) {
    //     if (w.is_null()) {
    //         batch.push_back(std::weak_ptr<Word>{});  // or some placeholder for weak_ptr
    //     } else {
    //         batch.push_back(std::make_shared<Word>(w));
    //     }
    // }
// }

// void to_json(nlohmann::json& j, Vocabulary::Statistic const& stat)
// {
//     j = nlohmann::json{
//         {"words_count", stat.words_count},
//         {"known_words_count", stat.known_words_count},
//         {"in_progress_words_count", stat.in_progress_words_count},
//         {"new_words_count", stat.new_words_count}
//     };
// }

// void from_json(nlohmann::json const& j, Vocabulary::Statistic& stat)
// {
//     stat.words_count = j.at("words_count").get<size_t>();
//     stat.known_words_count = j.at("known_words_count").get<size_t>();
//     stat.in_progress_words_count = j.at("in_progress_words_count").get<size_t>();
//     stat.new_words_count = j.at("new_words_count").get<size_t>();
// }

}  // namespace vocabulary

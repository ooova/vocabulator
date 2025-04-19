#include "vocabulary.h"

#include <format>

#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
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
    return findWord(word).translation();
}

// void setStrategy(std::weak_ptr<LearningStrategies::Strategy> strategy);

Vocabulary::Statistic Vocabulary::getStatistic() const
{
    auto result{Statistic{.words_count = words_.size()}};

    for (auto const& w : words_) {
        if (w.retentionPercentage() > kRetentionPercentageForKnownWord) {
            ++result.known_words_count;
        } else if (w.retentionPercentage() == 0) {
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
        // spdlog::error(msg);
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

Word& Vocabulary::nextWordToLearn()
{
    if (words_.empty()) {
        const auto msg{"Nothing to learn. Vocabulary is empty"};
        throw VocabularyError(msg);
    }

    try {
        return words_.at(tools::getRandomNumber(0, words_.size()-1)/* next_word_index_++ */);
    } catch (...) {
        // spdlog::error("{}(): word with index {} does not exists", __FUNCTION__,
        //               next_word_index_);
    }

    next_word_index_ = 0;

    return words_.front();
}

std::vector<Word> const& Vocabulary::words() const { return words_; }

// private ================================================

Word& Vocabulary::findWord(std::string_view const word)
{
    for (auto& w : words_) {
        if (w.word() == word) {
            return w;
        }
    }
    auto const msg{
        fmt::format("{}(): word {} is not found in the vocabulary", __FUNCTION__, word)};
    // spdlog::error(msg);
    throw VocabularyError(msg);
}

}  // namespace vocabulary

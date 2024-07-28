#include "vocabulary.h"

#include <fmt/format.h>

#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "spdlog/spdlog.h"

namespace vocabulary {

Vocabulary::Vocabulary(std::filesystem::path const& import_from, char item_delim,
                       char field_delim)
{
    importFromFile(import_from);
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
    std::ifstream inputFile(path /* , std::ios::app */);
    try {
        for (std::array<char, 1024> a; inputFile.getline(&a[0], a.size(), '\n');) {
            words_.push_back(Word::parse(/* std::string_view( */ a.data()));
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

void Vocabulary::exportToFile(std::filesystem::path const& path)
{
    std::ofstream outputFile(path /* , std::ios::app */);
    try {
        for (auto const& word : words_) {
            outputFile << word.toString() << '\n';
        }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

// // void addWordsFromFile(std::filesystem::path const& path);

// // import, export vocabulary
// void loadFromFile(std::filesystem::path const& path);
// void saveToFile(std::filesystem::path const& path);

Word& Vocabulary::nextWordToLearn()
{
    if (words_.empty()) {
        const auto msg{"nothing to learn"};
        spdlog::error("{}(): {}", __FUNCTION__, msg);
        throw VocabularyError(msg);
    }

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
    spdlog::error(msg);
    throw VocabularyError(msg);
}

}  // namespace vocabulary

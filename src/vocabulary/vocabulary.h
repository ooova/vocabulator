#ifndef VOCABULARY_VOCABULARY_H
#define VOCABULARY_VOCABULARY_H

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "vocabulary/word.h"

namespace vocabulary {

// using Translation = std::pair<std::vector<std::string> /*foreign*/,
//                               std::vector<std::string> /*examples*/>;

// using Word = std::pair<std::string, Translation>;

// class Word {
// private:
//     std::string word_{};
//     std::
// }

// std::string serialize(std::vector<std::string> const& values, char const delimiter);
// std::vector<std::string> deserialize(std::string_view str, char const delimiter);

// /**
//  * @throw std::logic_error if the input string format is wrong
//  * @throw std::out_of_range
//  */
// Word deserialize(std::string_view str);

// class Vocabulary {
// public:
//     Vocabulary() = default;

//     void addWord(std::string_view const word, Translation&& translation);
//     Translation translate(std::string_view const word) const;
//     Word getWord() const;
//     void save(std::filesystem::path const& path);
//     void load(std::filesystem::path const& path);

// private:
//     std::map<std::string /*native*/, Translation> vocabulary_{};
// };

class Vocabulary final {
public:
    // void setStrategy(std::weak_ptr<LearningStrategies::Strategy> strategy);
    Word& nextWord();
    void importFromFile(std::filesystem::path const& path);
    void exportToFile(std::filesystem::path const& path);
    void addWordsFromFile(std::filesystem::path const& path);
    // void addWord(std::string_view const word, Translation&& translation);
    void addWord(Word&& word);
    void loadFromFile(std::filesystem::path const& path);
    void saveToFile(std::filesystem::path const& path);

private:
    std::vector<Word> words_;
    // std::weak_ptr<LearningStrategies::Strategy> learning_strategy_;
};

}  // namespace vocabulary

#endif  // VOCABULARY_VOCABULARY_H

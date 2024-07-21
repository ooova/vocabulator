#include "vocabulary.h"

#include <fmt/format.h>

#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "spdlog/spdlog.h"

namespace {

void removePrefixSpaces(std::string_view& s)
{
    while (!s.empty() && (s.at(0) == ' ' || s.at(0) == '\t')) {
        s.remove_prefix(1);
    }
}

}  // namespace

namespace vocabulary {

// std::string serialize(std::vector<std::string> const& values, char const delimiter)
// {
//     if (values.empty()) {
//         return {};
//     }
//     return std::accumulate(
//         std::next(values.begin()), values.end(), values.front(),
//         [delimiter](auto const& a, auto const& b) { return a + delimiter + b; });
// }

std::vector<std::string> deserialize(std::string_view str, char const delimiter)
{
    auto result{std::vector<std::string>{}};

    if (str.empty()) {
        return result;
    }

    auto end{str.find(delimiter)};
    do {
        result.emplace_back(str.substr(0, end));
        str = str.substr(end);
        // begin = end;
        end = str.find(delimiter);
    } while (!str.empty());  // begin != std::string::npos);

    return result;
}

// Word deserialize(std::string_view str)
// {
//     spdlog::trace("{}(): str: {}", __FUNCTION__, str);
//     auto result{Word{}};

//     auto const removeDelimiterAndSpaces = [](auto& s) {
//         if (!s.empty() && (s.at(0) == kColumnDelimiter)) {
//             s.remove_prefix(1);
//         } else {
//             throw std::logic_error("wrong input string format");
//         }
//         removePrefixSpaces(s);
//     };

//     auto const findDelimiter = [](auto const str, auto const delimiter) {
//         // auto result{str.find(' ')};
//         // if (result == std::string::npos) {
//         //     result = str.find(kColumnDelimiter);
//         // }
//         // return result;
//         return str.find(delimiter);
//     };

//     removePrefixSpaces(str);

//     if (str.empty() || str.at(0) != kColumnDelimiter) {
//         throw std::logic_error("wrong input string format or the string is empty");
//     }

//     removeDelimiterAndSpaces(str);

//     auto wordEnd{findDelimiter(str, kColumnDelimiter)};
//     auto word{str.substr(0, wordEnd)};
//     if (wordEnd == std::string::npos) {
//         throw std::logic_error(
//             (std::string{
//                  "wrong input string format. Entry contains only \'word\' field: \'"} +
//              std::string{word} + "\'")
//                 .c_str());
//     }
//     spdlog::trace("{}: word found: {}", __PRETTY_FUNCTION__, word);
//     str = str.substr(wordEnd);

//     removeDelimiterAndSpaces(str);
//     auto translationEnd{findDelimiter(str, kColumnDelimiter)};
//     auto translation{str.substr(0, translationEnd)};
//     // if (translationEnd == std::string::npos) {
//     //     throw std::logic_error((std::string{"wrong input string format. Entry contains
//     //     only \'word\' field: \'"} + std::string{word} + "\'").c_str());
//     // }
//     spdlog::trace("{}: translation found: {}", __PRETTY_FUNCTION__, translation);

//     // result[]
//     // auto end{str.find(kItemsDelimiter)};

//     // do {
//     //     result.emplace_back(value.substr(0, end));
//     //     str = str.substr(end);
//     //     // begin = end;
//     //     end = str.find(kItemsDelimiter);
//     // } while (!str.empty());// begin != std::string::npos);

//     result.first = std::string{word};
//     result.second = {std::vector{std::string{translation}}, {}};

//     return result;
// }

void Vocabulary::addWord(std::string_view const word, Translation&& translation)
{
    spdlog::trace("{}(): word: {}, translation: {} | {}", __PRETTY_FUNCTION__, word,
                  serialize(translation.first, kItemsDelimiter),
                  serialize(translation.second, kItemsDelimiter));
    vocabulary_[std::string{word}] = translation;
}

Translation Vocabulary::translate(std::string_view const word) const
{
    if (vocabulary_.contains(std::string{word})) {
        return vocabulary_.at(std::string{word});
    }
    return {};
}

Word Vocabulary::getWord() const {
    if (vocabulary_.empty()) {
        spdlog::trace("{}: vocabulary is empty", __PRETTY_FUNCTION__);
        return {};
    }
    const auto word = vocabulary_.begin()->first;
    spdlog::trace("{}: word: {}", __PRETTY_FUNCTION__, word);
    return {word, vocabulary_.at(word)};
}

void Vocabulary::save(std::filesystem::path const path)
{
    std::ofstream outputFile(path /* , std::ios::app */);
    try {
        for (auto const& [word, translation] : vocabulary_) {
            // auto translation{std::string{}};
            auto const entry{fmt::format("|{}|{}|{}|\n", word,
                                         serialize(translation.first, kItemsDelimiter),
                                         serialize(translation.second, kItemsDelimiter))};
            outputFile << entry;
        }
        // std::vector<unsigned char> buf;
        // write some data into buf
        // outputFile.write(&buf[0], buf.size());  // write binary to the output stream
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

void Vocabulary::load(std::filesystem::path const path)
{
    std::ifstream inputFile(path /* , std::ios::app */);
    try {
        // std::array<char, 1024> ch_line;
        for (std::array<char, 1024> a; inputFile.getline(&a[0], a.size(), '\n');) {
            auto word{deserialize(std::string{a.data()})};
            vocabulary_[word.first] = word.second;
        }
        // while (!inputFile.eof()) {
        //     auto line{inputFile.getline(ch_line.data(), ch_line.size(), '\n')};
        //     auto word{deserialize(line)};
        //     vocabulary[word.first] = word.second;
        // }
    } catch (std::exception const& e) {
        spdlog::error("{}", e.what());
    }
}

}  // namespace vocabulary

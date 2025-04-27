#include "vocabulary/vocabulary.h"

#include <gtest/gtest.h>

#include "common/exceptions/vocabulary_error.h"
#include "spdlog/spdlog.h"

// #include <stdexcept>
// #include <string>

// #include "common/exceptions/parsing_error.h"

#include <filesystem>
#include <fstream>
#include <system_error>

using namespace vocabulary;

const auto kBasePath{std::filesystem::path{"/mnt/tmpfs/test"}};

class VocabularyFixture : public ::testing::Test {
protected:
    void SetUp() override
    {
        if (!std::filesystem::exists(kBasePath)) {
            std::error_code ec{};
            if (!std::filesystem::create_directories(kBasePath, ec)) {
                spdlog::error(ec.message());
            }
        }
        auto file{std::ofstream{kVocabularyPath}};
        if (file) {
            file << "|" + kTestWord + "|" + kTestTranslation + "; " + kTestTranslation2 +
                        "|" + kTestExample + "|"
                 << std::endl;
        } else {
            spdlog::error("vocabulary file not opened");
        }
    }
    void TearDown() override { std::filesystem::remove_all(kBasePath); }

protected:
    const std::string kTestWord{"word"};
    const std::string kTestTranslation{"слово"};
    const std::string kTestTranslation2{kTestTranslation + " 2"};
    const std::string kTestExample{"слово для тестУ"};
    const std::string kVocabularyFileName{"test.voc"};
    const std::filesystem::path kVocabularyPath{kBasePath / kVocabularyFileName};
};

TEST_F(VocabularyFixture, construct)
{
    auto obj{Vocabulary{}};
    EXPECT_THROW(obj.nextRandomWordToLearn(), VocabularyError);
    EXPECT_EQ(obj.getStatistic().words_count, 0);

    auto objImportFromFile{Vocabulary{kVocabularyPath}};
    ASSERT_EQ(objImportFromFile.getStatistic().words_count, 1);
    auto& wordToLearn{objImportFromFile.nextRandomWordToLearn()};
    EXPECT_EQ(wordToLearn.word(), kTestWord);
    ASSERT_EQ(wordToLearn.translation().variants().size(), 2);
    EXPECT_EQ(wordToLearn.translation().variants().at(0), kTestTranslation);
    EXPECT_EQ(wordToLearn.translation().variants().at(1), kTestTranslation2);
    ASSERT_EQ(wordToLearn.translation().examples().size(), 1);
    EXPECT_EQ(wordToLearn.translation().examples().at(0), kTestExample);
}

TEST_F(VocabularyFixture, addWord)
{
    auto obj{Vocabulary{}};
    obj.addWord(Word{kTestWord, {{kTestTranslation}}});
    ASSERT_EQ(obj.getStatistic().words_count, 1);
    auto& wordToLearn{obj.nextRandomWordToLearn()};
    EXPECT_EQ(wordToLearn.word(), kTestWord);
    EXPECT_EQ(wordToLearn.translation().variants().at(0), kTestTranslation);
    EXPECT_TRUE(wordToLearn.translation().examples().empty());

    auto obj1{Vocabulary{}};
    obj1.addWord(kTestWord, {{kTestTranslation}, {kTestExample}});
    ASSERT_EQ(obj1.getStatistic().words_count, 1);
    auto& wordToLearn1{obj1.nextRandomWordToLearn()};
    EXPECT_EQ(wordToLearn1.word(), kTestWord);
    EXPECT_EQ(wordToLearn1.translation().variants().at(0), kTestTranslation);
    EXPECT_EQ(wordToLearn1.translation().examples().at(0), kTestExample);
}

TEST_F(VocabularyFixture, translate) {
    auto obj{Vocabulary{}};
    obj.addWord(Word{kTestWord, {{kTestTranslation}}});

    EXPECT_THROW(obj.translate("gibberish"), VocabularyError);
    EXPECT_NO_THROW(obj.translate(kTestWord));
    auto const& word{obj.translate(kTestWord)};
    ASSERT_EQ(word.variants().size(), 1);
    EXPECT_EQ(word.variants().at(0), kTestTranslation);
    ASSERT_TRUE(word.examples().empty());
}

TEST_F(VocabularyFixture, export_and_import)
{
    const std::string kTestWord_2{"word_2"};
    const std::string kTestTranslation_2{"слово_2"};
    const std::string kTestTranslation2_2{kTestTranslation + " 2_2"};
    const std::string kTestExample_2{"слово для тестУ_2"};
    const std::string kCustomVocabularyFileName{"custom_test.voc"};
    const std::filesystem::path kCustomVocabularyPath{kBasePath /
                                                      kCustomVocabularyFileName};

    auto obj{Vocabulary{kVocabularyPath}};
    obj.addWord(kTestWord_2,
                {{kTestTranslation_2, kTestTranslation2_2}, {kTestExample_2}});
    spdlog::info("{}(): words before export:", __FUNCTION__);
    for (auto const& w : obj.words()) {
        spdlog::info("{}", w.toString());
    }
    obj.exportToFile(kCustomVocabularyPath);

    auto obj2{Vocabulary{}};
    obj2.importFromFile(kCustomVocabularyPath);
    spdlog::info("{}(): words after import:", __FUNCTION__);
    for (auto const& w : obj2.words()) {
        spdlog::info("{}", w.toString());
    }
    ASSERT_EQ(obj2.getStatistic().words_count, 2);
    auto const& words{obj2.words()};
    ASSERT_EQ(words.size(), 2);
    EXPECT_EQ(words.at(0).word(), kTestWord);
    ASSERT_EQ(words.at(0).translation().variants().size(), 2);
    EXPECT_EQ(words.at(0).translation().variants().at(0), kTestTranslation);
    EXPECT_EQ(words.at(0).translation().variants().at(1), kTestTranslation2);
    ASSERT_EQ(words.at(0).translation().examples().size(), 1);
    EXPECT_EQ(words.at(0).translation().examples().at(0), kTestExample);
    EXPECT_EQ(words.at(1).word(), kTestWord_2);
    ASSERT_EQ(words.at(1).translation().variants().size(), 2);
    EXPECT_EQ(words.at(1).translation().variants().at(0), kTestTranslation_2);
    EXPECT_EQ(words.at(1).translation().variants().at(1), kTestTranslation2_2);
    ASSERT_EQ(words.at(1).translation().examples().size(), 1);
    EXPECT_EQ(words.at(1).translation().examples().at(0), kTestExample_2);
}

TEST_F(VocabularyFixture, save_and_load) { ADD_FAILURE(); }

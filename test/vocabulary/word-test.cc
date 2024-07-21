#include "vocabulary/word.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "common/exceptions/parsing_error.h"

auto const kIDelim{vocabulary::Word::kDefaultItemsDelimiter};
auto const kFDelim{vocabulary::Word::kDefaultFieldsDelimiter};

auto const kCustomItemDelimiter{'/'};
auto const kCustomFieldDelimiter{'#'};

auto const kWordStr{std::string{"test_word"}};

auto const kVariant1{std::string{"variant1"}};
auto const kVariant2{std::string{"variant2"}};
auto const kVariants{std::vector{kVariant1, kVariant2}};

auto const kExample1{std::string{"example variant1"}};
auto const kExample2{std::string{"example variant2"}};
auto const kExamples{std::vector{kExample1, kExample2}};

auto const kVariantsExpectedResult{std::vector<std::string>{"variant1", "variant2"}};
auto const kExamplesExpectedResult{
    std::vector<std::string>{"example variant1", "example variant2"}};

using namespace vocabulary;

// ========================================================

class WordFixture : public ::testing::Test {
protected:
    std::vector<std::string> const variants1{{kVariant1}};
    std::string const word1AsString{{kFDelim + kWordStr + kFDelim + kVariant1 + kFDelim}};

    std::vector<std::string> const variants2{{kVariant1, kVariant2}};
    std::vector<std::string> const examples2{{kExample1, kExample2}};
    std::string const word2AsString{{kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim +
                                     kVariant2 + kFDelim + kExample1 + kIDelim +
                                     kExample2 + kFDelim}};
};

// Construct Word with correct parameters
TEST_F(WordFixture, wordConstructor)
{
    auto obj{Word{kWordStr, variants1}};
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());
    EXPECT_EQ(obj.toString(), word1AsString);
    EXPECT_EQ(obj.retentionPercentage(), 0);
    // EXPECT_EQ(obj.toBin(), );

    obj = Word{kWordStr, {variants2, examples2}};
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants2);
    EXPECT_EQ(obj.translation().examples(), examples2);
    EXPECT_EQ(obj.toString(), word2AsString);
    EXPECT_EQ(obj.retentionPercentage(), 0);
    // EXPECT_EQ(obj.toBin(), );
}

TEST_F(WordFixture, parsing)
{
    const auto examples1{std::vector{{kExample1}}};

    auto obj = Word::parse(kWordStr + kFDelim + kVariant1);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kFDelim + kWordStr + kFDelim + kVariant1 + kFDelim);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim + kFDelim);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim + kVariant2 +
                      kFDelim);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants2);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim + kVariant2 +
                      kIDelim + kFDelim + kExample1 + kFDelim);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants2);
    EXPECT_EQ(obj.translation().examples(), examples1);
    EXPECT_EQ(obj.toString(), kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim +
                                  kVariant2 + kFDelim + kExample1 + kFDelim);

    // custom delimeter

    obj = Word::parse(kWordStr + kCustomFieldDelimiter + kVariant1, kCustomItemDelimiter,
                      kCustomFieldDelimiter);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kCustomFieldDelimiter + kWordStr + kCustomFieldDelimiter +
                          kVariant1 + kCustomFieldDelimiter,
                      kCustomItemDelimiter, kCustomFieldDelimiter);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj = Word::parse(kCustomFieldDelimiter + kWordStr + kCustomFieldDelimiter +
                          kVariant1 + kCustomItemDelimiter + kCustomFieldDelimiter,
                      kCustomItemDelimiter, kCustomFieldDelimiter);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj =
        Word::parse(kCustomFieldDelimiter + kWordStr + kCustomFieldDelimiter + kVariant1 +
                        kCustomItemDelimiter + kVariant2 + kCustomFieldDelimiter,
                    kCustomItemDelimiter, kCustomFieldDelimiter);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants2);
    EXPECT_TRUE(obj.translation().examples().empty());

    obj =
        Word::parse(kCustomFieldDelimiter + kWordStr + kCustomFieldDelimiter + kVariant1 +
                        kCustomItemDelimiter + kVariant2 + kCustomItemDelimiter +
                        kCustomFieldDelimiter + kExample1 + kCustomFieldDelimiter,
                    kCustomItemDelimiter, kCustomFieldDelimiter);
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants2);
    EXPECT_EQ(obj.translation().examples(), examples1);
    EXPECT_EQ(obj.toString(), kCustomFieldDelimiter + kWordStr + kCustomFieldDelimiter +
                                  kVariant1 + kCustomItemDelimiter + kVariant2 +
                                  kCustomFieldDelimiter + kExample1 +
                                  kCustomFieldDelimiter);
}

TEST_F(WordFixture, addTranslation)
{
    auto obj{Word{kWordStr, variants1}};
    EXPECT_EQ(obj.word(), kWordStr);
    EXPECT_EQ(obj.translation().variants(), variants1);
    EXPECT_TRUE(obj.translation().examples().empty());
    EXPECT_EQ(obj.toString(), word1AsString);
    EXPECT_EQ(obj.retentionPercentage(), 0);
    // EXPECT_EQ(obj.toBin(), );

    obj.addTranslation({{kVariant2}, {kExample2}});
    EXPECT_EQ(obj.word(), kWordStr);
    auto const new_variants{std::vector{kVariant1, kVariant2}};
    EXPECT_EQ(obj.translation().variants(), new_variants);
    auto const new_examples{std::vector{kExample2}};
    EXPECT_EQ(obj.translation().examples(), new_examples);
    EXPECT_EQ(obj.toString(), kFDelim + kWordStr + kFDelim + kVariant1 + kIDelim +
                                  kVariant2 + kFDelim + kExample2 + kFDelim);
    EXPECT_EQ(obj.retentionPercentage(), 0);

    // EXPECT_EQ(obj.toBin(), );
}

TEST_F(WordFixture, wordConstructionWithEmptyWordThrow)
{
    EXPECT_THROW((Word{"", {variants1}}), std::invalid_argument);
}

TEST_F(WordFixture, throwWhenParseIncorrectString)
{
    EXPECT_THROW(Word::parse(""), ParsingError);
    EXPECT_THROW(Word::parse(kWordStr), ParsingError);
    EXPECT_THROW(Word::parse(kFDelim + kWordStr), ParsingError);
    EXPECT_THROW(Word::parse(kFDelim + kWordStr + kFDelim), ParsingError);
    EXPECT_THROW(Word::parse(kFDelim + kWordStr + kFDelim + kFDelim), ParsingError);
    EXPECT_THROW(Word::parse(kFDelim + kWordStr + kFDelim + kIDelim + kFDelim),
                 ParsingError);
    EXPECT_THROW(Word::parse(kFDelim + kWordStr + kIDelim + kWordStr + kFDelim +
                             kVariant1 + kFDelim),
                 ParsingError);
    EXPECT_THROW(
        Word::parse(std::string{kFDelim} + kIDelim + kFDelim + kVariant1 + kFDelim),
        ParsingError);
}

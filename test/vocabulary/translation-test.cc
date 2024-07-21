#include "vocabulary/translation.h"

#include <gtest/gtest.h>

#include <string>

#include "common/exceptions/parsing_error.h"

auto const kIDelim{vocabulary::Translation::kDefaultItemsDelimiter};
auto const kFDelim{vocabulary::Translation::kDefaultFieldsDelimiter};

auto const kCustomItemDelimiter{'/'};
auto const kCustomFieldDelimiter{'#'};

auto const kVariant1{std::string{"variant1"}};
auto const kVariant2{std::string{"variant2"}};
auto const kVariant3{std::string{"multi-word variant3"}};
auto const kVariants{std::vector{kVariant1, kVariant2, kVariant3}};

auto const kExample1{std::string{"example variant1"}};
auto const kExample2{std::string{"example variant2"}};
auto const kExample3{std::string{"example variant3"}};
auto const kExamples{std::vector{kExample1, kExample2, kExample3}};

auto const kTranslationString1{
    std::string{"|variant1; variant2; variant3 | example variant1; example variant2; "
                "example variant3; |"}};
auto const kTranslationString2{
    std::string{"   | \t variant1; variant2; variant3 | example variant1; example "
                "variant2; example variant3; \t |   "}};
auto const kTranslationString3{
    std::string{"variant1; variant2; variant3 | example variant1; example variant2; "
                "example variant3;"}};
auto const kTranslationString4{
    std::string{"variant1;     variant2; \t variant3| \texample variant1;example "
                "variant2;\t example variant3;"}};

auto const kVariantsExpectedResult{
    std::vector<std::string>{"variant1", "variant2", "variant3"}};
auto const kExamplesExpectedResult{
    std::vector<std::string>{"example variant1", "example variant2", "example variant3"}};

using namespace vocabulary;

TEST(TranslationTest, translationWithOneVariant)
{
    auto obj{Translation{{kVariant1}}};

    ASSERT_EQ(obj.variants().size(), 1);
    EXPECT_EQ(obj.variants().front(), kVariant1);
    EXPECT_EQ(obj.variantsToString(), kVariant1);

    EXPECT_TRUE(obj.examples().empty());
    EXPECT_TRUE(obj.examplesToString().empty());

    EXPECT_EQ(obj.toString(), kVariant1);
}

TEST(TranslationTest, translationWithSeveralVariants)
{
    auto const kVariantsString{kVariant1 + kIDelim + kVariant2 + kIDelim + kVariant3};

    auto obj{Translation{kVariants}};

    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_EQ(obj.variantsToString(), kVariantsString);

    EXPECT_TRUE(obj.examples().empty());
    EXPECT_TRUE(obj.examplesToString().empty());

    EXPECT_EQ(obj.toString(), kVariantsString);
}

TEST(TranslationTest, translationWithSeveralVariantsAndExamples)
{
    auto const kVariantsString{kVariant1 + kIDelim + kVariant2 + kIDelim + kVariant3};
    auto const kExamplesString{kExample1 + kIDelim + kExample2 + kIDelim + kExample3};
    auto const kExpectedFullString{kVariantsString + kFDelim + kExamplesString};

    std::cout << "kVariantsString = " << kVariantsString << std::endl;
    std::cout << "kExamplesString = " << kExamplesString << std::endl;
    std::cout << "kExpectedFullString = " << kExpectedFullString << std::endl;

    auto obj{Translation{kVariants, kExamples}};

    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_EQ(obj.variantsToString(), kVariantsString);

    EXPECT_EQ(obj.examples(), kExamples);
    EXPECT_EQ(obj.examplesToString(), kExamplesString);

    EXPECT_EQ(obj.toString(), kExpectedFullString);
}

TEST(TranslationTest, translationWithSeveralVariantsAndExamplesAndCustomDelimiters)
{
    auto const kVariantsString{kVariant1 + kCustomItemDelimiter + kVariant2 +
                               kCustomItemDelimiter + kVariant3};
    auto const kExamplesString{kExample1 + kCustomItemDelimiter + kExample2 +
                               kCustomItemDelimiter + kExample3};
    auto const kExpectedFullString{kVariantsString + kCustomFieldDelimiter +
                                   kExamplesString};

    std::cout << "kVariantsString = " << kVariantsString << std::endl;
    std::cout << "kExamplesString = " << kExamplesString << std::endl;
    std::cout << "kExpectedFullString = " << kExpectedFullString << std::endl;

    auto obj{
        Translation{kVariants, kExamples, kCustomItemDelimiter, kCustomFieldDelimiter}};

    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_EQ(obj.variantsToString(), kVariantsString);

    EXPECT_EQ(obj.examples(), kExamples);
    EXPECT_EQ(obj.examplesToString(), kExamplesString);

    EXPECT_EQ(obj.toString(), kExpectedFullString);
}

#include "spdlog/spdlog.h"
TEST(TranslationTest, translationParsing)
{
    spdlog::set_level(spdlog::level::trace);

    auto const kVariantsString{kVariant1 + kIDelim + kVariant2 + kIDelim + kVariant3};
    auto const kExamplesString{kExample1 + kIDelim + kExample2 + kIDelim + kExample3};
    auto const kExpectedFullString{kVariantsString + kFDelim + kExamplesString};

    auto obj{Translation::parse(kVariantsString)};
    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_TRUE(obj.examples().empty());

    obj = Translation::parse(kExpectedFullString);
    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_EQ(obj.examples(), kExamples);
}

TEST(TranslationTest, translationParsingWithCustomDelimiters)
{
    spdlog::set_level(spdlog::level::trace);

    auto const kVariantsString{kVariant1 + kCustomItemDelimiter + kVariant2 +
                               kCustomItemDelimiter + kVariant3};
    auto const kExamplesString{kExample1 + kCustomItemDelimiter + kExample2 +
                               kCustomItemDelimiter + kExample3};
    auto const kExpectedFullString{kVariantsString + kCustomFieldDelimiter +
                                   kExamplesString};

    auto obj{
        Translation::parse(kVariantsString, kCustomItemDelimiter, kCustomFieldDelimiter)};
    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_TRUE(obj.examples().empty());

    std::cout << std::endl;

    obj = Translation::parse(kExpectedFullString, kCustomItemDelimiter,
                             kCustomFieldDelimiter);
    EXPECT_EQ(obj.variants(), kVariants);
    EXPECT_EQ(obj.examples(), kExamples);
}

TEST(TranslationTest, translationParsingWithAdditionalSpaces)
{
    auto const kExpectedFullString{[] {
        auto result{std::string{}};
        for (auto const& v : kVariantsExpectedResult) {
            result += v + kIDelim;
        }
        result.back() = kFDelim;
        for (auto const& e : kExamplesExpectedResult) {
            result += e + kIDelim;
        }
        result.erase(result.size() - 1);
        return result;
    }()};

    auto obj{Translation::parse(kTranslationString1)};
    EXPECT_EQ(obj.variants(), kVariantsExpectedResult);
    EXPECT_EQ(obj.examples(), kExamplesExpectedResult);
    EXPECT_EQ(obj.toString(), kExpectedFullString);

    obj = Translation::parse(kTranslationString2);
    EXPECT_EQ(obj.variants(), kVariantsExpectedResult);
    EXPECT_EQ(obj.examples(), kExamplesExpectedResult);
    EXPECT_EQ(obj.toString(), kExpectedFullString);

    obj = Translation::parse(kTranslationString3);
    EXPECT_EQ(obj.variants(), kVariantsExpectedResult);
    EXPECT_EQ(obj.examples(), kExamplesExpectedResult);
    EXPECT_EQ(obj.toString(), kExpectedFullString);

    obj = Translation::parse(kTranslationString4);
    EXPECT_EQ(obj.variants(), kVariantsExpectedResult);
    EXPECT_EQ(obj.examples(), kExamplesExpectedResult);
    EXPECT_EQ(obj.toString(), kExpectedFullString);
}

TEST(TranslationTest, translationParsingThrowExceptionsWhenWordOrVariantNotFound)
{
    auto str{""};
    EXPECT_THROW(Translation::parse(str), ParsingError);

    str = "  | ";
    EXPECT_THROW(Translation::parse(str), ParsingError);

    str = "  \t ";
    EXPECT_THROW(Translation::parse(str), ParsingError);

    str = "   ";
    EXPECT_THROW(Translation::parse(str), ParsingError);

    str = "  | ; | ; |";
    EXPECT_THROW(Translation::parse(str), ParsingError);
}
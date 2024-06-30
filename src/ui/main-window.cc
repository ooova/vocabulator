#include "main-window.h"

#include "vocabulary/word.h"

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <span>

namespace {

constexpr auto kBackgroundColor{0x181818};

constexpr auto kScreenWidth{800};
constexpr auto kScreenHeight{450};
constexpr auto kScreenMargin{10};

constexpr auto kButtonWidth{100};
constexpr auto kButtonHeighth{30};

// namespace vocabulary {

// using Translation = std::pair<
//             std::vector<std::string>/*foreign*/, std::vector<std::string>/*examples*/
//         >
//     >;

// using Word = std::pair<std::string, Translation>;

auto word{vocabulary::Word{"слово", {{"word"}, {"log word"}}}};

} // namespace

namespace ui {

using namespace std::literals;
using namespace locale;

MainWindow::MainWindow()
    : RWindow(kScreenWidth, kScreenHeight, "Vocabulator")
    , button_previous_word_{{ kScreenMargin + (kScreenWidth - 2 * kScreenMargin) / 3 - kButtonWidth / 2,
            kScreenHeight - kScreenMargin - kButtonHeighth },
            { kButtonWidth,
            kButtonHeighth },
            Locale::translateInterface("previous"),
            []{spdlog::info("\'previous\' button clicked");},
            createFont()}
    , button_next_word_{{kScreenMargin + ((kScreenWidth - 2 * kScreenMargin) / 3) * 2 - kButtonWidth / 2,
            kScreenHeight - kScreenMargin - kButtonHeighth},
            {kButtonWidth,
            kButtonHeighth},
            Locale::translateInterface("next"),
            nullptr,
            createFont()}
    , button_load_vocabulary_{{kScreenMargin, kScreenMargin},
                   {kButtonWidth, kButtonHeighth},
                   Locale::translateInterface("load vocabulary"),
                   nullptr,
                   createFont()}
    , card_{{kScreenMargin, kScreenMargin * 2 + kButtonWidth},
            {kScreenWidth - kScreenMargin * 2, kScreenHeight - kScreenMargin * 2 - kButtonHeighth},
            word,
            createFont()}
{}

void MainWindow::draw() {

    button_previous_word_.draw();
    button_next_word_.draw();
    button_load_vocabulary_.draw();
    card_.draw();

    RWindow::DrawFPS(10, 10);
}

// widgets::TextBox& MainWindow::textBox() {
//     return  text_box_;
// }

RFont MainWindow::createFont() {
    const int kFontSize_{36};
    auto codepointsCount{0};
    auto codepoints = ::LoadCodepoints(Locale::getAlphabet(char_set_).data(), &codepointsCount);
    auto font{RFont(std::string(font_file_path_), kFontSize_, codepoints, codepointsCount)};
    ::UnloadCodepoints(codepoints);
    return font;
}

} // namespace ui

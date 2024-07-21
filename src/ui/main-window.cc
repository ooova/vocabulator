#include "main-window.h"

#include <array>
#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "vocabulary/vocabulary.h"

namespace {

constexpr auto kBackgroundColor{0x181818};

constexpr auto kScreenWidth{800};
constexpr auto kScreenHeight{450};
constexpr auto kScreenMargin{10};

constexpr auto kButtonWidth{100};
constexpr auto kButtonHeighth{30};

auto word{vocabulary::Word{"слово", {{"word"}, {"log word"}}}};

}  // namespace

namespace ui {

using namespace std::literals;
using namespace locale;

MainWindow::MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary)
    : RWindow(kScreenWidth, kScreenHeight, "Vocabulator")
    , vocabulary_{vocabulary}
    // , button_previous_word_{{kScreenMargin + (kScreenWidth - 2 * kScreenMargin) / 3 -
    //                              kButtonWidth / 2,
    //                          kScreenHeight - kScreenMargin - kButtonHeighth},
    //                         {kButtonWidth, kButtonHeighth},
    //                         Locale::translateInterface("previous"),
    //                         [] { spdlog::info("\'previous\' button clicked"); },
    //                         createFont()}
    , button_next_word_{{kScreenMargin + ((kScreenWidth - 2 * kScreenMargin) / 3) * 2 -
                             kButtonWidth / 2,
                         kScreenHeight - kScreenMargin - kButtonHeighth},
                        {kButtonWidth, kButtonHeighth},
                        Locale::translateInterface("next"),
                        [this] {
                            if (auto v = vocabulary_.lock()) {
                                if (auto word{v->getWord()}. )
                                card_.setWord(word);
                            }
                        },
                        createFont()}
    , button_load_vocabulary_{{kScreenMargin, kScreenMargin},
                              {kButtonWidth, kButtonHeighth},
                              Locale::translateInterface("load vocabulary"),
                              [this] {
                                  if (auto v = vocabulary_.lock()) {
                                      v->load("test.voc");
                                  } else {
                                      spdlog::critical(
                                          "vocabulary is not available (destroyed)");
                                  }
                              },
                              createFont()}
    , button_save_vocabulary_{{kScreenMargin * 2 + kButtonWidth, kScreenMargin},
                              {kButtonWidth, kButtonHeighth},
                              Locale::translateInterface("save vocabulary"),
                              [this] {
                                  if (auto v = vocabulary_.lock()) {
                                      v->save("test.voc");
                                  } else {
                                      spdlog::critical(
                                          "vocabulary is not available (destroyed)");
                                  }
                              },
                              createFont()}
    , button_vocabulary_add_word_{{kScreenMargin * 3 + kButtonWidth * 2, kScreenMargin},
                                  {kButtonWidth, kButtonHeighth},
                                  Locale::translateInterface("add word"),
                                  [this] {
                                      if (auto v = vocabulary_.lock()) {
                                          v->addWord("test", {{"тест", "проверка"},
                                                              {"тест пример 1",
                                                               "тест пример 2"}});
                                      } else {
                                          spdlog::critical(
                                              "vocabulary is not available (destroyed)");
                                      }
                                  },
                                  createFont()}
    , card_{{kScreenMargin, kScreenMargin * 2 + kButtonWidth},
            {kScreenWidth - kScreenMargin * 2,
             kScreenHeight - kScreenMargin * 2 - kButtonHeighth},
            word,
            createFont()}
{}

void MainWindow::draw()
{
    // button_previous_word_.draw();
    button_next_word_.draw();
    button_load_vocabulary_.draw();
    button_save_vocabulary_.draw();
    button_vocabulary_add_word_.draw();
    card_.draw();

    RWindow::DrawFPS(10, 10);
}

// widgets::TextBox& MainWindow::textBox() {
//     return  text_box_;
// }

RFont MainWindow::createFont()
{
    const int kFontSize_{36};
    auto codepointsCount{0};
    auto codepoints =
        ::LoadCodepoints(Locale::getAlphabet(char_set_).data(), &codepointsCount);
    auto font{
        RFont(std::string(font_file_path_), kFontSize_, codepoints, codepointsCount)};
    ::UnloadCodepoints(codepoints);
    return font;
}

}  // namespace ui

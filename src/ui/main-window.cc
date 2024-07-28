#include "main-window.h"

#include <array>
#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "ui/tools/text.h"
#include "vocabulary/vocabulary.h"

namespace {

auto word{vocabulary::Word{"слово", {{"word"}, {"log word"}}}};

}  // namespace

namespace ui {

using namespace std::literals;
using tools::Locale;

MainWindow::MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary)
    : RWindow(kScreenWidth, kScreenHeight, "Vocabulator")
    , vocabulary_{vocabulary}
    // , button_previous_word_{{kScreenMargin + (kScreenWidth - 2 * kScreenMargin) / 3 -
    //                              kButtonWidth / 2,
    //                          kScreenHeight - kScreenMargin - kButtonHeighth},
    //                         {kButtonWidth, kButtonHeighth},
    //                         Locale::translateInterface("previous"),
    //                         [] { spdlog::info("\'previous\' button clicked"); },
    //                         tools::createFont(font_file_path_,
    //                         Locale::getAlphabet(char_set_))}
    , button_next_word_{{kScreenMargin + ((kScreenWidth - 2 * kScreenMargin) / 3) * 2 -
                             kButtonWidth / 2,
                         kScreenHeight - kScreenMargin - kButtonHeighth},
                        {kButtonWidth, kButtonHeighth},
                        Locale::translateInterface("next"),
                        [this] {
                            // if (auto v = vocabulary_.lock()) {
                            //     if (auto word{v->getWord()}. )
                            //     card_.setWord(word);
                            // }
                        },
                        tools::createFont(font_file_path_,
                                          Locale::getAlphabet(char_set_))}
    , button_load_vocabulary_{{kScreenMargin, kScreenMargin},
                              {kButtonWidth, kButtonHeighth},
                              Locale::translateInterface("load vocabulary"),
                              [this] {
                                  //   if (auto v = vocabulary_.lock()) {
                                  //       v->load("test.voc");
                                  //   } else {
                                  //       spdlog::critical(
                                  //           "vocabulary is not available (destroyed)");
                                  //   }
                              },
                              tools::createFont(font_file_path_,
                                                Locale::getAlphabet(char_set_))}
    , button_save_vocabulary_{{kScreenMargin * 2 + kButtonWidth, kScreenMargin},
                              {kButtonWidth, kButtonHeighth},
                              Locale::translateInterface("save vocabulary"),
                              [this] {
                                  //   if (auto v = vocabulary_.lock()) {
                                  //       v->save("test.voc");
                                  //   } else {
                                  //       spdlog::critical(
                                  //           "vocabulary is not available (destroyed)");
                                  //   }
                              },
                              tools::createFont(font_file_path_,
                                                Locale::getAlphabet(char_set_))}
    , button_vocabulary_add_word_{{kScreenMargin * 3 + kButtonWidth * 2, kScreenMargin},
                                  {kButtonWidth, kButtonHeighth},
                                  Locale::translateInterface("add word"),
                                  [this] {
                                      //   if (auto v = vocabulary_.lock()) {
                                      //       v->addWord("test", {{"тест", "проверка"},
                                      //                           {"тест пример 1",
                                      //                            "тест пример 2"}});
                                      //   } else {
                                      //       spdlog::critical(
                                      //           "vocabulary is not available
                                      //           (destroyed)");
                                      //   }
                                  },
                                  tools::createFont(font_file_path_,
                                                    Locale::getAlphabet(char_set_))}
    , card_{{kScreenMargin, kScreenMargin * 2 + kButtonWidth},
            {kScreenWidth - kScreenMargin * 2,
             kScreenHeight - kScreenMargin * 2 - kButtonHeighth},
            vocabulary_.lock()->nextWordToLearn(),
            tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
// , add_new_word_{{kAddNewWordBoxPosition, kAddNewWordBoxSize,
// tools::createFont(font_file_path_, Locale::getAlphabet(char_set_)), true}}
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

}  // namespace ui

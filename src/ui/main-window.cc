#include "main-window.h"

#include <array>
#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "ui/tools/text.h"
#include "vocabulary/vocabulary.h"
#include "vocabulary/translation.h"
#include "spdlog/spdlog.h"

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
    //                          kScreenHeight - kScreenMargin - kButtonHeight},
    //                         {kButtonWidth, kButtonHeight},
    //                         Locale::translateInterface("previous"),
    //                         [] { spdlog::info("\'previous\' button clicked"); },
    //                         tools::createFont(font_file_path_,
    //                         Locale::getAlphabet(char_set_))}
    , button_next_word_{
        // {kScreenMargin + ((kScreenWidth - 2 * kScreenMargin) / 3) * 2 -
        //                      kButtonWidth / 2,
        //                  kScreenHeight - kScreenMargin - kButtonHeight},
                        {650, kScreenMargin},
                        {kButtonWidth, kButtonHeight},
                        Locale::translateInterface("next"),
                        [this] {
                            if (auto v = vocabulary_.lock()) {
                                try {
                                    // if (auto word{} )
                                    card_.setWord(v->nextWordToLearn());
                                } catch (VocabularyError const& ex) {
                                    spdlog::error(ex.what());
                                }
                            }
                        },
                        tools::createFont(font_file_path_,
                                          Locale::getAlphabet(char_set_))}
    , button_load_vocabulary_{{kScreenMargin, kScreenMargin},
                              {kButtonWidth, kButtonHeight},
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
                              {kButtonWidth, kButtonHeight},
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
                                  {kButtonWidth, kButtonHeight},
                                  Locale::translateInterface("add word"),
                                  [this] {
                                      if (auto voc = vocabulary_.lock(); voc) {
                                        voc->addWord(new_word_input_.text(), vocabulary::Translation::parse(new_word_translation_input_.text()));
                                      }
                                  },
                                  tools::createFont(font_file_path_,
                                                    Locale::getAlphabet(char_set_))}
    , card_{{kScreenMargin, kScreenMargin * 2 + kButtonWidth},
            {kScreenWidth - kScreenMargin * 2,
             kScreenHeight - kScreenMargin * 2 - kButtonHeight},
            vocabulary_.lock()->nextWordToLearn(),
            tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
    , new_word_input_{
        {kScreenMargin + kElementMargin, kScreenHeight - kScreenMargin - (kElementMargin + kButtonHeight) * 3},
        {kButtonWidth, kButtonHeight},
        // fix problem with cyrillic symbols
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))/* {} */}
    , new_word_translation_input_{
        {new_word_input_.GetX(), new_word_input_.GetY() + new_word_input_.GetHeight() + kElementMargin},
        {new_word_input_.GetWidth(), new_word_input_.GetHeight()},
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
    , new_word_example_input_{
        {new_word_translation_input_.GetX(), new_word_translation_input_.GetY() + new_word_translation_input_.GetHeight() + kElementMargin},
        {new_word_translation_input_.GetWidth(), new_word_translation_input_.GetHeight()},
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
{
    // print screen parameters
    spdlog::info("Screen width: {}", kScreenWidth);
    spdlog::info("Screen height: {}", kScreenHeight);
    spdlog::info("Screen margin: {}", kScreenMargin);
    spdlog::info("Element margin: {}", kElementMargin);
    spdlog::info("Button height: {}", kButtonHeight);
    spdlog::info("Card height: {}", kCardHeight);

    spdlog::info("new_word_input_: x({}) y({}) w({}) h({})", new_word_input_.GetX(), new_word_input_.GetY(), new_word_input_.GetWidth(), new_word_input_.GetHeight());
    spdlog::info("new_word_translation_input_: x({}) y({}) w({}) h({})", new_word_translation_input_.GetX(), new_word_translation_input_.GetY(), new_word_translation_input_.GetWidth(), new_word_translation_input_.GetHeight());
    spdlog::info("new_word_example_input_: x({}) y({}) w({}) h({})", new_word_example_input_.GetX(), new_word_example_input_.GetY(), new_word_example_input_.GetWidth(), new_word_example_input_.GetHeight());
}

void MainWindow::draw()
{
    // button_previous_word_.draw();
    button_next_word_.draw();
    button_load_vocabulary_.draw();
    button_save_vocabulary_.draw();
    button_vocabulary_add_word_.draw();
    card_.draw();
    new_word_input_.draw();
    new_word_translation_input_.draw();
    new_word_example_input_.draw();

    // optionally we can draw current FPS
    RWindow::DrawFPS(10, 10);
}

void MainWindow::update(float dt) {
    button_next_word_.update(dt);
    button_load_vocabulary_.update(dt);
    button_save_vocabulary_.update(dt);
    button_vocabulary_add_word_.update(dt);
    // card_.update(dt);
    new_word_input_.update(dt);
    new_word_translation_input_.update(dt);
    new_word_example_input_.update(dt);
}

// widgets::TextBox& MainWindow::textBox() {
//     return  text_box_;
// }

}  // namespace ui

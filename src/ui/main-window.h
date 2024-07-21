#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <array>
#include <memory>
#include <string>

#include "./tools/locale.h"
#include "./widgets/button.h"
#include "raylib-cpp.hpp"
#include "ui/widgets/card.h"

namespace vocabulary {
class Vocabulary;
}

namespace ui {

class MainWindow : public RWindow {
public:
    MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary);

    void draw();

private:
    const std::string font_file_path_{"./assets/fonts/Ubuntu-R.ttf"};
    std::array<locale::Language, 4> char_set_{
        locale::Language::kRU, locale::Language::kEN, locale::Language::kMathSymbols, locale::Language::kSpecSymbols};

    std::weak_ptr<vocabulary::Vocabulary> vocabulary_;

    // widgets::Button button_previous_word_;
    widgets::Button button_next_word_;
    widgets::Button button_load_vocabulary_;
    widgets::Button button_save_vocabulary_;
    widgets::Button button_vocabulary_add_word_;
    widgets::Card card_;

    RFont createFont();
};

}  // namespace ui

#endif  // UI_MAIN_WINDOW_H

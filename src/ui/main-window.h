#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

// #include "./tools/custom-font.h"
#include "./tools/locale.h"
#include "./widgets/button.h"
// #include "./widgets/text-box.h"
#include "ui/widgets/card.h"

#include "raylib-cpp.hpp"

#include <array>
#include <memory>
#include <string>

namespace ui {

class MainWindow : public RWindow {
public:
    MainWindow();

    void draw();

    // widgets::TextBox& textBox();
private:
    const std::string font_file_path_{"./assets/fonts/Ubuntu-R.ttf"};
    std::array<locale::Language, 3> char_set_{locale::Language::kRU, locale::Language::kEN, locale::Language::kMathSymbols};

    widgets::Button button_previous_word_;
    widgets::Button button_next_word_;
    widgets::Button button_load_vocabulary_;
    widgets::Card card_;

    RFont createFont();
};

} // namespace ui

#endif // UI_MAIN_WINDOW_H

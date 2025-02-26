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

constexpr int kBackgroundColor{0x181818};

constexpr int kScreenWidth{800};
constexpr int kScreenHeight{450};
constexpr int kScreenMargin{10};

constexpr int kButtonWidth{100};
constexpr int kButtonHeighth{30};

constexpr int kElementMargin{5};

constexpr int kCardHeight{200};

class MainWindow : public RWindow {
public:
    MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary);

    void draw();

private:
    // const RVector2 kAddNewWordBoxPosition{
    //     kScreenMargin, kScreenMargin + kElementMargin + kButtonHeighth + kElementMargin
    //     +
    //                        kCardHeight + kElementMargin};
    // const RVector2 kAddNewWordBoxSize{kScreenWidth + 2 * kScreenMargin, kCardHeight};

    const std::string font_file_path_{"../assets/fonts/Ubuntu-R.ttf"};
    std::array<tools::Language, 4> char_set_{
        tools::Language::kRU, tools::Language::kEN, tools::Language::kMathSymbols,
        tools::Language::kSpecSymbols};

    std::weak_ptr<vocabulary::Vocabulary> vocabulary_;

    // widgets::Button button_previous_word_;
    widgets::Button button_next_word_;
    widgets::Button button_load_vocabulary_;
    widgets::Button button_save_vocabulary_;
    widgets::Button button_vocabulary_add_word_;
    widgets::Card card_;
};

}  // namespace ui

#endif  // UI_MAIN_WINDOW_H

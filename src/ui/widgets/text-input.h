#ifndef UI_WIDGETS_TEXT_INPUT_H
#define UI_WIDGETS_TEXT_INPUT_H

#include "text-box.h"

#include "raylib-cpp.hpp"
#include "spdlog/spdlog.h" // for debug

namespace ui::widgets {

class TextInput : public TextBox {
public:
    enum class KeyCodes {
        kUp = 265,
        kRight = 262,
        kDown = 264,
        kLeft = 263,
        kShift = 344,
        kBackspace = 259,
        kDel = 261,
        // kEsc = 256,
        kEnter = 257,
    };

public:
    TextInput(RVector2 position, RVector2 size, RFont&& font,
            RColor textColor = RColor::Black(), RColor backgroundColor = RColor::White()/* RColor(0x00) */);

    bool isInFocus() { return in_focus_; }

    void update(float dt);
    void draw();

    std::string text(bool with_new_lines = false) const;

private:
    RVector2 cursorPosition_;
    RColor textColor_;

    bool cursor_visible_{true};
    float cursor_timer_{0.0f};
    const float cursor_blink_interval_{0.5f};
    bool in_focus_{false};

    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorDown();
    void moveCursorUp();
    void normalizeCursor();
    void insertCharacter(char32_t ch);
    void deleteCharacter(bool backspace);
    void deleteParagraph(bool backspace);

    void handleInput();
};

} // namespace ui::widgets

#endif // UI_WIDGETS_TEXT_INPUT_H

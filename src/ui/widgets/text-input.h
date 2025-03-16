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
    };

public:
    TextInput(RVector2 position, RVector2 size, RFont&& font,
            RColor textColor = RColor::Black(), RColor backgroundColor = RColor::White()/* RColor(0x00) */)
        : TextBox(position, size, std::move(font), textColor, backgroundColor)
        , cursorPosition_(0) {
        TextBox::setText("Hello world\nHello from Mars\nI'm here");
        spdlog::trace("lagger \'0\': {}", text_.at(0).first.font.glyphs[0].value);
    }

    void update(float dt);
    void draw();

private:
    RVector2 cursorPosition_;

    bool cursorVisible_ = true;
    float cursorTimer_ = 0.0f;
    const float cursorBlinkInterval_ = 0.5f;

    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorDown();
    void moveCursorUp();
    void normalizeCursor();
    void insertCharacter(char ch);
    void deleteCharacter(bool backspace);
    void deleteParagraph(bool backspace);
};

} // namespace ui::widgets

#endif // UI_WIDGETS_TEXT_INPUT_H

#ifndef UI_WIDGETS_TEXT_INPUT_H
#define UI_WIDGETS_TEXT_INPUT_H

#include "ui/widgets/widget.h"

#include "raylib-cpp.hpp"

#include <string>

namespace ui {

namespace events {
    struct KeyboardEvent;
    struct MouseEvent;
} // namespace events

namespace widgets {

class TextInput : public Widget {
public:
    TextInput(RVector2 position, RVector2 size, RFont&& font,
              RColor text_color = RColor::Black(), RColor background_color = RColor::White(),
              RColor cursor_color = RColor::Black());

    void draw() const override;

    void update(float dt) override;

    std::string getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; cursor_pos_ = text.size(); }

    void setIfFocus() { in_focus_ = true; }
    [[nodiscard]]bool isInFocus() const { return in_focus_; }

private:
    RFont font_;
    RColor text_color_;
    RColor background_color_;
    RColor cursor_color_;
    bool in_focus_{false};
    std::string text_{};
    size_t cursor_pos_ = 0;
    bool cursor_visible_ = true;
    float cursor_timer_ = 0.0f;
    int font_size_ = 16.0f;
    float spacing_ = 1.0f;
    float text_offset_x_;

    size_t prev_char_pos(size_t pos) const;
    size_t next_char_pos(size_t pos) const;

    void handleKeyboardEvent(ui::events::KeyboardEvent const& event);
    void handleMouseEvent(ui::events::MouseEvent const& event);
};

}  // namespace widgets

}  // namespace ui

#endif  // UI_WIDGETS_TEXT_INPUT_H
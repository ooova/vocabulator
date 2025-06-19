#include "text-input.h"

#include "common/events/event_dispatcher.h"
#include "tools/string_utils.h"
#include "ui/events/mouse_events.h"
#include "ui/events/keyboard_event.h"
#include "ui/events/text_input_event.h"

namespace ui::widgets {

TextInput::TextInput(RVector2 position, RVector2 size, RFont&& font,
                     RColor text_color, RColor background_color, RColor cursor_color)
    : Widget(position, size)
    , font_{std::move(font)}
    , text_color_{text_color}
    , background_color_{background_color}
    , cursor_color_{cursor_color}
    , font_size_{font_.GetBaseSize()}
    , text_offset_x_{position.GetX()}
{
    auto& event_dispatcher = common::EventDispatcher::instance();
    event_dispatcher.subscribe<events::KeyboardEvent>(
        [this](events::KeyboardEvent const& event) {
            this->handleKeyboardEvent(event);
        });
    event_dispatcher.subscribe<events::MouseEvent>(
        [this](events::MouseEvent const& event) {
            this->handleMouseEvent(event);
        });
}

void TextInput::update(float dt) {
    cursor_timer_ += dt;
    if (cursor_timer_ >= 0.5f) {
        cursor_visible_ = !cursor_visible_;
        cursor_timer_ = 0.0f;
    }
}

void TextInput::draw() const 
{
    const auto text_size = font_.MeasureText(text_.c_str(), font_size_, spacing_);
    const auto text_y_pos = GetY() + (GetHeight() - text_size.GetY()) / 2;
    Draw(background_color_);

    ::BeginScissorMode(GetX(), GetY(), GetWidth(), GetHeight());
    font_.DrawText(text_.c_str(), {text_offset_x_, text_y_pos}, font_size_, spacing_, text_color_);
    ::EndScissorMode();

    if (in_focus_ && cursor_visible_) {
        auto text_size = font_.MeasureText(text_.substr(0, cursor_pos_).c_str(), font_size_, spacing_);
        auto cursor_screen_x = text_offset_x_ + text_size.x;
        ::DrawLine(cursor_screen_x, text_y_pos, cursor_screen_x, text_y_pos + font_size_, cursor_color_);
    }
}

size_t TextInput::prev_char_pos(size_t pos) const {
    if (pos == 0) {
        return 0;
    }
    pos--;
    while (pos > 0 && (text_[pos] & 0xC0) == 0x80) {
        pos--;
    }
    return pos;
}

size_t TextInput::next_char_pos(size_t pos) const {
    if (pos >= text_.size()) {
        return text_.size();
    }
    pos++;
    while (pos < text_.size() && (text_[pos] & 0xC0) == 0x80) {
        pos++;
    }
    return pos;
}

// private ------------------------------------------------------------

void TextInput::handleKeyboardEvent(events::KeyboardEvent const& event)
{
    if (in_focus_) {
        
        for (auto const& codepoint : event.codepoints) {
            std::string char_str = tools::string_utils::codepoint_to_utf8(codepoint);
            text_.insert(cursor_pos_, char_str);
            cursor_pos_ += char_str.size();
        }

        switch (event.key) {
            case KEY_BACKSPACE:
                if (cursor_pos_ > 0) {
                    size_t prev_pos = prev_char_pos(cursor_pos_);
                    text_.erase(prev_pos, cursor_pos_ - prev_pos);
                    cursor_pos_ = prev_pos;
                }
                break;
            case KEY_DELETE:
                if (cursor_pos_ < text_.size()) {
                    size_t next_pos = next_char_pos(cursor_pos_);
                    text_.erase(cursor_pos_, next_pos - cursor_pos_);
                }
                break;
            case KEY_LEFT:
                if (cursor_pos_ > 0) {
                    cursor_pos_ = prev_char_pos(cursor_pos_);
                }
                break;
            case KEY_RIGHT:
                if (cursor_pos_ < text_.size()) {
                    cursor_pos_ = next_char_pos(cursor_pos_);
                }
                break;
            case KEY_HOME:
                cursor_pos_ = 0;
                break;
            case KEY_END:
                cursor_pos_ = text_.size();
                break;
            case KEY_ENTER:
                common::EventDispatcher::instance().dispatch(events::TextInputEvent{.text = text_});
                break;
        }
    }

    auto text_size = font_.MeasureText(text_.substr(0, cursor_pos_).c_str(), font_size_, spacing_);

    auto const spacing = spacing_ * 2;
    text_offset_x_ = GetX() + spacing;
    if (GetWidth() <= text_size.x) {
        text_offset_x_ = GetWidth() - text_size.x - spacing + GetX();
    }
}

void TextInput::handleMouseEvent(events::MouseEvent const& event)
{
    if (event.button == events::MouseEvent::Button::kLeft) {
        in_focus_ = CheckCollision(RVector2{event.x, event.y});
    }
}

}  // namespace ui::widgets
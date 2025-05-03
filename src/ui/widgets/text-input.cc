#include "text-input.h"

namespace ui::widgets {

TextInput::TextInput(RVector2 position, RVector2 size, RFont&& font,
                     RColor text_color, RColor background_color, RColor cursor_color)
    : RRectangle(position.GetX(), position.GetY(), size.GetX(), size.GetY())
    , text_offset_x_(position.GetX())
    , font_(std::move(font))
    , text_color_(text_color)
    , background_color_(background_color)
    , cursor_color_(cursor_color)
{
}

void TextInput::update(float dt) {
    if (::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT)) {
        in_focus_ = CheckCollision(::GetMousePosition());
    }

    if (in_focus_) {

        int codepoint;
        while ((codepoint = GetCharPressed()) != 0) {
            std::string char_str = codepoint_to_utf8(codepoint);
            text_.insert(cursor_pos_, char_str);
            cursor_pos_ += char_str.size();
        }

        const auto key_pressed = GetKeyPressed();
        switch (key_pressed) {
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
        }
    }

    auto text_size = font_.MeasureText(text_.substr(0, cursor_pos_).c_str(), font_size_, spacing_);

    auto const spacing = spacing_ * 2;
    text_offset_x_ = GetX() + spacing;
    if (GetWidth() <= text_size.x) {
        text_offset_x_ = GetWidth() - text_size.x - spacing + GetX();
    }

    cursor_timer_ += dt;
    if (cursor_timer_ >= 0.5f) {
        cursor_visible_ = !cursor_visible_;
        cursor_timer_ = 0.0f;
    }
}

void TextInput::draw() {
    const auto text_top_margin = spacing_ * 3;
    Draw(background_color_);

    ::BeginScissorMode(GetX(), GetY(), GetWidth(), GetHeight());
    font_.DrawText(text_.c_str(), {text_offset_x_, GetY() + text_top_margin}, font_size_, spacing_, text_color_);
    ::EndScissorMode();

    if (in_focus_ && cursor_visible_) {
        auto text_size = font_.MeasureText(text_.substr(0, cursor_pos_).c_str(), font_size_, spacing_);
        auto cursor_screen_x = text_offset_x_ + text_size.x;
        ::DrawLine(cursor_screen_x, GetY() + text_top_margin, cursor_screen_x, GetY() + font_size_ + text_top_margin, cursor_color_);
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

std::string TextInput::codepoint_to_utf8(int codepoint) const {
    std::string result;
    if (codepoint <= 0x7F) {
        result += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    return result;
}

}  // namespace ui::widgets
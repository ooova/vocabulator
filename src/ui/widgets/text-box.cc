#include "text-box.h"

#include "spdlog/spdlog.h"

namespace ui::widgets {

TextBox::TextBox(RVector2 position, RVector2 size, RFont&& font,
                 RColor textColor, RColor backgroundColor)
    : Widget(position, size)
    , font_{std::move(font)}
    , textColor_{textColor}
    , backgroundColor_{backgroundColor}
{
    clear();
}

TextBox& TextBox::operator<<(std::string_view text)
{
    if (!text.empty()) {
        if (text_.empty()) {
            addParagraph();
        }

        const auto* it = text.data();
        const auto* end = text.data() + text.size();
        
        while (it != end) {
            if (*it == '\n') {
                addParagraph();
                ++it;
                continue;
            }
            // check if the current paragraph overflows the text box and add a new paragraph if it does
            if (text_.back().first.MeasureEx().GetX() > (RRectangle::GetWidth() - font_.GetBaseSize())) {
                addParagraph();
            }
            auto& current_paragraph = text_.back().first.text;
            // Get the next UTF-8 character
            auto c = static_cast<unsigned char>(*it);
            if (c < 0x80) {
                // ASCII character
                current_paragraph.push_back(c);
                ++it;
            } else if (c < 0xE0) {
                // 2-byte UTF-8
                if (end - it < 2) break;
                current_paragraph.push_back(*it);
                current_paragraph.push_back(*(it + 1));
                it += 2;
            } else if (c < 0xF0) {
                // 3-byte UTF-8
                if (end - it < 3) break;
                current_paragraph.push_back(*it);
                current_paragraph.push_back(*(it + 1));
                current_paragraph.push_back(*(it + 2));
                it += 3;
            } else if (c < 0xF8) {
                // 4-byte UTF-8
                if (end - it < 4) break;
                current_paragraph.push_back(*it);
                current_paragraph.push_back(*(it + 1));
                current_paragraph.push_back(*(it + 2));
                current_paragraph.push_back(*(it + 3));
                it += 4;
            } else {
                // Invalid UTF-8 sequence
                ++it;
            }
        }
    }

    return *this;
}

void TextBox::setText(std::string_view const& text)
{
    initText();
    operator<<(text);
}

void TextBox::setAlignment(Alignment alignment) { text_.back().second = alignment; }

void TextBox::draw() const
{
    RRectangle::Draw(backgroundColor_);
    RVector2 position = RRectangle::GetPosition();

    for (const auto& paragraph : text_) {
        auto paragraph_size{paragraph.first.MeasureEx()};
        switch (paragraph.second) {
            case Alignment::kCenter: {
                position.SetX((RRectangle::GetPosition().GetX() +
                               RRectangle::GetSize().GetX() / 2) -
                              (paragraph_size.GetX() / 2));
            } break;
            case Alignment::kRight: {
                position.SetX(
                    (RRectangle::GetPosition().GetX() + RRectangle::GetSize().GetX()) -
                    paragraph_size.GetX());
            } break;
            default:
                position.SetX(RRectangle::GetPosition().GetX());
                break;
        }
        paragraph.first.Draw(position);
        position.SetY(position.GetY() + paragraph_size.GetY());
        const auto text_box_bottom_position = Widget::Rectangle::GetPosition().GetY() + GetSize().GetY();
        if (position.GetY() > (text_box_bottom_position)) {
            break;
        }
    }
}

// private ====================================================================

void TextBox::initText()
{
    text_.clear();
    addParagraph();
}

void TextBox::addParagraph()
{
    text_.emplace_back();
    text_.back().first.SetFont(font_);
    text_.back().first.SetFontSize(font_.GetBaseSize());
    text_.back().first.SetColor(textColor_);
}

void TextBox::update(float dt) {
    ;
}

}  // namespace ui::widgets

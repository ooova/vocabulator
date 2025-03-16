#include "text-box.h"

#include "spdlog/spdlog.h"

namespace ui::widgets {

TextBox::TextBox(RVector2 position, RVector2 size, RFont&& font,
                 RColor textColor, RColor backgroundColor)
    : RRectangle(position.GetX(), position.GetY(), size.GetX(), size.GetY())
    , font_{std::move(font)}
    , textColor_{textColor}
    , backgroundColor_{backgroundColor}
{
    clear();
}

TextBox& TextBox::operator<<(std::string_view text)
{
    const auto boxWidth = RRectangle::GetWidth() - text_.back().first.GetFontSize();
    for (const auto ch : text) {
        if (ch == '\n') {
            addParagraph();
            continue;
        }
        if (!text_.empty()) {
            auto& ray_text = text_.back().first; // GetText()
            if (!ray_text.GetText().empty()) {
                if (boxWidth <= ray_text.MeasureEx().GetX()) {
                    // check if text field is not empty
                    const auto ch = ray_text.GetText().back();
                    ray_text.text.back() = '\n';
                    ray_text.text.push_back(ch);
                }
            }
        }
        text_.back().first.SetText(text_.back().first.GetText() + std::string{ch});
    }
    return *this;
}

void TextBox::setText(std::string_view const& text)
{
    initText();
    operator<<(text);
}

void TextBox::setAlignment(Alignment alignment) { text_.back().second = alignment; }

// void setFont(std::string_view const font_file_path) {
//     spdlog::info("Font {} loaded", font_file_path);
//     std::for_each(text_.begin(), text_.end(), [this](auto& paragraph){
//         ::UnloadFont(paragraph.font);
//         paragraph.font = font_;
//     });
// }

void TextBox::draw()
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
        if (position.GetY() >
            RRectangle::GetPosition().GetY() + RRectangle::GetSize().GetY()) {
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
    text_.back().first.SetFontSize(16);
    text_.back().first.SetColor(textColor_);
}

void TextBox::update(float dt) {
    ;
}

}  // namespace ui::widgets

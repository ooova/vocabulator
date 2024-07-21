#ifndef UI_WIDGETS_TEXT_BOX_H
#define UI_WIDGETS_TEXT_BOX_H

#include "raylib-cpp.hpp"
#include "spdlog/spdlog.h"

#include <string_view>
#include <memory>

namespace ui::widgets {

class TextBox : public RRectangle {
public:
    enum class Alignment {
        kLeft,
        kCenter,
        kRight,
    };

    TextBox(RVector2 position, RVector2 size, RFont&& font, RColor textColor = RColor::White(), RColor backgroundColor = {0x00})
        : RRectangle(position.GetX(), position.GetY(), size.GetX(), size.GetY())
        , font_{std::move(font)}
        , textColor_{textColor}
        , backgroundColor_{backgroundColor}
    {
        clear();
    }

    TextBox& operator<<(std::string_view text) {
        const auto boxWidth = RRectangle::GetWidth() - text_.back().first.GetFontSize();
        for (const auto ch : text) {
            if (ch == '\n') {
                addParagraph();
                continue;
            }
            if (boxWidth <= text_.back().first.MeasureEx().GetX()) {
                const auto ch = text_.back().first.text.back();
                text_.back().first.text.back() = '\n';
                text_.back().first.text.push_back(ch);
            }
            text_.back().first.SetText(text_.back().first.GetText() + std::string{ch});
        }
        return *this;
    }

    void clear() {
        initText();
    }

    void setText(std::string_view const& text) {
        initText();
        operator<<(text);
    }

    void setAlignment(Alignment alignment) {
        text_.back().second = alignment;
    }

    // void setFont(std::string_view const font_file_path) {
    //     spdlog::info("Font {} loaded", font_file_path);
    //     std::for_each(text_.begin(), text_.end(), [this](auto& paragraph){
    //         ::UnloadFont(paragraph.font);
    //         paragraph.font = font_;
    //     });
    // }

    void draw() {
        RRectangle::Draw(backgroundColor_);
        RVector2 position = RRectangle::GetPosition();
        for (const auto& paragraph : text_) {
            auto paragraph_size{paragraph.first.MeasureEx()};
            switch (paragraph.second) {
                case Alignment::kCenter: {
                    position.SetX((RRectangle::GetPosition().GetX() + RRectangle::GetSize().GetX() / 2) - (paragraph_size.GetX() / 2));
                }
                break;
                case Alignment::kRight: {
                    position.SetX((RRectangle::GetPosition().GetX() + RRectangle::GetSize().GetX()) - paragraph_size.GetX());
                }
                break;
                default:
                    position.SetX(RRectangle::GetPosition().GetX());
                    break;
            }
            paragraph.first.Draw(position);
            position.SetY(position.GetY() + paragraph_size.GetY());
            if (position.GetY() > RRectangle::GetPosition().GetY() + RRectangle::GetSize().GetY()) {
                break;
            }
        }
    }

private:
    RFont font_{};
    RColor textColor_{RColor::Black()};
    RColor backgroundColor_{RColor::White()};

    std::vector<std::pair<RText, Alignment>> text_{};

    void initText() {
        text_.clear();
        addParagraph();
    }

    void addParagraph() {
        text_.emplace_back();
        text_.back().first.SetFont(font_);
        text_.back().first.SetFontSize(16);
        text_.back().first.SetColor(textColor_);
    }
};

} // namespace ui::widgets

#endif // UI_WIDGETS_TEXT_BOX_H

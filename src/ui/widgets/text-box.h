#ifndef UI_WIDGETS_TEXT_BOX_H
#define UI_WIDGETS_TEXT_BOX_H

#include "ui/widgets/widget.h"

#include "raylib-cpp.hpp"

#include <string_view>

namespace ui::widgets {

class TextBox : public Widget {
public:
    enum class Alignment {
        kLeft,
        kCenter,
        kRight,
    };

    TextBox(RVector2 position, RVector2 size, RFont&& font,
            RColor textColor = RColor::White(), RColor backgroundColor = RColor(0x00));

    TextBox& operator<<(std::string_view text);

    void clear() { initText(); }

    void setText(std::string_view const& text);

    void setAlignment(Alignment alignment);

    void setTextColor(RColor const& color) { textColor_ = color; }
    RColor getTextColor() const { return textColor_; }

    void setBackgroundColor(RColor const& color) { backgroundColor_ = color; }
    RColor getBackgroundColor() const { return backgroundColor_; }

    std::string getText() const {
        std::string result;
        for (const auto& paragraph : text_) {
            result += paragraph.first.text;
        }
        return result;
    }

    void draw() const override;

    void update(float dt) override;

protected:
    std::vector<std::pair<RText, Alignment>> text_{};

private:
    RFont font_{};
    RColor textColor_{RColor::Black()};
    RColor backgroundColor_{RColor::White()};

    void initText();

    void addParagraph();
};

}  // namespace ui::widgets

#endif  // UI_WIDGETS_TEXT_BOX_H

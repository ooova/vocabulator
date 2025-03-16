#ifndef UI_WIDGETS_TEXT_BOX_H
#define UI_WIDGETS_TEXT_BOX_H

#include <string_view>

#include "raylib-cpp.hpp"

namespace ui::widgets {

class TextBox : public RRectangle {
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

    // void setFont(std::string_view const font_file_path);
    // void showCursor(bool show);

    virtual void draw();

protected:
    std::vector<std::pair<RText, Alignment>> text_{};

private:
    RFont font_{};
    RColor textColor_{RColor::Black()};
    RColor backgroundColor_{RColor::White()};
    // bool show_cursor_{false};

    void initText();

    void addParagraph();
};

}  // namespace ui::widgets

#endif  // UI_WIDGETS_TEXT_BOX_H

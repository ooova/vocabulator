#include "button.h"

#include <algorithm>

namespace ui::widgets {

Button::Button(RVector2 position, RVector2 size, std::string_view const text,
        ClickCallback clickCallback, RFont&& font, RColor color)
    : RRectangle(position.GetX(), position.GetY(), size.GetX(), size.GetY())
    , font_{std::move(font)}
    , button_color_{color}
    , clickCallback_{clickCallback}
{
    setText(text);
    spdlog::trace("\ntext width: {}\nbutton width: {}\nX position: {}",
                    text_.MeasureEx().GetX(), RRectangle::GetSize().GetX(),
                    RRectangle::GetPosition().GetX());
}

Button::~Button() { stop_ = true; }

void Button::setText(std::string_view const text)
{
    text_ = RText(font_, std::string(text), font_.GetBaseSize());
}

void Button::draw() const
{
    Draw(button_color_);
    
    // const auto lines_count = std::count(text_.GetText().begin(), text_.GetText().end(), '\n');
    // const auto text_height = font_.GetBaseSize() * lines_count;
    // const auto text_width = text_.MeasureEx().GetX();
    // const auto text_x = GetPosition().GetX() + (GetSize().GetX() / 2 - text_width / 2);
    // const auto text_y = GetPosition().GetY() + (GetSize().GetY() / 2 - text_height / 2);
    // text_.Draw(text_x, text_y);
    text_.Draw(GetPosition().GetX() + (GetSize().GetX() / 2 - text_.MeasureEx().GetX() / 2),
                GetPosition().GetY() + (GetSize().GetY() / 2 - text_.MeasureEx().GetY() / 2));
}

void Button::update(float dt) {
    static auto clicked{false};
    if (!clicked &&
        RMouse::IsButtonDown(::MOUSE_LEFT_BUTTON) &&
        CheckCollision(::GetMousePosition()) &&
        clickCallback_) {
        clicked = true;
        clickCallback_();
    }
    if (RMouse::IsButtonUp(::MOUSE_LEFT_BUTTON)) {
        clicked = false;
    }
}

}  // namespace ui::widgets

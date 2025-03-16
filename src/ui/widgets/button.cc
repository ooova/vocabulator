#include "button.h"

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
    text_ = RText(font_, std::string(text), kFontSize_);
}

void Button::draw() const
{
    RRectangle::Draw(button_color_);
    text_.Draw(RRectangle::GetPosition().GetX() +
                    (RRectangle::GetSize().GetX() / 2 - text_.MeasureEx().GetX() / 2),
                RRectangle::GetPosition().GetY() +
                    (RRectangle::GetSize().GetY() / 2 - kFontSize_ / 2));
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

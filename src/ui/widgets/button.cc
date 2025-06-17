#include "button.h"

#include "common/events/event_dispatcher.h"
#include "ui/events/mouse_events.h"

#include <algorithm>

namespace ui::widgets {

Button::Button(RVector2 position, RVector2 size, std::string_view const text,
        ClickCallback clickCallback, RFont&& font, RColor color)
    : Widget(position, size)
    , font_{std::move(font)}
    , button_color_{color}
    , clickCallback_{clickCallback}
{
    setText(text);

    auto& event_dispatcher = common::EventDispatcher::instance();
    event_dispatcher.subscribe<events::MouseEvent>(
        [this](events::MouseEvent const& event) {
            handleMouseEvent(event);
        });
}

void Button::setText(std::string_view const text)
{
    text_ = RText(font_, std::string(text), font_.GetBaseSize());
}

void Button::draw() const
{
    Draw(button_color_);

    text_.Draw(GetPosition().GetX() + (GetSize().GetX() / 2 - text_.MeasureEx().GetX() / 2),
                GetPosition().GetY() + (GetSize().GetY() / 2 - text_.MeasureEx().GetY() / 2));
}

void Button::update([[maybe_unused]] float dt) 
{
}

// private ------------------------------------------------------------

void Button::handleMouseEvent(events::MouseEvent const& event)
{
    if (event.button == ui::events::MouseEvent::Button::kLeft &&
        CheckCollision(RVector2{event.x, event.y})) {
        if (clickCallback_) {
            clickCallback_();
        }
    }
}

}  // namespace ui::widgets

#ifndef UI_WIDGETS_BUTTON_H
#define UI_WIDGETS_BUTTON_H

#include <chrono>
#include <string_view>
#include <thread>

#include "raylib-cpp.hpp"
#include "spdlog/spdlog.h"
#include "tools/scoped_async_wrapper.h"
#include "ui/events/event.h"

namespace ui::widgets {

class Button : public RRectangle {
public:
    using ClickCallback = void (*)(void);

    Button(RVector2 position, RVector2 size, std::string_view const text,
           ClickCallback clickCallback, RFont&& font = {}, RColor color = RColor::Gray())
        : RRectangle(position.GetX(), position.GetY(), size.GetX(), size.GetY())
        , font_{std::move(font)}
        , button_color_{color}
        , clickCallback_{clickCallback}
        , button_click_poll_{[this]() {
            auto clicked{RMouse::IsButtonPressed(::MOUSE_LEFT_BUTTON)};
            while (!stop_) {
                if (clickCallback_ && !clicked &&
                    RMouse::IsButtonPressed(::MOUSE_LEFT_BUTTON) &&
                    CheckCollision(::GetMousePosition())) {
                    clicked = true;
                    clickCallback_();
                }
                clicked = RMouse::IsButtonDown(::MOUSE_LEFT_BUTTON);
                std::this_thread::sleep_for(kPollingSleepTimeMS);
            }
        }}
    {
        setText(text);
        spdlog::trace("\ntext width: {}\nbutton width: {}\nX position: {}",
                      text_.MeasureEx().GetX(), RRectangle::GetSize().GetX(),
                      RRectangle::GetPosition().GetX());
    }

    ~Button() { stop_ = true; }

    void setText(std::string_view const text)
    {
        text_ = RText(font_, std::string(text), kFontSize_);
    }

    void draw() const
    {
        RRectangle::Draw(button_color_);
        text_.Draw(RRectangle::GetPosition().GetX() +
                       (RRectangle::GetSize().GetX() / 2 - text_.MeasureEx().GetX() / 2),
                   RRectangle::GetPosition().GetY() +
                       (RRectangle::GetSize().GetY() / 2 - kFontSize_ / 2));
    }

private:
    std::chrono::milliseconds const kPollingSleepTimeMS{20};
    int const kFontSize_{16};

    RFont const font_{};
    RColor const button_color_{};
    RText text_{};
    ClickCallback clickCallback_{};
    tools::AsyncWrapper button_click_poll_;
    std::atomic_bool stop_{false};
};

}  // namespace ui::widgets

#endif  // UI_WIDGETS_BUTTON_H

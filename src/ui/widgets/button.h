#ifndef UI_WIDGETS_BUTTON_H
#define UI_WIDGETS_BUTTON_H

#include "ui/widgets/widget.h"

#include "raylib-cpp.hpp"
#include "spdlog/spdlog.h"

#include <chrono>
#include <functional>
#include <string_view>
#include <thread>
#include <atomic>

namespace ui {

namespace events {
    struct MouseEvent;
} // namespace events

namespace widgets {

class Button : public Widget {
public:
    using ClickCallback = std::function<void(void)>;

    Button(RVector2 position, RVector2 size, std::string_view const text, 
           ClickCallback clickCallback, RFont&& font = {}, RColor color = RColor::Gray());

    ~Button() = default;

    void setText(std::string_view const text);

    void draw() const override;

    void update(float dt) override;

private:
    RFont const font_{};
    RColor const button_color_{};
    RText text_{};
    ClickCallback clickCallback_{};

    void handleMouseEvent(events::MouseEvent const& event);
};

}  // namespace widgets

}  // namespace ui

#endif  // UI_WIDGETS_BUTTON_H

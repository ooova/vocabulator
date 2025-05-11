#ifndef UI_WIDGETS_BUTTON_H
#define UI_WIDGETS_BUTTON_H

#include <chrono>
#include <functional>
// #include <memory>
#include <string_view>
#include <thread>

#include "raylib-cpp.hpp"
#include "spdlog/spdlog.h"
#include "tools/scoped_async_wrapper.h"
#include "ui/events/event.h"

namespace ui::widgets {

class Button : public RRectangle {
public:
    // using ClickCallback = void (*)(void);
    using ClickCallback = std::function<void(void)>;

    Button(RVector2 position, RVector2 size, std::string_view const text,
           ClickCallback clickCallback, RFont&& font = {}, RColor color = RColor::Gray());

    ~Button();

    void setText(std::string_view const text);

    void draw() const;

    void update(float dt);

private:
    std::chrono::milliseconds const kPollingSleepTimeMS{100};

    RFont const font_{};
    RColor const button_color_{};
    RText text_{};
    ClickCallback clickCallback_{};
    std::atomic_bool stop_{false};
};

}  // namespace ui::widgets

#endif  // UI_WIDGETS_BUTTON_H

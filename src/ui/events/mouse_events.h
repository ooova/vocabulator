#ifndef UI_EVENTS_MOUSE_EVENTS_H
#define UI_EVENTS_MOUSE_EVENTS_H

#include "common/events/event.h"

namespace ui::events {

struct MouseEvent : public common::Event {
    enum class Button {
        kNone,
        kLeft,
        kRight,
        kMiddle,
    };

    enum class Action {
        kNone,
        kPress,
        kRelease,
        kClick,
    };

    float x;
    float y;
    Button button;
    // Action action;
};

} // namespace ui::events

#endif // UI_EVENTS_MOUSE_EVENTS_H
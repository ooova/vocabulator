#ifndef UI_EVENTS_EVENT_KEYBOARD_H
#define UI_EVENTS_EVENT_KEYBOARD_H

#include "common/events/event.h"

#include <vector>

namespace ui::events {

struct KeyboardEvent : public common::Event {
    int key;
    std::vector<int> codepoints;
};

} // namespace ui::events

#endif // UI_EVENTS_EVENT_KEYBOARD_H

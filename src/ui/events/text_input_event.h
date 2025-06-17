#ifndef UI_EVENTS_TEXT_INPUT_EVENT_H
#define UI_EVENTS_TEXT_INPUT_EVENT_H

#include "common/events/event.h"
#include <string>

namespace ui::events {

struct TextInputEvent : public common::Event {
    std::string text;
};

} // namespace ui::events

#endif // UI_EVENTS_TEXT_INPUT_EVENT_H

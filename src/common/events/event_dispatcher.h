#ifndef COMMON_EVENTS_EVENT_DISPATCHER_H
#define COMMON_EVENTS_EVENT_DISPATCHER_H

#include "common/events/event.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

namespace common {

// // Templated Event class for specific event types
// template<typename T>
// class TypedEvent : public Event {
// public:
//     explicit TypedEvent(const T& data) : data_(data) {}
//     const T& getData() const { return data_; }

// private:
//     T data_;
// };

// Event Handler function type
using EventHandler = std::function<void(const Event&)>;

// Event Dispatcher class
class EventDispatcher {
private:
    EventDispatcher() = default;

public:
    ~EventDispatcher() = default;

    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    static EventDispatcher& instance() {
        static EventDispatcher instance;
        return instance;
    }

    // Register a handler for a specific event type
    template<typename T>
    void subscribe(std::function<void(T const&)> handler) {
        handlers_[std::type_index(typeid(T))].push_back(
            [handler](const Event& event) {
                handler(static_cast<const T&>(event));
            }
        );
    }

    // Dispatch an event with specific data
    template<typename T>
    void dispatch(const T& event) {
        auto type = std::type_index(typeid(T));
        
        if (handlers_.find(type) != handlers_.end()) {
            for (const auto& handler : handlers_[type]) {
                if (handler) {
                    handler(event);
                }
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<EventHandler>> handlers_;
};

} // namespace common

#endif // COMMON_EVENTS_EVENT_DISPATCHER_H

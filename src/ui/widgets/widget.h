#ifndef UI_WIDGETS_WIDGET_H
#define UI_WIDGETS_WIDGET_H

#include "raylib-cpp.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>

namespace ui {

class Widget : public RRectangle, public std::enable_shared_from_this<Widget> {
public:
    using Ptr = std::shared_ptr<Widget>;
    using WeakPtr = std::weak_ptr<Widget>;

    Widget() : RRectangle(0, 0, 0, 0) {}
    Widget(const RVector2& position, const RVector2& size)
        : RRectangle(position, size) {}
    virtual ~Widget() = default;

    // Prevent copying
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    // Allow moving
    Widget(Widget&&) = default;
    Widget& operator=(Widget&&) = default;

    // Basic widget properties
    virtual void setPosition(const RVector2& pos) { 
        SetPosition(pos);
    }
    virtual void setSize(const RVector2& size) { 
        SetSize(size);
    }
    virtual void setVisible(bool visible) { visible_ = visible; }
    virtual void setEnabled(bool enabled) { enabled_ = enabled; }
    virtual void setFocused(bool focused) { focused_ = focused; }

    const RVector2 getPosition() const { return {GetX(), GetY()}; }
    const RVector2 getSize() const { return {GetWidth(), GetHeight()}; }
    bool isVisible() const { return visible_; }
    bool isEnabled() const { return enabled_; }
    bool isFocused() const { return focused_; }

    // Widget hierarchy
    void addChild(Ptr child) {
        if (child) {
            child->parent_ = weak_from_this();
            children_.push_back(child);
        }
    }

    void removeChild(const Ptr& child) {
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end()) {
            (*it)->parent_.reset();
            children_.erase(it);
        }
    }

    const std::vector<Ptr>& getChildren() const { return children_; }
    Ptr getParent() const { return parent_.lock(); }

    virtual void draw() const = 0;
    virtual void update(float dt) = 0;

protected:
    bool visible_{true};
    bool enabled_{true};
    bool focused_{false};
    std::vector<Ptr> children_;
    WeakPtr parent_;
};

} // namespace ui

#endif // UI_WIDGETS_WIDGET_H
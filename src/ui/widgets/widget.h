#ifndef WIDGET_H
#define WIDGET_H

#include "raylib-cpp.hpp"

class Widget {
    public:
        Widget(RVector2 position, RVector2 size)
            : position_(position), size_(size) {}
        virtual ~Widget() = default;
        virtual void draw() = 0;
        virtual void update(float dt) = 0;
    protected:
        RVector2 position_;
        RVector2 size_;
};

#endif // WIDGET_H
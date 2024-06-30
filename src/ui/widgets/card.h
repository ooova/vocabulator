#ifndef UI_WIDGETS_CARD_H
#define UI_WIDGETS_CARD_H

#include "ui/widgets/text-box.h"
#include "vocabulary/word.h"

#include "raylib-cpp.hpp"

namespace ui::widgets {

class Card {
public:
    Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font = {});

    void draw();
private:
    TextBox text_box_;
};

} // namespace ui::widgets

#endif // UI_WIDGETS_CARD_H
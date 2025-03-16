#ifndef UI_WIDGETS_CARD_H
#define UI_WIDGETS_CARD_H

#include "ui/widgets/text-box.h"
#include "vocabulary/vocabulary.h"

#include "raylib-cpp.hpp"

namespace ui::widgets {

class Card {
public:
    Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font = {});

    void draw();
    void setWord(vocabulary::Word const& word);
    void update(float dt);
private:
    TextBox text_box_;
};

} // namespace ui::widgets

#endif // UI_WIDGETS_CARD_H
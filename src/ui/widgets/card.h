#ifndef UI_WIDGETS_CARD_H
#define UI_WIDGETS_CARD_H

#include "ui/widgets/widget.h"
#include "ui/widgets/text-box.h"
#include "vocabulary/word.h"

#include "raylib-cpp.hpp"

namespace ui::widgets {

class Card : public TextBox {
public:
    Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font = {});

    void draw() const override;
    void update(float dt) override;

    void setWord(vocabulary::Word const& word);
    void setPosition(RVector2 const& position) override;
};

} // namespace ui::widgets

#endif // UI_WIDGETS_CARD_H
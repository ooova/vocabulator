#include "ui/widgets/card.h"

#include <format>

namespace ui::widgets {

Card::Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font)
    : TextBox(position, size, std::move(font))
{
    // setWord(word);
}

void Card::draw() const { TextBox::draw(); }

void Card::setWord(vocabulary::Word const& word)
{
    clear();
    operator<<(word.word());
    setAlignment(TextBox::Alignment::kCenter);
    operator<<("\n----");
    setAlignment(TextBox::Alignment::kCenter);
    operator<<("\n");
    operator<<(word.translation().variantsToString());
    setAlignment(TextBox::Alignment::kCenter);
    if (!word.translation().examples().empty()) {
        operator<<("\n(");
        operator<<(word.translation().examplesToString());
        operator<<(")");
        setAlignment(TextBox::Alignment::kCenter);
    }
}

void Card::setPosition(RVector2 const& position)
{
    TextBox::setPosition(position);
}

void Card::update(float dt)
{
    ;
}

}  // namespace ui::widgets

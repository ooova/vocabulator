#include "ui/widgets/card.h"

namespace ui::widgets {

Card::Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font)
    : text_box_{position, size, std::move(font)}
{
    // setWord(word);
}

void Card::draw() { text_box_.draw(); }

void Card::setWord(vocabulary::Word const& word)
{
    text_box_.clear();
    text_box_ << word.word();
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    text_box_ << "\n----";
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    text_box_ << "\n";
    text_box_ << word.translation().variantsToString();
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    if (!word.translation().examples().empty()) {
        text_box_ << "\n(";
        text_box_ << word.translation().examplesToString();
        text_box_ << ")";
        text_box_.setAlignment(TextBox::Alignment::kCenter);
    }
}

}  // namespace ui::widgets

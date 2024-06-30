#include "ui/widgets/card.h"

namespace ui::widgets {

Card::Card(RVector2 position, RVector2 size, vocabulary::Word const& word, RFont&& font)
    : text_box_{position, size, std::move(font)}
{
    text_box_ << word.first;
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    text_box_ << "\n----";
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    text_box_ << "\n";
    text_box_ << word.second.first.back();
    text_box_.setAlignment(TextBox::Alignment::kCenter);
    text_box_ << "\n(";
    text_box_ << word.second.second.back();
    text_box_ << ")";
    text_box_.setAlignment(TextBox::Alignment::kCenter);
}

void Card::draw() {
    text_box_.draw();
}

} // namespace ui::widgets

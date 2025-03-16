#include "text-input.h"

#include "spdlog/spdlog.h"

#include <iterator>

namespace ui::widgets {

void TextInput::update(float dt)
{
    if (CheckCollision(RMouse::GetPosition())) {
        RMouse::SetCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        RMouse::SetCursor(MOUSE_CURSOR_DEFAULT);
    }

    const auto key_code = RKeyboard::GetKeyPressed();
    const auto char_code = RKeyboard::GetCharPressed();
    if (key_code) {
        spdlog::trace("key code: \'{}\', char code: \'{}\'", key_code, char_code);
        switch (key_code) {
            case static_cast<int>(KeyCodes::kUp):
                moveCursorUp();
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            case static_cast<int>(KeyCodes::kRight):
                moveCursorRight();
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            case static_cast<int>(KeyCodes::kDown):
                moveCursorDown();
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            case static_cast<int>(KeyCodes::kLeft):
                moveCursorLeft();
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            case static_cast<int>(KeyCodes::kBackspace):
                deleteCharacter(true);
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            case static_cast<int>(KeyCodes::kDel):
                deleteCharacter(false);
                spdlog::trace("cursor position \'{}\': ", cursorPosition_.ToString());
                break;
            default:
                // spdlog::info("no dedicated method to handle key code")
                if (char_code) {
                    auto ch = static_cast<char>(char_code);
                    spdlog::trace("char: {}", ch);
                    insertCharacter(ch);
                }
                break;
        }
    }

    cursorTimer_ += dt;
    if (cursorTimer_ >= cursorBlinkInterval_) {
        cursorTimer_ = 0.0f;
        cursorVisible_ = !cursorVisible_;
    }
}

// void TextInput::draw() {
//     TextBox::draw();

//     // grok
//     // Отрисовываем курсор, если он виден
//     if (cursorVisible_) {
//         try {
//             RVector2 cursorPos = RRectangle::GetPosition();
//             const auto& paragraph = text_.at(cursorPosition_.GetY());
//             const auto& text = paragraph.first;
//             float fontSize = text.GetFontSize();

//             // Вычисляем Y-позицию (смещение по строкам)
//             cursorPos.SetY(cursorPos.GetY() + cursorPosition_.GetY() * fontSize);

//             // Вычисляем X-позицию в зависимости от выравнивания
//             std::string_view textBeforeCursor = std::string_view(text.text).substr(0, cursorPosition_.GetX());
//             float textWidthBeforeCursor = ::MeasureText(textBeforeCursor.data());//.GetX();

//             switch (paragraph.second) {
//                 case Alignment::kCenter: {
//                     float paragraphWidth = text.MeasureEx().GetX();
//                     cursorPos.SetX(RRectangle::GetPosition().GetX() +
//                                    (RRectangle::GetSize().GetX() - paragraphWidth) / 2 +
//                                    textWidthBeforeCursor);
//                 } break;
//                 case Alignment::kRight: {
//                     float paragraphWidth = text.MeasureEx().GetX();
//                     cursorPos.SetX(RRectangle::GetPosition().GetX() +
//                                    RRectangle::GetSize().GetX() - paragraphWidth +
//                                    textWidthBeforeCursor);
//                 } break;
//                 default: // kLeft
//                     cursorPos.SetX(cursorPos.GetX() + textWidthBeforeCursor);
//                     break;
//             }

//             // Параметры курсора
//             float cursorWidth = 2.0f; // Ширина курсора
//             float cursorHeight = fontSize; // Высота курсора
//             RColor cursorColor = RColor::Black(); // Цвет курсора

//             // Отрисовка вертикальной черты
//             DrawRectangleV(cursorPos, {cursorWidth, cursorHeight}, cursorColor);
//         } catch (const std::exception& ex) {
//             spdlog::error("Ошибка при отрисовке курсора: {}", ex.what());
//         }
//     }
// }

// grok: draw with cursor as _ symbol
void TextInput::draw() {
    if (cursorVisible_) {
        try {
            auto& text = text_.at(cursorPosition_.GetY()).first.text;
            if (cursorPosition_.GetX() < text.size()) {
                // Сохраняем оригинальный символ
                char originalChar = text[cursorPosition_.GetX()];
                // Заменяем символ на '_'
                text[cursorPosition_.GetX()] = '_';
                TextBox::draw(); // Отрисовываем текст с курсором
                // Восстанавливаем оригинальный символ
                text[cursorPosition_.GetX()] = originalChar;
            } else {
                // Если курсор в конце строки, добавляем '_'
                text += '_';
                TextBox::draw();
                text.pop_back(); // Удаляем временный '_'
            }
        } catch (const std::exception& ex) {
            spdlog::error("Ошибка при отрисовке курсора: {}", ex.what());
        }
    } else {
        TextBox::draw(); // Отрисовываем без курсора
    }
}

void TextInput::moveCursorLeft()
{
    cursorPosition_ -= {1, 0};
    normalizeCursor();
}

void TextInput::moveCursorRight()
{
    cursorPosition_ += {1, 0};
    normalizeCursor();
}

void TextInput::moveCursorDown()
{
    cursorPosition_ += {0, 1};
    normalizeCursor();
}

void TextInput::moveCursorUp()
{
    cursorPosition_ -= {0, 1};
    normalizeCursor();
}

// TODO: if X pos. of cursor is out of range - get it back to the range
//      ? what if text in the paragraph is empty?
void TextInput::normalizeCursor() {
    // Ensure cursor is not above the first line
    if (cursorPosition_.GetY() < 0) {
        cursorPosition_.SetY(0);
    }

    // Ensure cursor is not below the last line
    if (text_.empty()) {
        cursorPosition_.SetY(0);
    }
    else if (text_.size() <= cursorPosition_.GetY()) {
        cursorPosition_.SetY(text_.size() - 1);
    }

    // Ensure cursor is not to the left of the first character in the line
    if (cursorPosition_.GetX() < 0) {
        cursorPosition_.SetX(0);
    }

    const auto& current_paragraph = text_.at(cursorPosition_.GetY()).first.text;
    // Ensure cursor is not to the right of the last character in the line
    if (current_paragraph.empty()) {
        cursorPosition_.SetX(0);
    }
    else {
        auto const paragraph_length = current_paragraph.size();
        if (paragraph_length <= cursorPosition_.GetX()) {
            cursorPosition_.SetX(paragraph_length - 1);
        }
    }
}


// TODO: insert char into specific (Y-th) paragraph
// void TextInput::insertCharacter(char ch) {
//     TextBox::operator<<(std::string{ch});
// }
void TextInput::insertCharacter(char ch) {
    try {
        auto& text = text_.at(cursorPosition_.GetY()).first.text;
        const auto& index = cursorPosition_.GetX();
        if (text.empty()) {
            text.push_back(ch);
        }
        else {
            text.insert(index + 1, 1, ch);
        }
        moveCursorRight();
    } catch (std::exception const& ex) {
        spdlog::error("can not insert character at position \'{}\': ",
                      cursorPosition_.ToString(), ex.what());
    }
}


void TextInput::deleteCharacter(bool backspace)
{
    try {
        auto& text = text_.at(cursorPosition_.GetY()).first.text;
        const auto& index = cursorPosition_.GetX();
        if (backspace) {
            if (index > 0) {
                text.erase(index - 1, 1);
            }
            moveCursorLeft();
        }
        else {
            if (index < text.size()) {
                text.erase(index, 1);
            }
        }
        normalizeCursor();
        if (text.empty()) {
            deleteParagraph(backspace);
        }
    } catch (std::exception const& ex) {
        spdlog::error("can not remove character from position \'{}\': ",
                      cursorPosition_.ToString(), ex.what());
    }
}

void TextInput::deleteParagraph(bool backspace) {
    if (1 < text_.size()) {
        text_.erase(std::next(text_.begin(), static_cast<int>(cursorPosition_.GetY())));
        if (backspace) {
            moveCursorUp();
        } else {
            normalizeCursor();
        }
    }
}

}  // namespace ui::widgets

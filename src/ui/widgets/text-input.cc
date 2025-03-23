#include "text-input.h"

#include "spdlog/spdlog.h"

#include <iterator>
#include <codecvt>

namespace ui::widgets {

TextInput::TextInput(RVector2 position, RVector2 size, RFont&& font,
        RColor textColor, RColor backgroundColor)
    : TextBox(position, size, std::move(font), textColor, backgroundColor)
    , cursorPosition_{0}
    , textColor_{textColor}
{
    // TextBox::setText("Hello world\nHello from Mars\nI'm here");
    // spdlog::trace("lagger \'0\': {}", text_.at(0).first.font.glyphs[0].value);
}

void TextInput::update(float dt)
{
    if (RMouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollision(RMouse::GetPosition())) {
            in_focus_ = true;
            TextBox::setTextColor(textColor_);
        }
        else {
            TextBox::setTextColor(RColor::Gray());
            in_focus_ = false;
        }
    }

    if (CheckCollision(RMouse::GetPosition())) {
        RMouse::SetCursor(MOUSE_CURSOR_IBEAM);
    }
    else {
        RMouse::SetCursor(MOUSE_CURSOR_DEFAULT);
    }

    if (in_focus_) {
        handleInput();
    }

    cursor_timer_ += dt;
    if (cursor_timer_ >= cursor_blink_interval_) {
        cursor_timer_ = 0.0f;
        cursor_visible_ = !cursor_visible_;
    }
}

void TextInput::handleInput()
{
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
            case static_cast<int>(KeyCodes::kEnter):
                insertCharacter('\n');
                break;
            default:
                if (char_code) {

                    // auto ch = static_cast<char>(char_code);
                    spdlog::trace("char: {}", char_code);


                    insertCharacter(char_code);
                }
                break;
        }
    }
}

// void TextInput::draw() {
//     TextBox::draw();

//     // grok
//     // Отрисовываем курсор, если он виден
//     if (cursor_visible_) {
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
    if (cursor_visible_ && in_focus_) {
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

std::string TextInput::text(bool with_new_lines) const {
    std::string result;
    for (const auto& line : text_) {
        result += line.first.GetText() + (with_new_lines ? "\n" : "");
    }
    return result;
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

void TextInput::insertCharacter(char32_t ch) {
    try {
        // convert utf8 char to sts::string
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::string utf8_string = converter.to_bytes(ch);

        if (text_.empty()) {
            TextBox::setText(std::string{utf8_string});
        }
        else {
            std::string raw_text{};
            for (auto y = 0UL; y < text_.size(); ++y) {
                auto const& paragraph{text_.at(y).first.GetText()};
                if (y == cursorPosition_.GetY()) {
                    auto const chars_count{cursorPosition_.GetX() + 1};
                    raw_text += paragraph.substr(0, chars_count)
                                + utf8_string
                                + ((paragraph.size() < chars_count) ? "" : paragraph.substr(chars_count));
                } else {
                    raw_text += paragraph;
                }
                if (y < text_.size() - 1) {
                    raw_text += '\n';
                }
            }
            TextBox::setText(raw_text);
            if (utf8_string == "\n") {
                moveCursorDown();
            }
        }
        moveCursorRight();
    } catch (const std::out_of_range& e) {
        spdlog::error("Error: Out of range exception occurred while inserting character.");
    } catch (const std::exception& e) {
        spdlog::error("An unexpected error occurred: {}", e.what());
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

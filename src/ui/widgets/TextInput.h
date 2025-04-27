#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP

#include "raylib-cpp.hpp"
#include <string>

class TextInput {
public:
    TextInput(float x, float y, float width, float height, int maxLen = 32)
        : bounds(x, y, width, height),
          text(""),
          focused(false),
          cursorBlinkTimer(0.0f),
          showCursor(true),
          maxLength(maxLen),
          backgroundColor(RAYWHITE),
          textColor(BLACK),
          borderColor(DARKGRAY),
          cursorColor(BLACK),
          fontSize(20),
          spacing(2) {}

    void Update() {
        // Check for mouse click to focus/unfocus
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            focused = CheckCollisionPointRec(GetMousePosition(), bounds);
        }

        if (focused) {
            // Handle text input
            int key = GetCharPressed();
            while (key > 0) {
                // if (key >= 32 && key <= 126 && text.length() < maxLength) {
                //     text += static_cast<char>(key);
                // }
                std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                std::string utf8_string = converter.to_bytes(key);
                text+= utf8_string;
                key = GetCharPressed();
            }

            // Handle backspace
            if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
                text.pop_back();
            }

            // Update cursor blink
            cursorBlinkTimer += GetFrameTime();
            if (cursorBlinkTimer >= 0.5f) {
                showCursor = !showCursor;
                cursorBlinkTimer = 0.0f;
            }
        }
    }

    void Draw() {
        // Draw background
        bounds.Draw(backgroundColor);

        // Draw border
        DrawRectangleLinesEx(bounds, 2, borderColor);

        // Draw text
        raylib::Text textObj(text, fontSize, textColor);
        textObj.SetSpacing(spacing);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
        float textX = bounds.x + 5;
        float textY = bounds.y + (bounds.height - textSize.y) / 2;
        textObj.Draw({textX, textY});

        // Draw cursor if focused
        if (focused && showCursor) {
            float cursorX = textX + textSize.x + 2;
            float cursorY = textY;
            DrawLineEx({cursorX, cursorY}, {cursorX, cursorY + textSize.y}, 2, cursorColor);
        }
    }

    // Getters and setters
    std::string GetText() const { return text; }
    void SetText(const std::string& newText) { text = newText.substr(0, maxLength); }
    bool IsFocused() const { return focused; }
    void SetColors(raylib::Color bg, raylib::Color txt, raylib::Color border, raylib::Color cursor) {
        backgroundColor = bg;
        textColor = txt;
        borderColor = border;
        cursorColor = cursor;
    }

private:
    raylib::Rectangle bounds;
    std::string text;
    bool focused;
    float cursorBlinkTimer;
    bool showCursor;
    int maxLength;
    raylib::Color backgroundColor;
    raylib::Color textColor;
    raylib::Color borderColor;
    raylib::Color cursorColor;
    int fontSize;
    float spacing;
};

#endif

#include "common/exceptions/parsing_error.h"
#include "spdlog/spdlog.h"
#include "ui/main-window.h"
#include "ui/widgets/button.h"
#include "ui/widgets/text-box.h"
#include "vocabulary/vocabulary.h"

// #include <cmath> // NOLINT
#include <string_view>

// tests
#include "raylib-cpp.hpp"
#include "ui/widgets/text-input.h"
// #include "ui/tools/text.h"
#include "ui/tools/locale.h"
const std::string font_file_path_{"../assets/fonts/Ubuntu-R.ttf"};
std::array<ui::tools::Language, 4> char_set_{
    ui::tools::Language::kRU, ui::tools::Language::kEN, ui::tools::Language::kMathSymbols,
    ui::tools::Language::kSpecSymbols};
RFont makeFont(std::filesystem::path font_file_path, std::string_view char_set, int const font_size = 36)
{
    auto codepointsCount{0};
    auto codepoints =
        ::LoadCodepoints(char_set.data(), &codepointsCount);
    auto font{
        RFont(std::string(font_file_path), font_size, codepoints, codepointsCount)};
    ::UnloadCodepoints(codepoints);
    return font;
}

constexpr const auto kDefaultVocabularyPath{std::string_view{"../assets/vocabulary.md"}};

using namespace std::literals;

int main(void)
{
    spdlog::set_level(spdlog::level::level_enum::trace);

    try {
        auto prev_time{GetTime()};
        auto vocabulary{std::make_shared<vocabulary::Vocabulary>(kDefaultVocabularyPath)};

        auto window = ui::MainWindow(vocabulary);  // initWindow();

        // window.textBox().setText("uno due tre"sv);

        ui::widgets::TextInput input{{200, 100}, {100,100}, makeFont(font_file_path_, ui::tools::Locale::getAlphabet(char_set_))/* {} */};

        SetTargetFPS(40);
        //----------------------------------------------------------

        while (!window.ShouldClose()) {  // Detect window close button or ESC key
            auto time = GetTime();
            // Update
            //-----------------------------------------------------

            // Update player-controlled-box (box02)
            // boxB.x = GetMouseX() - boxB.width/2;
            // boxB.y = GetMouseY() - boxB.height/2;

            // Pause Box A movement
            // if (IsKeyPressed(KEY_SPACE)) pause = !pause;
            //-----------------------------------------------------

            // Draw
            //-----------------------------------------------------
            BeginDrawing();

            window.ClearBackground({0x28, 0x28, 0x28, 0xFF});

            window.draw();
            // textBox.Draw();

            // buttonPreviousWord.Draw();
            // buttonNextWord.Draw();

            // DrawFPS(10, 10);

            EndDrawing();
            //-----------------------------------------------------

            input.update(time - prev_time);
            input.draw();
            prev_time = time;
        }
    } catch (raylib::RaylibException const& ex) {
        spdlog::error("Can not initialize window: {}", ex.what());
    } catch (ParsingError const& ex) {
        spdlog::error("Parsing error: {}", ex.what());
    } catch (std::exception const& ex) {
        spdlog::error("Exception: {}", ex.what());
    }

    return 0;
}
















// /*******************************************************************************************
// *
// *   raylib-cpp [core] example - Basic window (adapted for HTML5 platform)
// *
// *   This example is prepared to compile for PLATFORM_WEB, PLATFORM_DESKTOP and PLATFORM_RPI
// *   As you will notice, code structure is slightly diferent to the other examples...
// *   To compile it for PLATFORM_WEB just uncomment #define PLATFORM_WEB at beginning
// *
// *   This example has been created using raylib 1.3 (www.raylib.com)
// *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
// *
// *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
// *
// ********************************************************************************************/

// #include "raylib-cpp.hpp"

// #if defined(PLATFORM_WEB)
//     #include <emscripten/emscripten.h>
// #endif

// //----------------------------------------------------------------------------------
// // Global Variables Definition
// //----------------------------------------------------------------------------------
// int screenWidth = 800;
// int screenHeight = 450;

// //----------------------------------------------------------------------------------
// // Module Functions Declaration
// //----------------------------------------------------------------------------------
// void UpdateDrawFrame(void);     // Update and Draw one frame

// //----------------------------------------------------------------------------------
// // Main Enry Point
// //----------------------------------------------------------------------------------
// int main()
// {
//     // Initialization
//     //--------------------------------------------------------------------------------------
//     raylib::Window window(screenWidth, screenHeight, "raylib-cpp [core] example - basic window");

// #if defined(PLATFORM_WEB)
//     emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
// #else
//     SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
//     //--------------------------------------------------------------------------------------

//     // Main game loop
//     while (!window.ShouldClose())    // Detect window close button or ESC key
//     {
//         UpdateDrawFrame();
//     }
// #endif

//     return 0;
// }

// //----------------------------------------------------------------------------------
// // Module Functions Definition
// //----------------------------------------------------------------------------------
// void UpdateDrawFrame(void)
// {
//     // Update
//     //----------------------------------------------------------------------------------
//     // TODO: Update your variables here
//     //----------------------------------------------------------------------------------

//     // Draw
//     //----------------------------------------------------------------------------------
//     BeginDrawing();

//         ClearBackground(RAYWHITE);

//         raylib::DrawText("Congrats! You created your first raylib-cpp window!", 190, 200, 20, LIGHTGRAY);

//     EndDrawing();
//     //----------------------------------------------------------------------------------
// }

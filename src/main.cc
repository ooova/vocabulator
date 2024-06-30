#include "ui/main-window.h"
#include "ui/widgets/button.h"
#include "ui/widgets/text-box.h"

// #include "raylib-cpp.hpp"
#include "spdlog/spdlog.h"

// #include <raylib.h>

#include <cmath> // NOLINT
#include <string_view>

// constexpr auto kBackgroundColor{0x181818};

// constexpr auto kScreenWidth{800};
// constexpr auto kScreenHeight{450};
// constexpr auto kScreenMargin{10};

// constexpr auto kButtonWidth{80};
// constexpr auto kButtonHeighth{30};

// RWindow initWindow() {
//     return RWindow(kScreenWidth, kScreenHeight, "Vocabulator");
// }

std::vector<
    std::map<
        std::string/*native*/, std::pair<
            std::vector<std::string>/*foreign*/, std::vector<std::string>/*examples*/
        >
    >
    > db{
    {
        {"раз", {{"one", "1"}, {"one day"}}},
        {"два", {{"two", "2"}, {"two times"}}}
    }
};

using namespace std::literals;

int main(void)
{
    spdlog::set_level(spdlog::level::level_enum::trace);

    try {
        auto window = ui::MainWindow();//initWindow();

        // window.textBox().setText("uno due tre"sv);

        SetTargetFPS(40);
        //----------------------------------------------------------

        while (!window.ShouldClose()) {    // Detect window close button or ESC key
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
        }
    }
    catch (raylib::RaylibException const& ex) {
        spdlog::error("Can not initialize window: {}", ex.what());
    }


    return 0;
}

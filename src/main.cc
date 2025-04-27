#include "common/exceptions/parsing_error.h"
#include "spdlog/spdlog.h"
#include "ui/main-window.h"
#include "ui/widgets/button.h"
#include "ui/widgets/text-box.h"
#include "vocabulary/vocabulary.h"

// #include <cmath> // NOLINT
#include <string_view>
#include <cstdlib> // std::getenv

#include "network/http/client/nttp_client.h"
// tests
#include "raylib-cpp.hpp"
#include "ui/widgets/text-input.h"
// #include "ui/tools/text.h"
#include "ui/tools/locale.h"
#include "nlohmann/json.hpp"


// #include "ui/widgets/TextInput.h"

constexpr const auto kDefaultVocabularyPath{std::string_view{"../assets/vocabulary.md"}};

using namespace std::literals;

int main(void)
{
    spdlog::set_level(spdlog::level::level_enum::trace);

    try {
        auto prev_time{GetTime()};
        auto vocabulary{std::make_shared<vocabulary::Vocabulary>(kDefaultVocabularyPath)};

        auto client{std::make_shared<network::HttpClient>()};

        auto window = ui::MainWindow(vocabulary, client);  // initWindow();

        SetTargetFPS(40);
        //----------------------------------------------------------

        // TextInput input(100, 200, 200, 40, 20);
        // input.SetColors(WHITE, BLACK, DARKGRAY, BLACK);

        while (!window.ShouldClose()) {  // Detect window close button or ESC key
            auto time = GetTime();
            // Update
            //-----------------------------------------------------

            // input.Update();

            //-----------------------------------------------------

            // Draw
            //-----------------------------------------------------
            BeginDrawing();

            window.ClearBackground({0x28, 0x28, 0x28, 0xFF});

            window.update(time - prev_time);
            window.draw();

            // input.Draw();
            EndDrawing();
            //-----------------------------------------------------

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

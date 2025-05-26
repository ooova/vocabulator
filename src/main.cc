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
// #include "ui/tools/font-manager.h"
#include "ui/tools/locale.h"
#include "nlohmann/json.hpp"

using namespace std::literals;

int main(void)
{
    spdlog::set_level(spdlog::level::level_enum::trace);

    try {
        auto prev_time{GetTime()};
        auto vocabulary{std::make_shared<vocabulary::Vocabulary>()};

        auto client{std::make_shared<network::HttpClient>()};

        auto window = ui::MainWindow(vocabulary, client);

        SetTargetFPS(40);

        while (!window.ShouldClose()) {  // Detect window close button or ESC key
            auto time = GetTime();

            // Update
            //-----------------------------------------------------

            window.update(time - prev_time);

            //-----------------------------------------------------

            // Draw
            //-----------------------------------------------------
            BeginDrawing();

            window.draw();

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

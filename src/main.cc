#include "spdlog/spdlog.h"

#include "common/config/config.h"
#include "network/http/client/nttp_client.h"
#include "ui/main-window.h"
#include "vocabulary/vocabulary.h"

using namespace std::literals;

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::level_enum::trace);

    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

    try {
        auto prev_time{GetTime()};

        if (argc == 2) {
            common::Config::instance().setConfigFilePath(argv[1]);
        } else {
            spdlog::info("No command line arguments provided, loading default config");
            common::Config::instance().setConfigFilePath("assets/config");
        }
        common::Config::instance().loadFromFile();

        auto vocabulary{std::make_shared<vocabulary::Vocabulary>()};
        auto client{std::make_shared<network::HttpClient>()};
        auto window = ui::MainWindow(vocabulary, client);


        while (!window.ShouldClose()) {  // Detect window close button or ESC key
            auto time = GetTime();
            window.update(time - prev_time);
            window.draw();
            prev_time = time;
        }
    } catch (raylib::RaylibException const& ex) {
        spdlog::error("Can not initialize window: {}", ex.what());
    } catch (std::exception const& ex) {
        spdlog::error("Exception: {}", ex.what());
    }

    return 0;
}

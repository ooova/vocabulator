#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <array>
#include <memory>
#include <string>

#include "./tools/locale.h"
#include "./widgets/button.h"
#include "raylib-cpp.hpp"
#include "ui/widgets/card.h"
#include "ui/widgets/text-input.h"
#include "network/http/request.h"

namespace vocabulary {
class Vocabulary;
}

namespace network {
class HttpClient;
}  // namespace network

namespace ui {

constexpr int kBackgroundColor{0x181818};

constexpr int kScreenWidth{800};
constexpr int kScreenHeight{450};
constexpr int kScreenMargin{10};

constexpr int kButtonWidth{100};
constexpr int kButtonHeight{30};

constexpr int kElementMargin{5};

constexpr int kCardHeight{200};

class MainWindow : public RWindow {
public:
    MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary, std::weak_ptr<network::HttpClient> http_client);

    void draw();

    void update(float dt);

private:
    // const RVector2 kAddNewWordBoxPosition{
    //     kScreenMargin, kScreenMargin + kElementMargin + kButtonHeighth + kElementMargin
    //     +
    //                        kCardHeight + kElementMargin};
    // const RVector2 kAddNewWordBoxSize{kScreenWidth + 2 * kScreenMargin, kCardHeight};

    const std::string font_file_path_{"../assets/fonts/Ubuntu-R.ttf"};
    std::array<tools::Language, 4> char_set_{
        tools::Language::kRU, tools::Language::kEN, tools::Language::kMathSymbols,
        tools::Language::kSpecSymbols};

    std::weak_ptr<vocabulary::Vocabulary> vocabulary_;
    std::weak_ptr<network::HttpClient> http_client_;

    // widgets::Button button_previous_word_;
    widgets::Button button_add_word_to_batch_;
    widgets::Button button_next_word_;
    widgets::Button button_load_vocabulary_;
    widgets::Button button_save_vocabulary_;
    widgets::Button button_vocabulary_add_word_;
    widgets::Card card_;
    widgets::TextInput new_word_input_;
    widgets::TextInput new_word_translation_input_;
    widgets::TextInput new_word_example_input_;

    inline static std::string const default_server_{"localhost"};
    inline static std::string const default_port_{"1234"};
    inline static std::string const default_target_{"/v1/chat/completions"};
    inline static std::string const default_method_{"POST"};
    inline static std::vector<std::pair<std::string, std::string>> const default_headers_{
        {
            {"Content-Type", "application/json"},
        }
    };

    // methods

    std::shared_ptr<network::Request> createRequest(
        std::string const& request,
        network::Request::Callback callback,
        std::string const& server = default_server_,
        std::string const& port = default_port_,
        std::string const& target = default_target_,
        std::string const& method = default_method_,
        std::vector<std::pair<std::string, std::string>> const& headers =
            default_headers_);
};

}  // namespace ui

#endif  // UI_MAIN_WINDOW_H

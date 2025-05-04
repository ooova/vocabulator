#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include "network/http/request.h"
#include "ui/tools/font-manager.h"
#include "ui/widgets/button.h"
#include "ui/widgets/card.h"
#include "ui/widgets/text-input.h"

#include "raylib-cpp.hpp"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace vocabulary {
class Vocabulary;
}

namespace network {
class HttpClient;
}

namespace ui {

struct Config {
    static constexpr int kScreenWidth{800};
    static constexpr int kScreenHeight{450};
    static constexpr int kScreenMargin{10};
    static constexpr int kButtonWidth{100};
    static constexpr int kButtonHeight{30};
    static constexpr int kElementMargin{5};
    static constexpr int kCardHeight{200};
    static inline const RColor kBackgroundColor{0x181818};
    static inline const std::string kFontPath{"../assets/fonts/Ubuntu-R.ttf"};
    static inline const std::string kDefaultServer{"localhost"};
    static inline const std::string kDefaultPort{"1234"};
    static inline const std::string kDefaultTarget{"/v1/chat/completions"};
    static inline const std::string kDefaultMethod{"POST"};
    static inline const std::vector<std::pair<std::string, std::string>> kDefaultHeaders{
        {{"Content-Type", "application/json"}}};
};

class MainWindow : public RWindow {
public:
    MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary,
               std::weak_ptr<network::HttpClient> http_client,
               const Config& config = Config(),
               std::shared_ptr<ui::tools::FontManager> font_manager = nullptr);

    void draw();
    void update(float dt);

private:
    struct Layout {
        RVector2 button_load_vocabulary_pos;
        RVector2 button_save_vocabulary_pos;
        RVector2 button_vocabulary_add_word_pos;
        RVector2 button_add_word_to_batch_pos;
        RVector2 button_next_word_pos;
        RVector2 card_pos;
        RVector2 new_word_input_pos;
        RVector2 new_word_translation_input_pos;
        RVector2 new_word_example_input_pos;
        RVector2 card_size;
        RVector2 button_size;
        RVector2 input_size;
    };

    const Config config_;
    std::shared_ptr<ui::tools::FontManager> font_manager_;
    std::weak_ptr<vocabulary::Vocabulary> vocabulary_;
    std::weak_ptr<network::HttpClient> http_client_;
    Layout layout_;

    widgets::Button button_add_word_to_batch_;
    widgets::Button button_next_word_;
    widgets::Button button_load_vocabulary_;
    widgets::Button button_save_vocabulary_;
    widgets::Button button_vocabulary_add_word_;
    widgets::Card card_;
    widgets::TextInput new_word_input_;
    widgets::TextInput new_word_translation_input_;
    widgets::TextInput new_word_example_input_;
    std::string error_message_;

    void initWidgets();
    Layout calculateLayout() const;
    std::shared_ptr<network::Request> createRequest(
        const std::string& request,
        network::Request::Callback callback) const;
    void showError(const std::string& message);

    // Button callback methods
    void onAddWordToBatch();
    void onNextWord();
    void onLoadVocabulary();
    void onSaveVocabulary();
    void onAddWord();
    void handleTranslationRequest(const std::string& word);
};

}  // namespace ui

#endif  // UI_MAIN_WINDOW_H
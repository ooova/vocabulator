#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include "common/config/config.h"
#include "network/http/request.h"
#include "ui/tools/font-manager.h"
#include "ui/widgets/button.h"
#include "ui/widgets/card.h"
#include "ui/widgets/text-input.h"
#include "ui/widgets/text-box.h"

#include "raylib-cpp.hpp"

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace vocabulary {
class Vocabulary;
class Word;
}

namespace network {
class HttpClient;
}

namespace ui {

class MainWindow : public RWindow {
public:
    MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary,
               std::weak_ptr<network::HttpClient> http_client,
               std::shared_ptr<ui::tools::FontManager> font_manager = nullptr);

    void draw();
    void update(float dt);

private:
    using enum common::ConfigId;

    struct Layout {
        float scale_factor;
        float window_margin;
        float element_margin;

        RVector2 card_size;
        RVector2 button_size;
        RVector2 input_size;
        RVector2 text_box_vocabulary_statistics_size;
        RVector2 text_box_word_statistics_size;

        RVector2 button_load_vocabulary_pos;
        RVector2 button_save_vocabulary_pos;
        RVector2 button_add_word_to_batch_pos;
        RVector2 button_vocabulary_add_word_pos;
        RVector2 button_next_word_pos;
        RVector2 button_know_the_word_pos;
        RVector2 button_dont_know_the_word_pos;
        RVector2 card_pos;
        RVector2 input_new_word_pos;
        RVector2 input_new_word_translation_pos;
        RVector2 input_new_word_example_pos;
        RVector2 text_box_vocabulary_statistics_pos;
        RVector2 text_box_word_statistics_pos;
    };

    Layout layout_;

    static inline const std::vector<std::pair<std::string, std::string>> kDefaultHeaders{
        {{"Content-Type", "application/json"}}};

    common::Config& config_;
    std::shared_ptr<ui::tools::FontManager> font_manager_;
    std::weak_ptr<vocabulary::Vocabulary> vocabulary_;
    std::weak_ptr<network::HttpClient> http_client_;
    std::weak_ptr<vocabulary::Word> word_;

    std::unique_ptr<widgets::Button> button_add_word_to_batch_{nullptr};
    std::unique_ptr<widgets::Button> button_next_word_{nullptr};
    std::unique_ptr<widgets::Button> button_know_the_word_{nullptr};
    std::unique_ptr<widgets::Button> button_dont_know_the_word_{nullptr};
    std::unique_ptr<widgets::Button> button_load_vocabulary_{nullptr};
    std::unique_ptr<widgets::Button> button_save_vocabulary_{nullptr};
    std::unique_ptr<widgets::Button> button_vocabulary_add_word_{nullptr};
    std::unique_ptr<widgets::Card> card_{nullptr};
    std::unique_ptr<widgets::TextInput> input_new_word_{nullptr};
    std::unique_ptr<widgets::TextInput> input_new_word_translation_{nullptr};
    std::unique_ptr<widgets::TextInput> input_new_word_example_{nullptr};
    std::unique_ptr<widgets::TextBox> text_box_word_statistics_{nullptr};
    std::unique_ptr<widgets::TextBox> text_box_vocabulary_statistics_{nullptr};

    std::string error_message_{};
    std::string status_message_{};

    float status_message_timer_{};

    // methods
    void calculateLayout();
    void createUiElements();
    void updateUiElementsLayout();

    std::shared_ptr<network::Request> createRequest(
        const std::string& request,
        network::Request::Callback callback) const;

    void showError(const std::string& message);
    void showStatus(const std::string& message);

    // Button callback methods
    void onAddWordToBatch();
    void onNextWord();
    void onKnowTheWord();
    void onDontKnowTheWord();
    void onLoadVocabulary();
    void onSaveVocabulary();
    void onAddWord();
    void handleTranslationRequest(const std::string& word);

    // update statistics
    void updateWordStatisticsText();
    void updateVocabularyStatisticsText();
};

}  // namespace ui

#endif  // UI_MAIN_WINDOW_H
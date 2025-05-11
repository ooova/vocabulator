#include "main-window.h"

#include <array>
#include <functional>
#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "network/http/client/nttp_client.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "ui/tools/font-manager.h"
#include "ui/tools/locale.h"
#include "vocabulary/translation.h"
#include "vocabulary/vocabulary.h"
#include "vocabulary/word.h"

namespace ui {

using namespace std::literals;

MainWindow::MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary,
                       std::weak_ptr<network::HttpClient> http_client,
                       const Config& config,
                       std::shared_ptr<ui::tools::FontManager> font_manager)
    : RWindow(config.kScreenWidth, config.kScreenHeight, "Vocabulator")
    , config_(config)
    , font_manager_(font_manager ? font_manager
                                 : std::make_shared<ui::tools::FontManager>(
                                       config.kFontPath,
                                       std::vector<tools::Language>{
                                           tools::Language::kRU, tools::Language::kEN,
                                           tools::Language::kMathSymbols,
                                           tools::Language::kSpecSymbols}))
    , vocabulary_{vocabulary}
    , http_client_{http_client}
    , layout_{calculateLayout()}
    , button_add_word_to_batch_{layout_.button_add_word_to_batch_pos, layout_.button_size,
                                ui::tools::Locale::translateInterface("add to batch"),
                                [this] { onAddWordToBatch(); }, font_manager_->getFont()}
    , button_next_word_{layout_.button_next_word_pos, layout_.button_size,
                        ui::tools::Locale::translateInterface("next"),
                        [this] { onNextWord(); }, font_manager_->getFont()}
    , button_know_the_word_{layout_.button_know_the_word_pos, layout_.button_size,
                            ui::tools::Locale::translateInterface("know"),
                            [this] { onKnowTheWord(); }, font_manager_->getFont()}
    , button_dont_know_the_word_{layout_.button_dont_know_the_word_pos,
                                 layout_.button_size,
                                 ui::tools::Locale::translateInterface("don't know"),
                                 [this] { onDontKnowTheWord(); },
                                 font_manager_->getFont()}
    , button_load_vocabulary_{layout_.button_load_vocabulary_pos, layout_.button_size,
                              ui::tools::Locale::translateInterface("load vocabulary"),
                              [this] { onLoadVocabulary(); }, font_manager_->getFont()}
    , button_save_vocabulary_{layout_.button_save_vocabulary_pos, layout_.button_size,
                              ui::tools::Locale::translateInterface("save vocabulary"),
                              [this] { onSaveVocabulary(); }, font_manager_->getFont()}
    , button_vocabulary_add_word_{layout_.button_vocabulary_add_word_pos,
                                  layout_.button_size,
                                  ui::tools::Locale::translateInterface("add word"),
                                  [this] { onAddWord(); }, font_manager_->getFont()}
    , card_{layout_.card_pos, layout_.card_size, {}, font_manager_->getFont(config_.kCardFontSize)}
    , input_new_word_{layout_.input_new_word_pos, layout_.input_size,
                      font_manager_->getFont()}
    , input_new_word_translation_{layout_.input_new_word_translation_pos,
                                  layout_.input_size, font_manager_->getFont()}
    , input_new_word_example_{layout_.input_new_word_example_pos, layout_.input_size,
                              font_manager_->getFont()}
    , text_box_word_statistics_{
          layout_.text_box_word_statistics_pos, layout_.text_box_word_statistics_size,
          font_manager_->getFont(config_.kTextBoxWordStatisticsFontSize)}
{
    spdlog::info("MainWindow initialized with screen {}x{}", config_.kScreenWidth,
                 config_.kScreenHeight);

    onLoadVocabulary();
}

MainWindow::Layout MainWindow::calculateLayout() const
{
    Layout layout;
    layout.button_size = RVector2(static_cast<float>(config_.kButtonWidth),
                                  static_cast<float>(config_.kButtonHeight));
    layout.input_size = RVector2(static_cast<float>(config_.kButtonWidth),
                                 static_cast<float>(config_.kButtonHeight));
    layout.card_size = RVector2(static_cast<float>(config_.kScreenWidth - config_.kScreenMargin * 2),
                                static_cast<float>(config_.kCardHeight/* config_.kScreenHeight - config_.kScreenMargin * 2 - config_.kButtonHeight */));
    layout.element_margin = RVector2(static_cast<float>(config_.kElementMargin),
                                static_cast<float>(config_.kElementMargin));
    layout.text_box_word_statistics_size = RVector2(
        static_cast<float>(config_.kTextBoxWordStatisticsWidth),
        static_cast<float>(config_.kTextBoxWordStatisticsHeight));

    layout.button_load_vocabulary_pos =
        RVector2(static_cast<float>(config_.kScreenMargin + config_.kElementMargin),
                 static_cast<float>(config_.kScreenMargin + config_.kElementMargin));
    layout.button_save_vocabulary_pos =
        RVector2(layout.button_load_vocabulary_pos.x + config_.kButtonWidth +
                     config_.kScreenMargin + config_.kElementMargin,
                 layout.button_load_vocabulary_pos.y);
    layout.button_vocabulary_add_word_pos =
        RVector2(layout.button_save_vocabulary_pos.x + config_.kButtonWidth +
                     config_.kScreenMargin,
                 layout.button_save_vocabulary_pos.y);
    layout.button_add_word_to_batch_pos =
        RVector2(static_cast<float>(config_.kScreenWidth - config_.kScreenMargin -
                                    config_.kButtonWidth * 2 - config_.kElementMargin),
                 static_cast<float>(config_.kScreenMargin));
    layout.button_next_word_pos =
        RVector2(layout.button_add_word_to_batch_pos.x + config_.kButtonWidth +
                     config_.kElementMargin,
                 layout.button_add_word_to_batch_pos.y);
    layout.button_know_the_word_pos =
        RVector2{layout.button_load_vocabulary_pos.GetX(),
                 layout.button_load_vocabulary_pos.GetY() + layout.button_size.GetY() +
                     layout.element_margin.GetY()};
    layout.button_dont_know_the_word_pos =
        RVector2{layout.button_next_word_pos.GetX(), layout.button_next_word_pos.GetY() +
                                                         layout.button_size.GetY() +
                                                         layout.element_margin.GetY()};

    layout.card_pos =
        RVector2(static_cast<float>(config_.kScreenMargin + config_.kElementMargin),
                 static_cast<float>(config_.kScreenMargin * 2 + config_.kButtonHeight));

    layout.input_new_word_pos = RVector2(
        static_cast<float>(config_.kScreenMargin + config_.kElementMargin),
        layout.card_pos.GetY() + layout.card_size.GetY() + config_.kElementMargin);
    layout.input_new_word_translation_pos = RVector2(
        layout.input_new_word_pos.x + config_.kButtonWidth + config_.kElementMargin,
        layout.input_new_word_pos.y);
    layout.input_new_word_example_pos = RVector2(
        layout.input_new_word_translation_pos.x + config_.kButtonWidth + config_.kElementMargin,
        layout.input_new_word_translation_pos.y);

    layout.text_box_word_statistics_pos = RVector2(
        config_.kScreenWidth - config_.kScreenMargin - config_.kElementMargin - config_.kTextBoxWordStatisticsWidth,
        layout.card_pos.GetY() + config_.kElementMargin);

    return layout;
}

void MainWindow::draw()
{
    ClearBackground(config_.kBackgroundColor);
    button_add_word_to_batch_.draw();
    button_next_word_.draw();
    button_load_vocabulary_.draw();
    button_save_vocabulary_.draw();
    button_vocabulary_add_word_.draw();
    card_.draw();
    button_know_the_word_.draw();
    button_dont_know_the_word_.draw();
    input_new_word_.draw();
    input_new_word_translation_.draw();
    input_new_word_example_.draw();

    if (!error_message_.empty()) {
        ::DrawText(error_message_.c_str(), 10, config_.kScreenHeight - 20, 16, RColor::Red());
    }
    else if (!status_message_.empty()) {
        ::DrawText(status_message_.c_str(), 10, config_.kScreenHeight - 20, 16, RColor::Green());
    }

    RWindow::DrawFPS(10, 10);
}

void MainWindow::update(float dt)
{
    button_add_word_to_batch_.update(dt);
    button_next_word_.update(dt);
    button_know_the_word_.update(dt);
    button_dont_know_the_word_.update(dt);
    button_load_vocabulary_.update(dt);
    button_save_vocabulary_.update(dt);
    button_vocabulary_add_word_.update(dt);
    input_new_word_.update(dt);
    input_new_word_translation_.update(dt);
    input_new_word_example_.update(dt);

    if (!status_message_.empty() || !error_message_.empty()) {
        status_message_timer_ += dt;
        if (status_message_timer_ >= config_.kStatusMessageTimer) {
            status_message_timer_ = 0;
            error_message_.clear();
            status_message_.clear();
        }
    }
}

std::shared_ptr<network::Request> MainWindow::createRequest(
    const std::string& request, network::Request::Callback callback) const
{
    nlohmann::json body;
    // body["model"] = "gemma-3-4b-it";
    body["messages"] = nlohmann::json::array();
    body["messages"].push_back({{"role", "system"},
                               {"content", "Ты переводчик русского и английского языков.\n"
                                          "Ты должен предоставлять перевод слов и словосочетаний на русский язык, "
                                          "а также пример использования этого слова или словосочетания в предложении. "
                                          "Ответ должен состоять из одной строки, а формат ответа должен строго "
                                          "соответствовать следующему шаблону:\n"
                                          "\"<русский вариант 1> ; <русский вариант 2> | "
                                          "<Пример использования слова или словосочетания в предложении на английском языке>\"\n\n"
                                          "Например\nслово:\n\"fine\"\nответ:\n"
                                          "\"хорошо ; отлично | I'm fine, thank you for asking.\"\n\n"
                                          "Если запрашиваемого слова не существует, то ответ должен быть пустым. Если в слове допущена ошибка, "
                                          "то её необходимо исправить и предоставить ответ для исправленного варианта."}});
    body["messages"].push_back({{"role", "user"}, {"content", request}});
    body["temperature"] = 0.2;
    body["stream"] = false;

    auto http_request = std::make_shared<network::Request>();
    http_request->host = config_.kDefaultServer;
    http_request->port = config_.kDefaultPort;
    http_request->target = config_.kDefaultTarget;
    http_request->method = config_.kDefaultMethod;
    http_request->headers = config_.kDefaultHeaders;
    http_request->body = body.empty() ? "" : body.dump();
    http_request->callback = callback;

    return http_request;
}

void MainWindow::showError(const std::string& message)
{
    status_message_timer_ = 0;
    status_message_.clear();
    error_message_ = message;
    spdlog::error("MainWindow: {}", message);
}

void MainWindow::showStatus(const std::string& message)
{
    status_message_timer_ = 0;
    status_message_ = message;
}

// handlers

void MainWindow::onAddWordToBatch()
{
    if (auto v = vocabulary_.lock()) {
        if (!v->addUnknownWordToBatch()) {
            spdlog::warn("Cannot add word to batch");
            if (v->batchSize() >= vocabulary::Vocabulary::kMaxWordsToLearn) {
                showError("Batch is full");
            }
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onNextWord()
{
    if (auto v = vocabulary_.lock()) {
        if (word_ = v->nextWordToLearnFromBatch(); word_.has_value()) {
            try {
                card_.setWord(word_.value());
            } catch (const VocabularyError& ex) {
                showError(std::format("Failed to display word: {}", ex.what()));
            }
        } else {
            card_.setWord({});
            showError("No words available to learn");
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onKnowTheWord()
{
    if (word_.has_value()) {
        word_->get().know();
        onNextWord();
    } else {
        showError("No word");
    }
}
void MainWindow::onDontKnowTheWord()
{
    if (word_.has_value()) {
        word_->get().dontKnow();
        onNextWord();
    } else {
        showError("No word");
    }
}

void MainWindow::onLoadVocabulary()
{
    if (auto v = vocabulary_.lock()) {
        try {
            // v->importFromFile("");
            v->importFromJsonFile("");
            auto const msg{"vocabulary loaded successfully"};
            spdlog::info(msg);
            showStatus(msg);
        } catch (const std::exception& ex) {
            spdlog::error("Failed to load vocabulary: {}", ex.what());
            showError("Failed to load vocabulary");
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onSaveVocabulary()
{
    if (auto v = vocabulary_.lock()) {
        try {
            v->exportToJsonFile("");
            // v->exportToFile("");
        } catch (const std::exception& ex) {
            spdlog::error("Failed to save vocabulary: {}", ex.what());
            showError("Failed to save vocabulary");
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onAddWord()
{
    if (auto voc = vocabulary_.lock()) {
        if (input_new_word_.getText().empty()) {
            showError("Word cannot be empty");
            return;
        }
        std::string translation = input_new_word_translation_.getText();
        if (translation.empty()) {
            if (auto client = http_client_.lock()) {
                handleTranslationRequest(input_new_word_.getText());
            } else {
                showError("HTTP client is not available");
            }
        } else {
            try {
                voc->addWord(input_new_word_.getText(),
                             vocabulary::Translation::parse(translation));
                spdlog::info("Word added: {} - {}", input_new_word_.getText(),
                             translation);
                input_new_word_.setText("");
                input_new_word_translation_.setText("");
                input_new_word_example_.setText("");
            } catch (const std::exception& ex) {
                spdlog::error("Error adding word: {}", ex.what());
                showError("Failed to add word");
            }
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::handleTranslationRequest(const std::string& word)
{
    network::Request::Callback http_response_handler = [this](const std::string& response,
                                                              const std::string& error) {
        if (!error.empty()) {
            spdlog::error("HTTP error: {}", error);
            showError("Translation request failed");
            return;
        }
        try {
            auto json_response = nlohmann::json::parse(response);
            auto translation =
                json_response["choices"][0]["message"]["content"].get<std::string>();
            if (auto v = vocabulary_.lock()) {
                v->addWord(input_new_word_.getText(),
                           vocabulary::Translation::parse(translation));
                spdlog::info("Word added: {} - {}", input_new_word_.getText(),
                             translation);
                input_new_word_.setText("");
                input_new_word_translation_.setText("");
                input_new_word_example_.setText("");
            } else {
                showError("Vocabulary is not available");
            }
        } catch (const std::exception& ex) {
            spdlog::error("Error processing response: {}", ex.what());
            showError("Failed to process translation");
        }
    };
    auto request = createRequest(word, std::move(http_response_handler));
    if (auto client = http_client_.lock()) {
        client->sendRequest(request);
    } else {
        showError("HTTP client is not available");
    }
}

void MainWindow::updateWordStatisticsText()
{
    // auto const message_statistics_impressions{
    //     std::format("\nImpressions: {} (known: {}, don't known: {})",
    //                 word.knowNumber() + word.dontKnowNumber(), word.knowNumber(),
    //                 word.dontKnowNumber())};
    // text_box_ << message_statistics_impressions;
    // text_box_.setAlignment(TextBox::Alignment::kRight);

    // auto const message_statistics_retention_rate{
    //     std::format("\nRetention rate: {}%", word.retentionRate())};
    // text_box_ << message_statistics_retention_rate;
    // text_box_.setAlignment(TextBox::Alignment::kRight);
}

}  // namespace ui
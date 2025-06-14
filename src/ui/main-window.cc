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
                       std::shared_ptr<ui::tools::FontManager> font_manager)
    : RWindow(common::Config::instance().getValue<int>(kWindowWidth),
              common::Config::instance().getValue<int>(kWindowHeight),
              "Vocabulator",
              (common::Config::instance().getValue<bool>(kWindowResizable) ? FLAG_WINDOW_RESIZABLE : 0))
    , config_{common::Config::instance()}
    , font_manager_(font_manager ? font_manager
                                 : std::make_shared<ui::tools::FontManager>(
                                       config_.getValue<std::string>(kFontPath),
                                       std::vector<tools::Language>{
                                           tools::Language::kRU, tools::Language::kEN,
                                           tools::Language::kNumbers,
                                           tools::Language::kMathSymbols,
                                           tools::Language::kSpecSymbols}))
    , vocabulary_{vocabulary}
    , http_client_{http_client}
{
    spdlog::info("MainWindow initialized with screen {}x{}", config_.getValue<int>(kWindowWidth), config_.getValue<int>(kWindowHeight));

    SetTargetFPS(60);
    SetMinSize(config_.getValue<int>(kWindowWidth), config_.getValue<int>(kWindowHeight));

    calculateLayout();
    createUiElements();

    onLoadVocabulary();

    auto const& stat = vocabulary_.lock()->getStatistic();
    spdlog::info("vocabulary statistics:\nwords count: {}; known words count: {}; in progress words count: {}; new words count: {};",
        stat.words_count,
        stat.known_words_count,
        stat.in_progress_words_count,
        stat.new_words_count);
}

void MainWindow::calculateLayout()
{
    layout_.scale_factor = config_.getValue<float>(kScaleFactor);
    // ---------- layout sizes ----------

    // Margins
    layout_.window_margin = config_.getValue<float>(kWindowMargin);
    layout_.element_margin = config_.getValue<float>(kElementMargin);

    // Sizes
    layout_.button_size = RVector2{
        config_.getValue<float>(kButtonWidth),
        config_.getValue<float>(kButtonHeight)
    };
    layout_.input_size = RVector2{config_.getValue<float>(kButtonWidth), config_.getValue<float>(kButtonHeight)};
    layout_.card_size = RVector2{config_.getValue<float>(kCardWidth), config_.getValue<float>(kCardHeight)};
    layout_.text_box_vocabulary_statistics_size =
        RVector2{config_.getValue<float>(kTextBoxVocabularyStatisticsWidth), config_.getValue<float>(kTextBoxVocabularyStatisticsHeight)};
    layout_.text_box_word_statistics_size = RVector2{config_.getValue<float>(kTextBoxWordStatisticsWidth), config_.getValue<float>(kTextBoxWordStatisticsHeight)};

    // ---------- elements positions ----------

    // Load and Save Vocabulary buttons (stacked vertically, top left)
    layout_.button_load_vocabulary_pos = RVector2{
        layout_.window_margin + layout_.element_margin,
        layout_.window_margin + layout_.element_margin
    };
    layout_.button_save_vocabulary_pos = RVector2{
        layout_.button_load_vocabulary_pos.x + layout_.button_size.GetX() + layout_.element_margin,
        layout_.button_load_vocabulary_pos.y
    };

    // Vocabulary statistics box (left, below buttons)
    layout_.text_box_vocabulary_statistics_pos = RVector2{
        layout_.window_margin + layout_.element_margin,
        layout_.button_load_vocabulary_pos.y + layout_.button_size.GetY() * 1.5f + layout_.element_margin
    };

    // Add word to batch button (left, below vocabulary statistics box)
    const float y_pos_add_word_to_batch = layout_.text_box_vocabulary_statistics_pos.y + layout_.text_box_vocabulary_statistics_size.GetY() + layout_.element_margin; 
    layout_.button_add_word_to_batch_pos = RVector2{
        layout_.window_margin + layout_.element_margin,
        y_pos_add_word_to_batch
    };

    // Card (at left side of "save vocabulary" button, upper part)
    layout_.card_pos = RVector2{
        layout_.button_save_vocabulary_pos.x + layout_.button_size.GetX() + layout_.element_margin,
        layout_.window_margin + layout_.element_margin
    };

    // Word statistics box (below "card", centered below card)
    layout_.text_box_word_statistics_pos = RVector2{
        layout_.card_pos.x + layout_.card_size.GetX() / 2.0f - layout_.text_box_word_statistics_size.GetX() / 2.0f,
        layout_.card_pos.y + layout_.card_size.GetY() + layout_.element_margin
    };

    // Row of buttons (know-next-don't know): 
    //  - "don't know" (at left side and below of word statistics box)
    //  - "next" (at right side of "don't know" button)
    //  - "know" (at right side of and below of word statistics box)
    layout_.button_dont_know_the_word_pos = RVector2{
        layout_.text_box_word_statistics_pos.x,
        layout_.text_box_word_statistics_pos.y + layout_.text_box_word_statistics_size.GetY() + layout_.element_margin
    };
    layout_.button_next_word_pos = RVector2{
        layout_.button_dont_know_the_word_pos.x + layout_.button_size.GetX() + layout_.element_margin,
        layout_.button_dont_know_the_word_pos.y
    };
    layout_.button_know_the_word_pos = RVector2{
        layout_.text_box_word_statistics_pos.x + layout_.text_box_word_statistics_size.GetX() - layout_.button_size.GetX(),
        layout_.text_box_word_statistics_pos.y + layout_.text_box_word_statistics_size.GetY() + layout_.element_margin
    };

    // Bottom row (below "add word to batch" button or "don't know" button depending on which is down): 
    //  - "add word" button (at left side of the window)
    //  - three input fields (at right side of the button)
    float y_pos = layout_.button_dont_know_the_word_pos.y + layout_.button_size.GetY() + layout_.element_margin;
    if (y_pos < y_pos_add_word_to_batch) {
        y_pos = y_pos_add_word_to_batch;
    }
    layout_.button_vocabulary_add_word_pos = RVector2{
        layout_.window_margin + layout_.element_margin,
        y_pos
    };
    layout_.input_new_word_pos = RVector2{
        layout_.button_vocabulary_add_word_pos.x + layout_.button_size.GetX() + layout_.element_margin,
        layout_.button_vocabulary_add_word_pos.y
    };
    layout_.input_new_word_translation_pos = RVector2{
        layout_.input_new_word_pos.x + layout_.input_size.GetX() + layout_.element_margin,
        layout_.button_vocabulary_add_word_pos.y
    };
    layout_.input_new_word_example_pos = RVector2{
        layout_.input_new_word_translation_pos.x + layout_.input_size.GetX() + layout_.element_margin,
        layout_.button_vocabulary_add_word_pos.y
    };
}

void MainWindow::createUiElements()
{
    button_add_word_to_batch_ = std::make_unique<widgets::Button>(layout_.button_add_word_to_batch_pos, layout_.button_size,
                                ui::tools::Locale::translateInterface("add to batch"),
                                [this] { onAddWordToBatch(); }, font_manager_->getFont());
    button_next_word_ = std::make_unique<widgets::Button>(layout_.button_next_word_pos, layout_.button_size,
                        ui::tools::Locale::translateInterface("next"),
                        [this] { onNextWord(); }, font_manager_->getFont());
    button_know_the_word_ = std::make_unique<widgets::Button>(layout_.button_know_the_word_pos, layout_.button_size,
                            ui::tools::Locale::translateInterface("know"),
                            [this] { onKnowTheWord(); }, font_manager_->getFont());
    button_dont_know_the_word_ = std::make_unique<widgets::Button>(layout_.button_dont_know_the_word_pos,
                                 layout_.button_size,
                                 ui::tools::Locale::translateInterface("don't know"),
                                 [this] { onDontKnowTheWord(); },
                                 font_manager_->getFont());
    button_load_vocabulary_ = std::make_unique<widgets::Button>(
                              layout_.button_load_vocabulary_pos,
                              RVector2{layout_.button_size.GetX(), layout_.button_size.GetY() * 1.5f},
                              ui::tools::Locale::translateInterface("Load\nvocabulary"),
                              [this] { onLoadVocabulary(); }, font_manager_->getFont());
    button_save_vocabulary_ = std::make_unique<widgets::Button>(
                              layout_.button_save_vocabulary_pos,
                              RVector2{layout_.button_size.GetX(), layout_.button_size.GetY() * 1.5f},
                              ui::tools::Locale::translateInterface("Save\nvocabulary"),
                              [this] { onSaveVocabulary(); }, font_manager_->getFont());
    button_vocabulary_add_word_ = std::make_unique<widgets::Button>(layout_.button_vocabulary_add_word_pos,
                                  layout_.button_size,
                                  ui::tools::Locale::translateInterface("add word"),
                                  [this] { onAddWord(); }, font_manager_->getFont());
    card_ = std::make_unique<widgets::Card>(layout_.card_pos, layout_.card_size, vocabulary::Word(), font_manager_->getFont(config_.getValue<int>(kCardFontSize)));
    input_new_word_ = std::make_unique<widgets::TextInput>(layout_.input_new_word_pos, layout_.input_size,
                      font_manager_->getFont());
    input_new_word_translation_ = std::make_unique<widgets::TextInput>(layout_.input_new_word_translation_pos,
                                  layout_.input_size, font_manager_->getFont());
    input_new_word_example_ = std::make_unique<widgets::TextInput>(
        layout_.input_new_word_example_pos,
        RVector2{layout_.input_size.GetX() * 3.0f, layout_.input_size.GetY()},
        font_manager_->getFont());
    text_box_word_statistics_ = std::make_unique<widgets::TextBox>(
          layout_.text_box_word_statistics_pos, layout_.text_box_word_statistics_size,
          font_manager_->getFont(config_.getValue<int>(kTextBoxWordStatisticsFontSize)));
    text_box_vocabulary_statistics_ = std::make_unique<widgets::TextBox>(
          layout_.text_box_vocabulary_statistics_pos, layout_.text_box_word_statistics_size,
          font_manager_->getFont(config_.getValue<int>(kTextBoxVocabularyStatisticsFontSize)));
}

void MainWindow::updateUiElementsLayout()
{
    button_add_word_to_batch_->SetPosition(layout_.button_add_word_to_batch_pos);
    button_next_word_->SetPosition(layout_.button_next_word_pos);
    button_know_the_word_->SetPosition(layout_.button_know_the_word_pos);
    button_dont_know_the_word_->SetPosition(layout_.button_dont_know_the_word_pos);
    button_load_vocabulary_->SetPosition(layout_.button_load_vocabulary_pos);
    button_save_vocabulary_->SetPosition(layout_.button_save_vocabulary_pos);
    button_vocabulary_add_word_->SetPosition(layout_.button_vocabulary_add_word_pos);
    card_->SetPosition(layout_.card_pos);
    input_new_word_->SetPosition(layout_.input_new_word_pos);
    input_new_word_translation_->SetPosition(layout_.input_new_word_translation_pos);
    input_new_word_example_->SetPosition(layout_.input_new_word_example_pos);
    text_box_word_statistics_->SetPosition(layout_.text_box_word_statistics_pos);
    text_box_vocabulary_statistics_->SetPosition(layout_.text_box_vocabulary_statistics_pos);
}

void MainWindow::draw()
{
    BeginDrawing();

    ClearBackground(RColor{config_.getValue<uint>(kWindowBackgroundColor)});

    button_add_word_to_batch_->draw();
    button_next_word_->draw();
    button_load_vocabulary_->draw();
    button_save_vocabulary_->draw();
    button_vocabulary_add_word_->draw();
    card_->draw();
    button_know_the_word_->draw();
    button_dont_know_the_word_->draw();
    input_new_word_->draw();
    input_new_word_translation_->draw();
    input_new_word_example_->draw();
    text_box_word_statistics_->draw();
    text_box_vocabulary_statistics_->draw();

    if (!error_message_.empty()) {
        ::DrawText(error_message_.c_str(), 10, config_.getValue<int>(kWindowHeight) - 20, 16, RColor::Red());
    }
    else if (!status_message_.empty()) {
        ::DrawText(status_message_.c_str(), 10, config_.getValue<int>(kWindowHeight) - 20, 16, RColor::Green());
    }

    // if constexpr (0/* kDebugBuild */) {
    //     RWindow::DrawFPS(10, 10);
    // }

    EndDrawing();
}

void MainWindow::update(float dt)
{
    // reload config by key combination (Ctrl + R)
    if (IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_CONTROL)) {
        config_.loadFromFile();
        calculateLayout();
        updateUiElementsLayout();
    }

    if (IsWindowResized()) {
        calculateLayout();
        updateUiElementsLayout();
    }

    button_add_word_to_batch_->update(dt);
    button_next_word_->update(dt);
    button_know_the_word_->update(dt);
    button_dont_know_the_word_->update(dt);
    button_load_vocabulary_->update(dt);
    button_save_vocabulary_->update(dt);
    button_vocabulary_add_word_->update(dt);
    input_new_word_->update(dt);
    input_new_word_translation_->update(dt);
    input_new_word_example_->update(dt);

    if (!status_message_.empty() || !error_message_.empty()) {
        status_message_timer_ += dt;
        if (status_message_timer_ >= config_.getValue<int>(kStatusMessageTimer)) {
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
    http_request->host = config_.getValue<std::string>(kDefaultServer);
    http_request->port = config_.getValue<std::string>(kDefaultPort);
    http_request->target = config_.getValue<std::string>(kDefaultTarget);
    http_request->method = config_.getValue<std::string>(kDefaultMethod);
    http_request->headers = kDefaultHeaders;
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
        else {
            onNextWord();
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onNextWord()
{
    if (auto v = vocabulary_.lock()) {
        word_ = v->nextWordToLearnFromBatch();
        if (auto word = word_.lock()) {
            updateWordStatisticsText();
            updateVocabularyStatisticsText();
            try {
                card_->setWord(*word);
            } catch (const VocabularyError& ex) {
                showError(std::format("Failed to display word: {}", ex.what()));
            }
        } else {
            card_->setWord({});
            showError("No words available to learn");
        }
    } else {
        showError("Vocabulary is not available");
    }
}

void MainWindow::onKnowTheWord()
{
    if (!word_.expired()) {
        word_.lock()->know();
        onNextWord();
    } else {
        showError("No word");
    }
}

void MainWindow::onDontKnowTheWord()
{
    if (!word_.expired()) {
        word_.lock()->dontKnow();
        onNextWord();
    } else {
        showError("No word");
    }
}

void MainWindow::onLoadVocabulary()
{
    if (auto v = vocabulary_.lock()) {
        try {
            // v->importFromFile(config_.kVocabularyPathMd);
            v->importFromJsonFile(config_.getValue<std::string>(kVocabularyPathJson));
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
        auto const& stat = v->getStatistic();
        spdlog::info("vocabulary statistics upon saving:\nwords count: {}; known words count: {}; in progress words count: {}; new words count: {};",
            stat.words_count,
            stat.known_words_count,
            stat.in_progress_words_count,
            stat.new_words_count);

        try {
            v->exportToJsonFile(config_.getValue<std::string>(kVocabularyPathJson));
            // v->exportToFile(config_.kVocabularyPathMd);
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
        if (input_new_word_->getText().empty()) {
            showError("Word cannot be empty");
            return;
        }
        std::string translation = input_new_word_translation_->getText();
        if (translation.empty()) {
            if (auto client = http_client_.lock()) {
                handleTranslationRequest(input_new_word_->getText());
            } else {
                showError("HTTP client is not available");
            }
        } else {
            try {
                voc->addWord(input_new_word_->getText(),
                             vocabulary::Translation::parse(translation));
                spdlog::info("Word added: {} - {}", input_new_word_->getText(),
                             translation);
                input_new_word_->setText("");
                input_new_word_translation_->setText("");
                input_new_word_example_->setText("");
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
                v->addWord(input_new_word_->getText(),
                           vocabulary::Translation::parse(translation));
                spdlog::info("Word added: {} - {}", input_new_word_->getText(),
                             translation);
                input_new_word_->setText("");
                input_new_word_translation_->setText("");
                input_new_word_example_->setText("");
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
    if (word_.expired()) {
        return;
    }

    text_box_word_statistics_->clear();

    auto word = word_.lock();

    auto const message_statistics_impressions{
        std::format("Word statistics: known - {}, don't known - {}",
                    word->knowNumber(), word->dontKnowNumber())};
    *text_box_word_statistics_ << message_statistics_impressions;
    text_box_word_statistics_->setAlignment(widgets::TextBox::Alignment::kRight);

    if (auto voc = vocabulary_.lock()) {
        auto const message_statistics_retention_rate{
            std::format("\nRetention rate: {} (target rate - {})", word->retentionRate(), voc->targetRetentionRate())};
        *text_box_word_statistics_ << message_statistics_retention_rate;
        text_box_word_statistics_->setAlignment(widgets::TextBox::Alignment::kRight);
    } 
    else {
        spdlog::error("Vocabulary is not available");
    }

    spdlog::trace("word \'{}\' statistics:{}",
                  word->word(), message_statistics_impressions);
}

void MainWindow::updateVocabularyStatisticsText()
{
    if (vocabulary_.expired()) {
        return;
    }

    text_box_vocabulary_statistics_->clear();

    auto vocabulary = vocabulary_.lock();

    auto const message_statistics{
        std::format("\nVocabulary statistic:\n"
                   "words in vocabulary - {}\n"
                   "in progress - {}\n"
                   "to learn - {}\n"
                   "have been learned - {}", 
                   vocabulary->getStatistic().words_count,
                   vocabulary->getStatistic().in_progress_words_count,
                   vocabulary->getStatistic().new_words_count,
                   vocabulary->getStatistic().known_words_count)};
    *text_box_vocabulary_statistics_ << message_statistics;
    text_box_vocabulary_statistics_->setAlignment(widgets::TextBox::Alignment::kLeft);

    spdlog::trace("vocabulary statistics: {}", message_statistics);
}

}  // namespace ui
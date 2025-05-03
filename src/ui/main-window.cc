#include "main-window.h"

#include <array>
#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "ui/tools/text.h"
#include "vocabulary/vocabulary.h"
#include "vocabulary/translation.h"
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "network/http/client/nttp_client.h"

namespace ui {

using namespace std::literals;
using tools::Locale;

MainWindow::MainWindow(std::weak_ptr<vocabulary::Vocabulary> vocabulary, std::weak_ptr<network::HttpClient> http_client)
    : RWindow(kScreenWidth, kScreenHeight, "Vocabulator")
    , vocabulary_{vocabulary}
    , http_client_{http_client}
    , button_add_word_to_batch_{
                        {650 - kButtonWidth - kElementMargin, kScreenMargin},
                        {kButtonWidth, kButtonHeight},
                        Locale::translateInterface("add to batch"),
                        [this] {
                            if (auto v = vocabulary_.lock(); v) {
                                if (!v->addUnknownWordToBatch()) {
                                    spdlog::warn("can not add word to batch");
                                    if (v->batchSize() >= vocabulary::Vocabulary::kMaxWordsToLearn) {
                                        spdlog::warn("batch is full");
                                    }
                                }
                            } else {
                                spdlog::error("vocabulary is not available (destroyed)");
                            }
                        },
                        tools::createFont(font_file_path_,
                                          Locale::getAlphabet(char_set_))}
    , button_next_word_{
                        {650, kScreenMargin},
                        {kButtonWidth, kButtonHeight},
                        Locale::translateInterface("next"),
                        [this] {
                            if (auto v = vocabulary_.lock(); v) {
                                if (auto word = v->wordToLearnFromBatch(); word) {
                                    try {
                                        card_.setWord(word->get());
                                    } catch (VocabularyError const& ex) {
                                        spdlog::error(ex.what());
                                    }
                                }
                                else {
                                    spdlog::warn("no words to learn");
                                }
                            }
                            else {
                                spdlog::error("vocabulary is not available (destroyed)");
                            }
                        },
                        tools::createFont(font_file_path_,
                                          Locale::getAlphabet(char_set_))}
    , button_load_vocabulary_{{kScreenMargin, kScreenMargin},
                              {kButtonWidth, kButtonHeight},
                              Locale::translateInterface("load vocabulary"),
                              [this] {
                                    if (auto v = vocabulary_.lock(); v) {
                                        v->importFromJsonFile("");
                                    } else {
                                        spdlog::critical(
                                            "vocabulary is not available (destroyed)");
                                    }
                              },
                              tools::createFont(font_file_path_,
                                                Locale::getAlphabet(char_set_))}
    , button_save_vocabulary_{{kScreenMargin * 2 + kButtonWidth, kScreenMargin},
                              {kButtonWidth, kButtonHeight},
                              Locale::translateInterface("save vocabulary"),
                              [this] {
                                    if (auto v = vocabulary_.lock(); v) {
                                        v->exportToJsonFile("");
                                    } else {
                                        spdlog::critical(
                                            "vocabulary is not available (destroyed)");
                                    }
                              },
                              tools::createFont(font_file_path_,
                                                Locale::getAlphabet(char_set_))}
    , button_vocabulary_add_word_{{kScreenMargin * 3 + kButtonWidth * 2, kScreenMargin},
                                  {kButtonWidth, kButtonHeight},
                                  Locale::translateInterface("add word"),
                                  [this] {
                                      if (auto voc = vocabulary_.lock(); voc) {
                                            if (new_word_input_.getText().empty()) {
                                                spdlog::error("Word is empty");
                                                return;
                                            }
                                            std::string translation{};
                                            if (new_word_translation_input_.getText().empty()) {
                                                if (auto client = http_client_.lock(); client) {
                                                    auto response_promise{std::promise<void>()};
                                                    auto response_received{response_promise.get_future()};
                                                    network::Request::Callback&& http_response_handler =
                                                        [this, &translation, &response_promise](const std::string& response,
                                                            const std::string& error) {
                                                            if (!error.empty()) {
                                                                spdlog::error("Ошибка: {}", error);
                                                            }
                                                            else {
                                                                spdlog::info("response:\n{}", response);
                                                                auto json_response = nlohmann::json::parse(response);
                                                                translation = json_response["choices"][0]["message"]["content"];
                                                            }
                                                            response_promise.set_value();
                                                        };
                                                    auto request = createRequest(new_word_input_.getText(),
                                                                            std::move(http_response_handler));
                                                    client->sendRequest(request);
                                                    response_received.wait();
                                                }
                                                else {
                                                    spdlog::error("Http client is not available (or destroyed)");
                                                    return;
                                                }
                                            }
                                            else {
                                                translation = new_word_translation_input_.getText();
                                            }
                                        voc->addWord(new_word_input_.getText(), vocabulary::Translation::parse(translation));
                                        spdlog::info("Word added: {} - {}", new_word_input_.getText(), translation);
                                        new_word_input_.setText("");
                                        new_word_translation_input_.setText("");
                                        new_word_example_input_.setText("");
                                      }
                                      else {
                                            spdlog::critical("vocabulary is not available (destroyed)");
                                      }
                                  },
                                  tools::createFont(font_file_path_,
                                                    Locale::getAlphabet(char_set_))}
    , card_{{kScreenMargin, kScreenMargin * 2 + kButtonWidth},
            {kScreenWidth - kScreenMargin * 2,
             kScreenHeight - kScreenMargin * 2 - kButtonHeight},
            {},//vocabulary_.lock()->nextRandomWordToLearn(),
            tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
    , new_word_input_{
        {kScreenMargin + kElementMargin, kScreenHeight - kScreenMargin - (kElementMargin + kButtonHeight) * 3},
        {kButtonWidth, kButtonHeight},
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))/* {} */}
    , new_word_translation_input_{
        {new_word_input_.GetX(), new_word_input_.GetY() + new_word_input_.GetHeight() + kElementMargin},
        {new_word_input_.GetWidth(), new_word_input_.GetHeight()},
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
    , new_word_example_input_{
        {new_word_translation_input_.GetX(), new_word_translation_input_.GetY() + new_word_translation_input_.GetHeight() + kElementMargin},
        {new_word_translation_input_.GetWidth(), new_word_translation_input_.GetHeight()},
        tools::createFont(font_file_path_, Locale::getAlphabet(char_set_))}
{
    spdlog::info("Screen width: {}", kScreenWidth);
    spdlog::info("Screen height: {}", kScreenHeight);
    spdlog::info("Screen margin: {}", kScreenMargin);
    spdlog::info("Element margin: {}", kElementMargin);
    spdlog::info("Button height: {}", kButtonHeight);
    spdlog::info("Card height: {}", kCardHeight);
    spdlog::info("--------------------------------------------------");

    spdlog::info("new_word_input_: x({}) y({}) w({}) h({})", new_word_input_.GetX(), new_word_input_.GetY(), new_word_input_.GetWidth(), new_word_input_.GetHeight());
    spdlog::info("new_word_translation_input_: x({}) y({}) w({}) h({})", new_word_translation_input_.GetX(), new_word_translation_input_.GetY(), new_word_translation_input_.GetWidth(), new_word_translation_input_.GetHeight());
    spdlog::info("new_word_example_input_: x({}) y({}) w({}) h({})", new_word_example_input_.GetX(), new_word_example_input_.GetY(), new_word_example_input_.GetWidth(), new_word_example_input_.GetHeight());
    spdlog::info("==================================================");
}

void MainWindow::draw()
{
    button_add_word_to_batch_.draw();
    // button_previous_word_.draw();
    button_next_word_.draw();
    button_load_vocabulary_.draw();
    button_save_vocabulary_.draw();
    button_vocabulary_add_word_.draw();
    card_.draw();
    new_word_input_.draw();
    new_word_translation_input_.draw();
    new_word_example_input_.draw();

    // optionally we can draw current FPS
    RWindow::DrawFPS(10, 10);
}

void MainWindow::update(float dt) {
    button_add_word_to_batch_.update(dt);
    button_next_word_.update(dt);
    button_load_vocabulary_.update(dt);
    button_save_vocabulary_.update(dt);
    button_vocabulary_add_word_.update(dt);

    new_word_input_.update(dt);
    new_word_translation_input_.update(dt);
    new_word_example_input_.update(dt);
}

// private methods -------------------------------------------

std::shared_ptr<network::Request> MainWindow::createRequest(std::string const& request,
                               network::Request::Callback callback,
                               std::string const& server,
                               std::string const& port,
                               std::string const& target,
                               std::string const& method,
                               std::vector<std::pair<std::string, std::string>> const& headers) {
    nlohmann::json body;
    body["model"] = "gemma-3-4b-it";
    body["messages"] = nlohmann::json::array();
    body["messages"].push_back({{"role", "system"}, {"content", "Ты переводчик русского и английского языков.\nТы должен предоставлять перевод слов и словосочетаний на русский язык, а также пример использования этого слова или словосочетания в предложении. Ответ должен состоять из одной строки, а формат ответа должен строго соответствовать следующему шаблону:\n\"<русский вариант 1> ; <русский вариант 2> | <Пример использования слова или словосочетания в предложении на английском языке>\"\n\nНапример\nслово:\n\"fine\"\nответ:\n\"хорошо ; отлично | I'm fine, thank you for asking.\""}});
    body["messages"].push_back({{"role", "user"}, {"content", request}});
    body["temperature"] = 0.2;
    // "max_tokens": -1,
    body["stream"] = false;

    auto http_request = std::make_shared<network::Request>();
    http_request->host = server;
    http_request->port = port;
    http_request->target = target;
    http_request->method = method;
    http_request->headers = headers;
    http_request->body = body.empty() ? "" : body.dump();
    http_request->callback = callback;

    return http_request;
}

}  // namespace ui

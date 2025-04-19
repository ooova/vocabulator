// #include <iostream>
#include <string>
// #include <sstream>
#include <functional>
// #include <vector>
// #include <memory>
// #include <mutex>

#include "network/http/request.h"

#include "asio.hpp"
#include "spdlog/spdlog.h"

#include "tools/scoped_async_wrapper.h"
#include "nlohmann/json.hpp"

namespace network {

class HttpClient {
public:
    using Callback = std::function<void(const std::string&, const std::string&)>;

    HttpClient() = default;

    ~HttpClient() {
        work_guard_.reset();
        io_context_.stop();
    }

    void sendRequest(const std::string& host,
                     const std::string& port,
                     const std::string& target,
                     const std::string& method,
                     const std::vector<std::pair<std::string, std::string>>& headers,
                     const nlohmann::json& body,
                     Request::Callback callback)
    {
        auto request = std::make_shared<Request>();
        request->host = host;
        request->port = port;
        request->target = target;
        request->method = method;
        request->headers = headers;
        request->body = body.empty() ? std::string{""} : body.dump();
        request->callback = callback;

        asio::post(io_context_, [this, request]() { processRequest(request); });
    }

    void sendRequest(std::shared_ptr<network::Request> request) {
        // auto req = std::make_shared<Request>(request);
        asio::post(io_context_, [this, request]() { processRequest(request); });
    }

private:
    void processRequest(std::shared_ptr<Request> request) {
        resolver_.async_resolve(request->host, request->port.empty() ? "80" : request->port,
            [this, request](asio::error_code const& res_ec, asio::ip::tcp::resolver::results_type const& results) {
                if (!res_ec) {
                    asio::async_connect(socket_, results,
                        [this, request](const asio::error_code& con_ec, asio::ip::tcp::endpoint const&) {
                            if (!con_ec) {
                                sendRequestData(request);
                            } else {
                                handleError(request, "Connect failed", con_ec);
                            }
                        });
                } else {
                    handleError(request, "Resolve failed", res_ec);
                }
            });
    }

    void sendRequestData(std::shared_ptr<Request> request) {
        std::ostream request_stream{&request->request_buffer};
        request_stream << request->method << " " << request->target << " HTTP/1.1\r\n";
        request_stream << "Host: " << request->host << "\r\n";
        for (const auto& header : request->headers) {
            request_stream << header.first << ": " << header.second << "\r\n";
        }
        if (!request->body.empty()) {
            request_stream << "Content-Length: " << request->body.length() << "\r\n";
            request_stream << "Content-Type: application/json\r\n";
            request_stream << "Connection: close\r\n\r\n";
            request_stream << request->body;
        }
        request_stream << "\r\n\r\n";

        // std::stringstream ss;
        // ss << request_stream.rdbuf();
        // spdlog::trace("Request: \n{}", ss.str());
        // request_stream.seekp(0);

        asio::async_write(socket_, request->request_buffer.data(),
            [this, request](const asio::error_code& ec, std::size_t /*length*/) {
                if (!ec) {
                    readResponseHeaders(request);
                } else {
                    handleError(request, "Write failed", ec);
                }
            });
    }

    void readResponseHeaders(std::shared_ptr<Request> request) {
        asio::async_read_until(socket_, request->response_buffer, "\r\n",
            [this, request](const asio::error_code& ec, std::size_t /*length*/) {
                if (!ec) {
                    std::istream response_stream(&request->response_buffer);
                    std::string http_version;
                    response_stream >> http_version;
                    unsigned int status_code;
                    response_stream >> status_code;
                    std::string status_message;
                    std::getline(response_stream, status_message);
                    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
                        handleError(request, "Response message is empty or HTTP version starts not from \'HTTP\'", {});
                        return;
                    }
                    if (status_code != 200 ) {
                        spdlog::error("HTTP status code: {}; status message: {}", status_code, status_message);
                    }
                    asio::async_read_until(socket_, request->response_buffer, "\r\n\r\n",
                        [this, request](const asio::error_code& ec, std::size_t /*length*/) {
                                std::istream response_stream(&request->response_buffer);
                                // Read and log headers
                                std::string header;
                                std::size_t content_length = 0;
                                bool chunked = false;
                                while (std::getline(response_stream, header) && header != "\r") {
                                    spdlog::debug("Response Header: {}", header);
                                    if (auto pos = header.find("Content-Length:"); pos != std::string::npos) {
                                        content_length = std::stoi(header.substr(pos + 16));
                                    }
                                    if (header.find("Transfer-Encoding: chunked") != std::string::npos) {
                                        chunked = true;
                                    }
                                }
                                // Continue reading response body
                                readResponseBody(request, content_length, chunked);
                            }
                        );
                } else {
                    handleError(request, "Read headers failed", ec);
                }
            });
    }

    void readResponseBody(std::shared_ptr<Request> request, std::size_t content_length, bool chunked) {
        if (chunked) {
            try {
                spdlog::trace("------------------------------- readResponseBody -------------------------------");

                auto read_until_delim = [this, request](std::string const& delim = "\r\n") -> int {
                    spdlog::trace("------------------------------- read_until_delim -------------------------------");
                    int amount_of_bytes_in_chunk{0};

                    asio::error_code ec;
                    auto bytes_read = asio::read_until(socket_, request->response_buffer, delim, ec);
                    if (ec) {
                        spdlog::error("Read until char failed. Error: {}", ec.message());
                        return amount_of_bytes_in_chunk;
                    }

                    std::istream response_stream(&request->response_buffer);

                    std::string bytes_count_str;
                    std::getline(response_stream, bytes_count_str);
                    spdlog::trace("bytes_count_str: {}", bytes_count_str);

                    if (!bytes_count_str.empty()) {
                        try {
                            amount_of_bytes_in_chunk = std::stoi(bytes_count_str, nullptr, 16);
                        }
                        catch (std::exception const& ex) {
                            spdlog::error("Error while extracting amount of bytes in the chunk: {}", ex.what());
                        }
                    }
                    spdlog::trace("amount_of_bytes_in_chunk: {}", amount_of_bytes_in_chunk);
                    spdlog::trace("--------------------------------------------------------------");
                    return amount_of_bytes_in_chunk;
                };

                auto read_n_bytes = [this, request](int n, int& read_bytes_left_out) -> std::string {
                    spdlog::trace("------------------------------- read_n_bytes -------------------------------");
                    asio::error_code ec;
                    std::string buffer;

                    spdlog::trace("n = {}", n);
                    spdlog::trace("response buffer size = {}", request->response_buffer.size());

                    auto amount_of_bytes_to_read{0};
                    if (request->response_buffer.size() < n) {
                        amount_of_bytes_to_read = n - request->response_buffer.size();
                    }
                    spdlog::trace("amount_of_bytes_to_read: {}", amount_of_bytes_to_read);

                    auto bytes_read = asio::read(socket_, request->response_buffer, asio::transfer_exactly(amount_of_bytes_to_read), ec);
                    spdlog::trace("bytes_read: {}", bytes_read);

                    if (!ec || ec == asio::error::eof) {
                        std::istream response_stream(&request->response_buffer);
                        std::string response_body((std::istreambuf_iterator<char>(response_stream)),
                                                  std::istreambuf_iterator<char>());

                        spdlog::trace("content: {}", response_body);
                        spdlog::trace("--------------------------------------------------------------");
                        if (n < response_body.size()) {
                            read_bytes_left_out = 0;
                        }
                        read_bytes_left_out = n - response_body.size();
                        return response_body;
                    }

                    spdlog::trace("--------------------------------------------------------------");
                    spdlog::error("Read content failed {}", ec.message());

                    return {};
                };

                std::string response_body{};
                auto amount_of_bytes_in_chunk = read_until_delim();
                while (amount_of_bytes_in_chunk) {
                    // Original value ('amount_of_bytes_in_chunk') represents amount of "valuable" chars,
                    // but we also have to count '\r' and '\n' chars, which represents end of the chunk,
                    // so, next part of the response message (read by 'read_until_delim' method)
                    // will start with chunk size
                    response_body += read_n_bytes(amount_of_bytes_in_chunk + 2, amount_of_bytes_in_chunk);
                    if (amount_of_bytes_in_chunk == 0) {
                        amount_of_bytes_in_chunk = read_until_delim();
                    }
                }
                spdlog::trace(" chunk is finished -------------------------------");

                if (request->callback) {
                    request->callback(response_body, "");
                }
            }
            catch (asio::system_error const& ex) {
                handleError(request, "ReadResponseBody() - exception while reading ", ex.code());
            }
        }
        else if (content_length > 0) {
            asio::async_read(socket_, request->response_buffer, asio::transfer_exactly(content_length - request->response_buffer.size()),
                [this, request, content_length](const asio::error_code& ec, std::size_t /*length*/) {
                    if (!ec || ec == asio::error::eof) {
                        std::istream response_stream(&request->response_buffer);
                        std::string response_body((std::istreambuf_iterator<char>(response_stream)),
                                                  std::istreambuf_iterator<char>());
                        if (request->callback) {
                            request->callback(response_body, "");
                        }
                    } else {
                        handleError(request, "Read content failed", ec);
                    }
                });
        }
        else {
            // No Content-Length and not chunked: read until EOF
            asio::async_read(socket_, request->response_buffer, asio::transfer_at_least(1),
                [this, request](const asio::error_code& ec, std::size_t /*length*/) {
                    if (!ec) {
                        readResponseBody(request, 0, false); // Recursive call
                    } else if (ec == asio::error::eof) {
                        std::istream response_stream(&request->response_buffer);
                        std::string response_body((std::istreambuf_iterator<char>(response_stream)),
                                                  std::istreambuf_iterator<char>());
                        if (request->callback) {
                            request->callback(response_body, "");
                        }
                    } else {
                        handleError(request, "Read content failed", ec);
                    }
                });
        }
    }

    void handleError(std::shared_ptr<Request> request, const std::string& message, const asio::error_code& ec) {
        if (request->callback) {
            request->callback("", message + "; " + ec.message());
        }
        // Optionally, you can close the socket on error
        // asio::post(io_context_, [this]() { socket_.lowest_layer().close(); });
    }

private:
        // : resolver_{io_context_}
        // , socket_{io_context_}
        // , work_guard_{asio::make_work_guard(io_context_)}
        // , io_thread_{std::make_unique<tools::AsyncWrapper>([this] { io_context_.run(); })}
    asio::io_context io_context_{};
    asio::ip::tcp::resolver resolver_{io_context_};
    asio::ip::tcp::socket socket_{io_context_};
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_{asio::make_work_guard(io_context_)};
    std::unique_ptr<tools::AsyncWrapper> io_thread_{std::make_unique<tools::AsyncWrapper>([this] { io_context_.run(); })};
    // std::mutex mutex_;
};

} // namespace network
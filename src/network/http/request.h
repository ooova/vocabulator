#ifndef NETWORK_HTTP_REQUEST_H
#define NETWORK_HTTP_REQUEST_H

#include <string>
#include <vector>
#include <utility>
#include <functional>

#include "asio.hpp"

namespace network {

struct Request {
    using Callback = std::function<void(const std::string&, const std::string&)>;

    std::string host;
    std::string port;
    std::string target;
    std::string method;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    Callback callback;
    asio::streambuf request_buffer;
    asio::streambuf response_buffer;
};

} // namespace network

#endif // NETWORK_HTTP_REQUEST_H

#ifndef COMMON_EXCEPTIONS_CONFIG_ERROR_H
#define COMMON_EXCEPTIONS_CONFIG_ERROR_H

#include <stdexcept>
#include <string>

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg)
        : std::runtime_error(msg)
    {}

    explicit ConfigError(const char* msg)
        : std::runtime_error(msg)
    {}
};

#endif // COMMON_EXCEPTIONS_CONFIG_ERROR_H
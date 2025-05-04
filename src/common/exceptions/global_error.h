#ifndef COMMON_EXCEPTIONS_GLOBAL_ERROR_H
#define COMMON_EXCEPTIONS_GLOBAL_ERROR_H

#include <stdexcept>
#include <string>

class GlobalError : public std::runtime_error {
public:
    GlobalError(std::string const& msg)
        : std::runtime_error(msg)
    {}
};

#endif  // COMMON_EXCEPTIONS_GLOBAL_ERROR_H

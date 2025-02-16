#ifndef COMMON_EXCEPTIONS_PARSING_ERROR_H
#define COMMON_EXCEPTIONS_PARSING_ERROR_H

#include <stdexcept>
#include <string>

class ParsingError : public std::runtime_error {
public:
    ParsingError(std::string const& msg)
        : std::runtime_error(msg)
    {}
};

#endif  // COMMON_EXCEPTIONS_PARSING_ERROR_H
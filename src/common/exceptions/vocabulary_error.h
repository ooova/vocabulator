#ifndef COMMON_EXCEPTIONS_VOCABULARY_ERROR_H
#define COMMON_EXCEPTIONS_VOCABULARY_ERROR_H

#include <stdexcept>
#include <string>

class VocabularyError : std::runtime_error {
public:
    VocabularyError(std::string const& msg)
        : std::runtime_error(msg)
    {}
};

#endif  // COMMON_EXCEPTIONS_VOCABULARY_ERROR_H

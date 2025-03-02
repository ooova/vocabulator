#ifndef TOOLS_RANDOM_NUMBER_H
#define TOOLS_RANDOM_NUMBER_H

#include <random>

#include "spdlog/spdlog.h"

namespace tools {

int getRandomNumber(int min, int max)
{
    static auto seed{std::random_device()()};
    static std::mt19937 rng(seed);
    std::uniform_int_distribution</* std::mt19937::result_type */> dist(min, max);

    auto const result{dist(rng)};
    spdlog::info("next random number: {}", result);
    return result;
}

} // namespace tools

#endif // TOOLS_RANDOM_NUMBER_H

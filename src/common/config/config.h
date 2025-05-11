#ifndef COMMON_CONFIG_H

#define COMMON_CONFIG_H

#include <map>
#include <string>

namespace common {

enum class ConfigId {

    kScreenWidth,

};

template <typename ValueT>

struct ConfigValue {
    ConfigId id;

    std::string name;

    ValueT defaultValue;

    ValueT value;
};

class Config {
private:
};

}  // namespace common

#endif  // COMMON_CONFIG_H

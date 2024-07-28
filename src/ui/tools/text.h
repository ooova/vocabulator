#ifndef UI_TOOLS_TEXT_H
#define UI_TOOLS_TEXT_H

#include "raylib-cpp.hpp"

#include <filesystem>
#include <string_view>

namespace ui::tools {

RFont createFont(std::filesystem::path font_file_path, std::string_view char_set, int const font_size = 36)
{
    auto codepointsCount{0};
    auto codepoints =
        ::LoadCodepoints(char_set.data(), &codepointsCount);
    auto font{
        RFont(std::string(font_file_path), font_size, codepoints, codepointsCount)};
    ::UnloadCodepoints(codepoints);
    return font;
}

}

#endif // UI_TOOLS_TEXT_H

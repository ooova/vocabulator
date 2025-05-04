#ifndef UI_TOOLS_TEXT_H
#define UI_TOOLS_TEXT_H

#include "ui/tools/locale.h"

#include "raylib-cpp.hpp"

#include <filesystem>
#include <string_view>
#include <vector>
#include <string>

namespace ui::tools {

class FontManager {
public:
    FontManager(std::filesystem::path const& font_path,
                std::vector<Language> const& char_sets,
                int const font_size = 36);
    RFont getFont() const;

private:
    std::filesystem::path font_path_;
    std::vector<Language> char_sets_;
    int font_size_;
};

} // ui::tools

#endif // UI_TOOLS_TEXT_H

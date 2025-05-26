#include "font-manager.h"

#include "common/exceptions/global_error.h"

#include "spdlog/spdlog.h"

namespace ui::tools {

FontManager::FontManager(std::filesystem::path const& font_path,
                         std::vector<tools::Language> const& char_sets,
                         int const font_size)
    : font_path_{font_path}
    , char_sets_{char_sets}
    , font_size_{font_size}
{
}

RFont FontManager::getFont(int const font_size) const
{
    auto const size = font_size > 0 ? font_size : font_size_;
    RFont font{};
    if (font_path_.empty()) {
        spdlog::error("Font path is empty, default font is loaded");
    }

    auto codepointsCount{0};
    auto codepoints = ::LoadCodepoints(Locale::getAlphabet(char_sets_).c_str(), &codepointsCount);

    try {
        font = std::move(RFont(font_path_.generic_string(), size, codepoints, codepointsCount));
    }
    catch (raylib::RaylibException const& ex) {
        spdlog::error("Font loading from \"{}\" failed: {}", font_path_.string(), ex.what());
    }

    return font;
}

} // ui::tools

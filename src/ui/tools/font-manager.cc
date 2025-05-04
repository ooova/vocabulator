#include "font-manager.h"

#include "common/exceptions/global_error.h"

#include <format>

namespace ui::tools {

FontManager::FontManager(std::filesystem::path const& font_path,
                         std::vector<tools::Language> const& char_sets,
                         int const font_size)
    : font_path_{font_path}
    , char_sets_{char_sets}
    , font_size_{font_size}
{
}

RFont FontManager::getFont() const
{
    auto codepointsCount{0};
    auto codepoints = ::LoadCodepoints(Locale::getAlphabet(char_sets_).c_str(), &codepointsCount);
    auto font{RFont(font_path_.c_str(), font_size_, codepoints, codepointsCount)};
    ::UnloadCodepoints(codepoints);

    if (!font.GetTexture().id) {
        throw GlobalError(std::format("Font loading from \"{}\" failed", font_path_.string()));
    }

    return font;
}

} // ui::tools

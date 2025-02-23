project('vocabulator', 'cpp')

cpp_options = ['-std=c++23']

inc_dirs = include_directories(
        './',
        './src',
        './json/include',
        './raylib/src',
        './raylib-cpp/include',
        './spdlog/include',
    )

src = []
subdir('src')

static_libs = ['/storage/ova/projects/my/vocabulator/with-raylib-cpp/raylib/src/libraylib.a']
# lib_xkbcommon = dependencies('xkbcommon')

executable(
        'vocabulator',
        src,
        link_args : static_libs,
        include_directories: inc_dirs,
        cpp_args : cpp_options,
        # dependencies: lib_xkbcommon
    )

# mylib_dep = declare_dependency(
#   link_args: ['/path/to/libmylib.a'],
#   include_directories: include_directories('/path/to/include')
# )

# executable('my_program', 'main.c', dependencies: [mylib_dep])
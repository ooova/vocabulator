project('vocabulator', 'cpp')

cpp_options = ['-std=c++23', '-O']

inc_dirs = include_directories(
        './',
        './src',
        './json/include',
        './raylib/src',
        './raylib-cpp/include',
        './spdlog/include',
        './asio/asio/include'
    )

src = []
subdir('src')

static_libs = ['/storage/ova/projects/my/vocabulator/with-raylib-cpp/raylib/src/libraylib.a']
x11_dep = dependency('x11', required: true, method: 'pkg-config')
gl_dep =  dependency('GL')
dl_dep = dependency('dl')
# thread = dependency('pthread', required: true, method: 'pkg-config')
lib_openssl = dependency('openssl')

executable(
        'vocabulator',
        src,
        link_args : static_libs,
        include_directories: inc_dirs,
        cpp_args : cpp_options,
        dependencies: [ x11_dep, gl_dep, dl_dep, lib_openssl] #, thread
    )

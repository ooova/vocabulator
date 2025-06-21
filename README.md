## What is it?

This is just a pet project for fun. I tried some libraries and, in parallel, created a simple app for memorizing foreign words. The app can be used in combination with AI models to add new words to the vocabulary.

## How to build?

You'll have to install `meson` build system and some libraries mentioned in the `meson.build` file (x11, GL, openssl). Then run the following commands form the root folder of the project
```shell
meson setup build
meson compile -C build
./build/vocabulator
```

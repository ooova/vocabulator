# CPP_FLAGS = -std=c++20 -Wextra -Wall -Werror

# INC_DIRS = -I./src -I./raylib-cpp/include -I./spdlog/include -I/usr/local/include

# LINC_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lfmt

# SRC = src/main.cc \
#     src/tools/string_utils.cc \
#     src/ui/main-window.cc \
#     src/ui/widgets/card.cc \
#     src/vocabulary/vocabulary.cc \
#     src/vocabulary/word.cc \
#     src/vocabulary/translation.cc

# OBJS = $(SRC:.cc=.o)


# # all:
# # 	g++ $(CPP_FLAGS) -o build/vocabulator src/main.cc build/ui.o build/tools.o build/vocabulary.o $(INC_DIRS) $(LINC_LIBS)

# # ui: #build/ui.o
# # 	g++ $(CPP_FLAGS) src/ui/main-window.cc src/ui/widgets/card.cc -o build/ui.o $(INC_DIRS) $(LINC_LIBS)

# # tools: #build/tools.o
# # 	g++ $(CPP_FLAGS) src/tools/string_utils.cc -o build/tools.o $(INC_DIRS) $(LINC_LIBS)

# # vocabulary: #build/vocabulary.o
# # 	g++ $(CPP_FLAGS) src/vocabulary/vocabulary.cc src/vocabulary/word.cc src/vocabulary/translation.cc -o build/vocabulary.o $(INC_DIRS) $(LINC_LIBS)

# # all:
# # 	g++ $(CPP_FLAGS) -o build/vocabulator $(SRC) $(INC_DIRS) $(LINC_LIBS)

# all: $(OBJS)
#  	g++ $(CPP_FLAGS) -o build/vocabulator $(OBJS)

# %.o: %.c
# 	g++ $(CPP_FLAGS) -c $< -o $@ $(INC_DIRS) $(LINC_LIBS)

# clean:
# 	rm ./build/*

# # .PHONY: all ui tools vocabulary clean

PROJECT = vocabulator

CPP_FLAGS = -std=c++20 -Wextra -Wall -Werror

INC_DIRS = -I./src -I./raylib-cpp/include -I./spdlog/include -I/usr/local/include

LINC_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lfmt

SRC = src/main.cc \
    src/tools/string_utils.cc \
    src/ui/main-window.cc \
    src/ui/widgets/card.cc \
    src/vocabulary/vocabulary.cc \
    src/vocabulary/word.cc \
    src/vocabulary/translation.cc

OBJS = $(SRC:.cc=.o)

all: vocabulator

vocabulator: $(OBJS)
	mkdir -p build
	g++ $(CPP_FLAGS) -o build/vocabulator $(OBJS) $(INC_DIRS) $(LINC_LIBS)

%.o: %.cc
	g++ $(CPP_FLAGS) -c $< -o $@ $(INC_DIRS)

clean:
	rm -f $(OBJS) build/vocabulator

.PHONY: all clean

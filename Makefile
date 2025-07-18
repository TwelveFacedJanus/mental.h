PROJECT_NAME = Mental

CC = clang
CFLAGS = -Wall -Wextra -std=c11 -g
CPPFLAGS = -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib
LIBS = -lglfw -lGLEW -framework OpenGL -framework Cocoa -framework IOKit

SOURCE_FILES = main.c

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

run:
	Mental.dSYM/Contents/Resources/DWARF/Mental

clean:
	rm -f $(PROJECT_NAME)

.PHONY: all clean
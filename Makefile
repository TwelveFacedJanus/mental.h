PROJECT_NAME = mental_h
CC = clang
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lglfw -lGLEW -luv -L/opt/homebrew/lib -I/opt/homebrew/include -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
DEFINES =  -DDEBUG=1 -DMENTAL_INCLUDE_GLFW
INCLUDES = 
SOURCES = main.c core/Mathematica.c core/Historical.c graphics/wm/wm.c core/Pipes.c graphics/render/render.c
OUTPUT = Binary/mentalGraphics

mentalGraphics: $(SOURCES)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) $(SOURCES) $(LDFLAGS) -o $(OUTPUT)

.PHONY: mentalGraphics clean

clean:
	rm -f Binary/mentalGraphics

PROJECT_NAME = mental_h
CC = clang
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lglfw -lGLEW -luv -lcglm -L/opt/homebrew/lib -I/opt/homebrew/include -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
DEFINES =  -DDEBUG=1 -DMENTAL_INCLUDE_GLFW
INCLUDES = 
SOURCES = main.c engine/engine.c engine/component.c engine/clouds.c engine/historical.c engine/project.c engine/wm.c engine/skybox.c engine/perlin.c engine/model3d.c
OUTPUT = Binary/mentalGraphics

mentalGraphics: $(SOURCES)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) $(SOURCES) $(LDFLAGS) -o $(OUTPUT)

.PHONY: mentalGraphics clean

clean:
	rm -f Binary/mentalGraphics

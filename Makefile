CC=gcc
CFLAGS=-O2 -std=c11 -Wall -Wextra -Isrc
LIBS=`pkg-config --libs glfw3` -lGLEW -lGL -ldl -lm
CFLAGS+=$(shell pkg-config --cflags glfw3)

SRC=src/main.c src/gl_util.c src/math3d.c src/camera.c src/mesh.c src/noise.c src/fbo.c

all: waterland

waterland: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS)

clean:
	rm -f waterland

.PHONY: all clean

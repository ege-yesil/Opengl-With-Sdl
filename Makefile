CC = gcc

files := src/main.c src/shader.c src/camera.c src/util/string.c src/util/vector.c src/util/hashMap.c src/object.c
flags := glad/libglad.a -Icglm/ -Iglad/include -lm -lSDL2 -lGL

debugMem:
	$(CC) -g $(files) $(flags) -fsanitize=address -o sdl_gl

debug:
	$(CC) -g $(files) $(flags) -o sdl_gl

release:
	$(CC) $(files) $(flags) -Wall -Werror -O3 -o sdl_gl

clean:
	rm -f sdl_gl


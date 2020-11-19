OS := $(shell uname)

ifeq ($(OS), Linux)
	CC = g++
	LIBS = -L/home/marcinrogala/Pulpit/inz/glfw-3.3.2/build/src -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread
	#-lglfw -lGL -lm -lX11 -lpthread -lXi -lXrandr -ldl
	CFLAGS = -g -Wall -std=c++17 -Wno-deprecated
else
	CC = x86_64-w64-mingw32-g++
#CC = i686-w64-mingw32-g++
#CC = i686-pc-mingw32-g++
	LIBS = -lglfw3 -lopengl32 -lgdi32
#LIBS = -l:glfw3dll.a -lopengl32 -lgdi32
	CFLAGS = -g -Wall -Wno-deprecated
endif

all:
	clang++ -g -w -Wall -std=c++11 *.cpp -o main -lglfw -lGLU -lGL -lm -lglut

clean:
	rm -rf *.o main

run:
	./main

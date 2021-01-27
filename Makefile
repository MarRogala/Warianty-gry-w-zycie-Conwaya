all:
	clang++ -stdlib=libc++ -O3 -g -w -Wall -std=c++17 source/*.cpp -o main -lglfw -lGLU -lGL -lm -lglut -lGLEW

clean:
	rm -rf *.o main

run:
	./main $(FILE)

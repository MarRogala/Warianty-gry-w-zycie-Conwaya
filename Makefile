all:
	clang++ -stdlib=libc++ -g -w -Wall -std=c++17 *.cpp -o main -lglfw -lGLU -lGL -lm -lglut -lGLEW

clean:
	rm -rf *.o main

run:
	./main $(FILE)

main:src/main.cpp
	g++ -g src/main.cpp -o renderer.o -I include/ -std=c++17 -pthread
	./renderer.o > output.ppm
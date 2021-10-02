main:src/main.cpp
	g++ -g src/main.cpp -o renderer.o -I include/
	./renderer.o > output.ppm
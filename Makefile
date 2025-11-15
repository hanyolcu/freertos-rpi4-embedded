program: build/main.o build/ethernet.o
	gcc -o build/program build/main.o build/ethernet.o

build/main.o: src/main.c
	gcc -c src/main.c -o build/main.o

build/ethernet.o: src/ethernet.c src/ethernet.h
	gcc -c src/ethernet.c -o build/ethernet.o

run: program
	sudo ./build/program

clean:
	rm -f build/*.o build/program

.PHONY: run clean
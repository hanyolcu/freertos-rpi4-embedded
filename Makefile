program: build/main.o build/ethernet.o build/uart.o
	gcc -pthread -o build/program build/main.o build/ethernet.o build/uart.o

build/main.o: src/main.c
	gcc -pthread -c src/main.c -o build/main.o

build/ethernet.o: src/ethernet.c src/ethernet.h
	gcc -pthread -c src/ethernet.c -o build/ethernet.o

build/uart.o: src/uart.c src/uart.h
	gcc -pthread -c src/uart.c -o build/uart.o

run: program
	sudo ./build/program

clean:
	rm -f build/*.o build/program

.PHONY: run clean program program
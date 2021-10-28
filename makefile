create:
	gcc -o main decorder.cpp emulator.cpp main.cpp

run:
	./main

clean:
	rm *.o
	rm *.txt
	rm *.s
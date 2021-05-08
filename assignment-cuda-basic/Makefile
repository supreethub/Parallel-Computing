CFLAGS = -O3 -Wall -Wexta -Wpedantic


main: main.cu
	./Clean.sh
	./Compile.sh
	./Run.sh

.PHONY: run clean

compile: main.cu
	./Compile.sh

run:
	./Run.sh

queue:
	./Queue.sh

clean:
	./Clean.sh

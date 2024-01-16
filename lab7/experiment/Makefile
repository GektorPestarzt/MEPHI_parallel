CC = mpicc

BACK_DIR = ./
MAIN = $(BACK_DIR)main.c

BACK := $(shell find ${BACK_DIR} -name "*.c" | sed -e 's/\.\///')

LDFLAGS = $(shell pkg-config --cflags)
OMPFLAGS = -fopenmp

all: parallel clean

parallel: $(BACK)
	@$(CC) $^ $(LDFLAGS) $(OMPFLAGS) -lm -o parallel.out
	@mpirun -np 4 ./parallel.out 0 50000000

clean:
	@rm -rf *.o *.out

codestyle:
	clang-format -i --style=google $(BACK_DIR)/*.c
	clang-format -i --style=google $(BACK_DIR)/*.h
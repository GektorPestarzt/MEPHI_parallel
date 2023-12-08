CC = mpicc

BACK_DIR = ./
MAIN = $(BACK_DIR)main.c

BACK := $(shell find ${BACK_DIR} -name "*.c" | sed -e 's/\.\///')

CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = $(shell pkg-config --cflags)

STAT_DIR = ../stat_files/

all: parallel clean

parallel: $(BACK)
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(OMPFLAGS) -o parallel.out
	@mpiexec -n 4 ./parallel.out

clean:
	@rm -rf *.o *.out

codestyle:
	clang-format -i --style=google $(BACK_DIR)/*.c
	clang-format -i --style=google $(BACK_DIR)/*.h
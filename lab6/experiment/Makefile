CC = gcc

BACK_DIR = back/
MAIN = $(BACK_DIR)main.c

BACK := $(shell find ${BACK_DIR} -name "*.c" | sed -e 's/\.\///')

# CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = $(shell pkg-config --cflags)
OMPFLAGS = -fopenmp

STAT_DIR = ../stat_files/

all: parallel clean

linear: $(BACK)
	@$(CC) $^ $(LDFLAGS) -o linear.out
	@mkdir -p $(STAT_DIR)
	@./linear.out $(STAT_DIR)

parallel: $(BACK)
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(OMPFLAGS) -o parallel.out
	@mkdir -p $(STAT_DIR)
	@./parallel.out $(STAT_DIR)

clean:
	@rm -rf *.o *.out

codestyle:
	clang-format -i --style=google $(BACK_DIR)/*.c
	clang-format -i --style=google $(BACK_DIR)/*.h
CC=gcc
#DEBUG=-O0 -g3 -fno-inline
CFLAGS=-Wall -Wextra -Werror -std=c11
CFLAGS+=$(DEBUG)
LIB_NAME=s21_decimal.a
SOURCES=$(wildcard s21_*.c)
OBJECTS=$(SOURCES:.c=.o)
CHECK_FILE=./tests/s21_decimal_test.check
LFLAGS = $(shell pkg-config --cflags --libs check)

.PHONY: all test gcov_report clean rebuild remove

all: $(LIB_NAME)

$(LIB_NAME): $(OBJECTS)
	ar rcs $(LIB_NAME) $(OBJECTS)
	ranlib $(LIB_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: rebuild
	checkmk cleanmode=1 $(CHECK_FILE) > $(CHECK_FILE:.check=.c)
	gcc --coverage $(DEBUG) -g $(SOURCES) $(CHECK_FILE:.check=.c) $(LFLAGS) -L. $(LIB_NAME) -o ./tests/test -lm
	./tests/test

gcov_report: test
	lcov -t "test" -o ./tests/gcov.info -c -d . --rc lcov_branch_coverage=1
	genhtml -o gcov_report ./tests/gcov.info --rc lcov_branch_coverage=1
	open gcov_report/index.html

clean:
	-rm $(OBJECTS)
	-rm ./tests/*.gcno ./tests/*.gcda ./tests/*.info ./*.gcno ./*.gcda ./*.info ./tests/test $(CHECK_FILE:.check=.c)
	-rm -r gcov_report

rebuild: remove $(LIB_NAME)

remove: clean
	-rm $(LIB_NAME)

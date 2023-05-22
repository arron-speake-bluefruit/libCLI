example: commands.c main.c ../source/parse.c
	clang -std=c11 -Wall -Wextra -Wpedantic -I../include -O2 $^ -o $@

.PHONY: clean

clean:
	$(RM) -f example

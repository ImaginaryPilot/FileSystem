CC = gcc

CFLAGS = -O2 -std=c99 -pedantic -Wall -o -lm

main: main.c filesystem.c trie.c inode.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean debug-scan

clean:
	rm -f main

debug-scan: scan
	cat example_part1_input.txt | valgrind ./scan

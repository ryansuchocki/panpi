# =================================== PANPI ===================================
# Project Makefile
# (C) 2022 Ryan Suchocki
# http://suchocki.co.uk/

all: panpi

VERSION := $(shell git describe --tags --dirty --always)

override CFLAGS += -g -Og -pg -isystem third_party -std=gnu2x -Wall -Wextra -DVERSION=\"$(VERSION)\"
override LDFLAGS += -lm -lasound -lfftw3 -lX11

panpi: src/*.c
	gcc $(CFLAGS) $(LDFLAGS) -o panpi $^

clean:
	rm -f panpi

run: clean panpi
	./panpi

check:
	@cppcheck -q --enable=all --inconclusive --suppress=missingInclude src

	@clang-format -n --style=file src/*.c src/*.h

	@echo "Check for headers without #pragma once"
	@grep --color -L "#pragma once" src/*.h

format:
	clang-format -i --style=file src/*.c src/*.h

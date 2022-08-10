
all: panpi

COMPILE_X11 ?= true
VERSION := $(shell git describe --tags --dirty --always)

override CFLAGS += -isystem third_party -I src -std=gnu2x \
	-DVERSION=\"$(VERSION)\" -DCOMPILE_X11=$(COMPILE_X11) \
	-Wall -Wextra \
	-Wmissing-prototypes -Wmissing-declarations -Wswitch-default -Wswitch-enum \
	-Wundef -Wunused-macros -Wshadow -Winline -Wpointer-arith -Winit-self \
	-Wnull-dereference -Wvector-operation-performance -Wformat-signedness \
	-Wwrite-strings -Wlogical-op -Wjump-misses-init -Wcast-align \
	-Wconversion -Wsign-conversion \
	-Wdouble-promotion -Wfloat-conversion -Wfloat-equal #\
	# -fanalyzer

override LDFLAGS += -lm -lasound -lfftw3

SOURCES := $(wildcard src/*.c src/*/*.c)

ifdef DEBUG
	override CFLAGS += -g -Og -pg -Wno-inline
else
	override CFLAGS += -O3 -march=native -ffast-math -flto
endif

ifeq ($(COMPILE_X11), true)
	override CFLAGS += -DUSE_X11
	override LDFLAGS += -lX11
else
	SOURCES := $(filter-out src/framebuffer/framebuffer_X11.c,$(SOURCES))
endif

panpi: $(SOURCES)
	$(info $(COMPILE_X11))
	gcc $(CFLAGS) $(LDFLAGS) -o panpi $^

clean:
	rm -f panpi

run: panpi
	./panpi

check:
	@cppcheck -q --enable=all --inconclusive --suppress=missingInclude src

	@clang-format -n --style=file src/*.c src/*.h

	@echo "Check for files without copyright header"
	@grep --color -L "(C) 2022 Ryan Suchocki" src/*.c src/*.h

	@echo "Check for headers without #pragma once"
	@grep --color -L "#pragma once" src/*.h

	@echo "Check for files with TODOs"
	@! grep --color -l "TODO" src/*.c src/*.h

format:
	clang-format -i --style=file src/*.c src/*.h

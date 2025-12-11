CFLAGS =
ALL_CFLAGS = -O0 -g $(CFLAGS) -I. -Iinclude -ILibPPC/include -fopenmp

LDFLAGS =
ALL_LDFLAGS = $(LDFLAGS) LibPPC/lib/static/libppc.a -fopenmp

# caminho para MACOS /opt/homebrew/bin/gcc-15
CC = /opt/homebrew/bin/gcc-15
LD = /opt/homebrew/bin/gcc-15
# CC = gcc
# LD = gcc

HEADERS   = LibPPC/include/libppc.h
LIBRARIES = LibPPC/lib/static/libppc.a

PROGRAMS = \
	matrixmult_serial \
	matrixmult_paralelo \
	countsort_serial \
	countsort_paralelo \
	quicksort_serial \
	quicksort_paralelo \
	triangulacao_serial \
	triangulacao_paralelo \
	gerar_matrizes_mult \
	gerar_vetor \
	gerar_matriz_tri

SRCS = $(addsuffix .c,$(PROGRAMS))
OBJS = $(SRCS:.c=.o)

VPATH = src

.PHONY: all clean distclean

%.o: %.c $(HEADERS)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

all: $(PROGRAMS)

$(PROGRAMS): %: %.o $(LIBRARIES)
	$(CC) $< -o $@ $(ALL_LDFLAGS)

LibPPC/lib/static/libppc.a:
	$(MAKE) -C LibPPC static

clean:
	rm -f *.o src/*.o $(PROGRAMS)

distclean: clean
	rm -f *.dat
	$(MAKE) -C LibPPC clean

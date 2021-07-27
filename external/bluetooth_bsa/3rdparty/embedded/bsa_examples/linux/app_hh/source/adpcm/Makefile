###############################################################################
# adpcm Makefile
# This makefile is written for a GNU make or equivalent.
# For example:
#   make clean
#   make
###############################################################################

SOURCE=codec.c main.c
OBJ = codec.o main.o
HEADER=codec.h Endian.h Types.h

CC=gcc
CFLAGS = -I.
DEPS = $(HEADER)
TARGET=adpcm_test


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

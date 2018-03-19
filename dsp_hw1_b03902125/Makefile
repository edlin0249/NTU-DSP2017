.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TARGET=test_hmm train test

all: $(TARGET)
# type make/make all to compile test_hmm

train: train.c
	gcc -std=c99 -O2 train.c -o train

test: test.c
	gcc -std=c99 -O2 test.c -o test

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file

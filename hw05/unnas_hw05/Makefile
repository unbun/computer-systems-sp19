
CFLAGS := -g
LDLIBS := #-lbsd

all: tokens

tokens: tokens.o tokenize.o svec.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

tokens.o: tokens.c tokenize.h
	gcc $(CFLAGS) -c -o $@ $<

tokenize.o: tokenize.c svec.h
	gcc $(CFLAGS) -c -o $@ $<

svec.o: svec.c svec.h
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -rf tmp tokens *.o *.plist

.PHONY: all clean

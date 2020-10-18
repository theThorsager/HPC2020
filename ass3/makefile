CC = gcc
BINS = newton
CFLAGS = -O2 -lm -g -lpthread

.PHONY : all
all : $(BINS)

newton : newton.c
	$(CC) -o $@ $^ $(CFLAGS)

newtonPoint : newtonPoint.c
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY : clean
clean :
	rm -rf $(BINS)

submission: *
	tar czf submit.tar.gz newton.c makefile

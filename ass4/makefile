CC = gcc
BINS = heat_diffusion testParse
CFLAGS = -O2 -lm -g -l OpenCL

.PHONY : all
all : $(BINS)

heat_diffusion : heat_diffusion.c Parsing01.c
	$(CC) -o $@ $^ $(CFLAGS)

testParse : testParse.c Parsing01.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY : clean
clean :
	rm -rf $(BINS)

submission :
	tar czf submit.tar.gz heat_diffusion.c makefile

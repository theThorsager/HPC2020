CC = gcc
BINS = heat_diffusion 
CFLAGS = -O2 -lm -g -l OpenCL -march=native

.PHONY : all
all : $(BINS)

heat_diffusion : heat_diffusion.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY : clean
clean :
	rm -rf $(BINS)

submission :
	tar czf submit.tar.gz heat_diffusion.c heat_diffusion.cl makefile

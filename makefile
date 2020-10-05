CC = gcc
BINS = cell_distances
CFLAGS = -O2 -lm -fopenmp -g

.PHONY : all
all : $(BINS)

cell_distances : cell_distances.c
	$(CC) -o $@ $< $(CFLAGS)



.PHONY : clean
clean :
	rm -rf $(BINS)

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

submit.tgz: *
	tar czf submit.tar.gz cell_distances.c makefile parse_file.c stringReadShortBM.c

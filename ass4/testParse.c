#include <stdio.h>

void READ(float** temp, int* dim);

int
main (int argc, char* argv[])
{

  int dim[2] = {0, 0};
  float** temp;

  READ(temp, dim);

  printf("dims: %d, %d\n", dim[0], dim[1]);

  return 0;

}

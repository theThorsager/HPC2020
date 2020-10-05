#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

short distance(short* pointA, short* pointB);
 
const size_t numOutput = 3466;  // 20*sqrt(3) * 100

int
main(
     int argc,
     char* argv[])
{
  char* filePath = "./cells";

  FILE* file = fopen(filePath, "r");
  if (file == NULL)
  {
    printf("Failed to open file: ./cells");
    return -1;
  }
  
  // read command line arg -t(num)$
  int threads = 5;
  if (argc >= 2 && argv[1][0] == '-' && argv[1][1] == 't')
    threads = atoi(argv[1] + 2);

  // printf("Running with %d threads.\n", threads);
  
  omp_set_num_threads(threads);
  
  // output array, index is distance * 100, and the value is the amount of distances
  int output[numOutput];
  for (size_t ix = 0; ix < numOutput; ++ix)
    output[ix] = 0;

  // allocate memory
  size_t blockS = (1<<14) * 3; //check
  short* A = (short*) malloc(sizeof(short) * blockS * 2);
  short* B = A + blockS;

  // read the size of the file % blockS
  size_t Csize = (1<<12)*3;   // the remainder
  size_t numBlocks = 6; // amount of full blocks
  
  // initial read of A, as the C size
  // A = Read(at = 0, size = Csize);


  
  // Treat special case C
#pragma omp parallel for shared(A) reduction(+:output[:numOutput])
  for (size_t ix = 0; ix < Csize - 3; ix += 3)
  {
    for (size_t jx = ix + 3; jx < Csize; jx += 3)
    {
      short dist = distance(A + ix, A + jx);
      ++output[dist];
    }
  }
  
  for (size_t ib = 0; ib < numBlocks; ++ib)
  {
    // NextB
    // B = Read(at = Csize + ib*blockS, size = blockS)

    // Check A against everything in B
    #pragma omp parallel for shared(A,B) reduction(+:output[:numOutput])
    for (size_t ix = 0; ix < Csize; ix += 3)
    {
      for (size_t jx = 0; jx < blockS; jx += 3)
      {
	short dist = distance(A + ix, B + jx);
	++output[dist];
      }
    }
  }

  // Main loop of the different blocks
  for (size_t ia = 0; ia < numBlocks; ++ia)
  {
    // NextA
    // A = Read(at = Csize + ia*blockS, size = blockS)
     
    // Check A against itself
    #pragma omp parallel for shared(A) reduction(+:output[:numOutput])
    for (size_t ix = 0; ix < blockS - 3; ix += 3)
    {
      for (size_t jx = ix + 3; jx < blockS; jx += 3)
      {
	short dist = distance(A + ix, A + jx);
	++output[dist];
      }
    }

    for (size_t ib = ia + 1; ib < numBlocks; ++ib)
    {
      // NextB
      // B = Read(at = Csize + ib*blockS, size = blockS)

      // Check A against everything in B
      #pragma omp parallel for shared(A,B) reduction(+:output[:numOutput])
      for (size_t ix = 0; ix < blockS; ix += 3)
      {
	for (size_t jx = 0; jx < blockS; jx += 3)
	{
	  short dist = distance(A + ix, B + jx);
	  // unroll, seems like comparing and adding 3 to jx is taking time, maybe, data dependency
	  ++output[dist];
	}
      }
    }    
  }  

  free(A);
  
  // Output the output
  for (size_t ix = 0; ix < numOutput; ++ix)
  {
    // do better with parser    
    printf("%f %d\n", ((float)ix) / 100.f, output[ix]);
  }
}
/*
short distance(short* pointA, short* pointB)
{
  short distX = pointA[0] - pointB[0];
  short sqDistX = distX * distX;

  short distY = pointA[1] - pointB[1];
  short sqDistY = distY * distY;

  short distZ = pointA[2] - pointB[2];
  short sqDistZ = distZ * distZ;

  return (short)sqrtf((float)(sqDistX+sqDistY+sqDistZ));
  }
*/
short distance(short* pointA, short* pointB)
{
  short dist = pointA[0] - pointB[0];
  short sqDist = dist * dist;

  dist = pointA[1] - pointB[1];
  sqDist += dist * dist;

  dist = pointA[2] - pointB[2];
  sqDist += dist * dist;

  return (short)sqrtf((float)(sqDist));
}

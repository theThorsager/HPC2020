#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

short distance(short* pointA, short* pointB);

// The max memory for this program is 12 * blocksize + 4 * numberOfOutputs
// = 12 * 2^13 + 4 * 3466 = 112 168 bytes < 1064^3

void Read(FILE *file, short* Block, size_t at, size_t blockSize)
{
  // this method is never run in parallel, so this allocation only happens once.
  // 8 * 2^13
  char* str = malloc(sizeof(char) * 8 * blockSize);
  
  fseek(file, 8*at,SEEK_SET);
  fread(str, sizeof(char),blockSize*8,file);
  
#pragma omp parallel for shared(Block, str)
  for ( size_t i = 0; i < blockSize; ++i )
  {
    Block[i]= 0 + (str[(i*8)+1] - '0') * 10000
      + (str[(i*8)+2] - '0') * 1000
      + (str[(i*8)+4] - '0') * 100
      + (str[(i*8)+5] - '0') * 10
      + (str[(i*8)+6] - '0');

    if(str[i*8] == '-')
      Block[i] *= -1;
  }
  
  free(str);
}

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

  int count_lines = 0;
  
  char chr = getc(file);
  while(chr != EOF){
    if(chr == '\n'){
      ++count_lines;
    }
    chr = getc(file);
  }
  
  // read command line arg -t(num)$
  int threads = 1;
  if (argc >= 2 && argv[1][0] == '-' && argv[1][1] == 't')
    threads = atoi(argv[1] + 2);

  // printf("Running with %d threads.\n", threads);
  
  omp_set_num_threads(threads);
  
  // output array, index is distance * 100, and the value is the amount of distances
  // 8 * 3466
  int output[numOutput];
  for (size_t ix = 0; ix < numOutput; ++ix)
    output[ix] = 0;

  // allocate memory
  // 2 * 2 * 2^13
  size_t linesPerBlock = (1<<13);  //check
  size_t blockS = linesPerBlock * 3; 
  short* A = (short*) malloc(sizeof(short) * blockS * 2);
  short* B = A + blockS;

  // printf("Gucci\n");
  
  // read the size of the file % blockS
  size_t Csize = (count_lines % linesPerBlock) * 3;   // the remainder
  size_t numBlocks = count_lines / linesPerBlock; // amount of full blocks
  
  // initial read of A, as the C size
  Read(file, A, 0, Csize);

  /*
  printf("%d, %d, count_lines: %d \n", Csize, numBlocks, count_lines);

  for (size_t ix = 0; ix < Csize; ix += 3)
    printf("{%d, %d, %d}\n", A[ix], A[ix+1], A[ix+2]);

  printf("%d\n", output[0]);
  
  return 0;
  */  

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
    Read(file, B, Csize + ib * blockS, blockS);

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
    Read(file, A, Csize + ia * blockS, blockS);
     
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
      Read(file, B, Csize + ib * blockS, blockS);
      
      // Check A against everything in B
      #pragma omp parallel for shared(A,B) reduction(+:output[:numOutput])
      for (size_t ix = 0; ix < blockS; ix += 3)
      {
	for (size_t jx = 0; jx < blockS; jx += 3)
	{
	  short dist = distance(A + ix, B + jx);
	  ++output[dist];
	}
      }
    }    
  }  
    
  // Output the output
  for (size_t ix = 0; ix < numOutput; ++ix)
  {
    // if (output[ix] != 0)
      printf("%05.2f %d\n", ((float)ix) / 100.f, output[ix]);
  }

  free(A);
}

short distance(short* pointA, short* pointB)
{
  int dist = (int)pointA[0] - (int)pointB[0];
  int sqDist = dist * dist;
    
  dist = (int)pointA[1] - (int)pointB[1];
  sqDist += dist * dist;
  
  dist = (int)pointA[2] - (int)pointB[2];
  sqDist += dist * dist;
  
  return (short)((sqrtf((float)(sqDist))) / 10.f);
}

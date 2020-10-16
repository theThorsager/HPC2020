#include <stdlib.h>
#include <stdio.h>

void WritePPM ( int d,
		 int MatrixSize,
		 int** ConvMatrix,
		 int** IterMatrix
		 );

int main(int argc, char* argv[])
{
  
  int d = 7;
  int size = 1000;
  
  // making test convergence matrix
  int * asentries = (int*) malloc(sizeof(int) * size*size);
  int ** ConvMatrix = (int**) malloc(sizeof(int*) * size);
  for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
    ConvMatrix[ix] = asentries + jx;

  for ( size_t ix = 0; ix < size; ++ix )
    for ( size_t jx = 0; jx < size; ++jx )
      ConvMatrix[ix][jx] = 0; // filling it with 0 (->black)
  //

  WritePPM(d, size, ConvMatrix, ConvMatrix);

  free(ConvMatrix);
  free(asentries);
  
  return 0;
}


void WritePPM ( int d,
		 int size,
		 int** ConvMatrix,
		 int** IterMatrix
		 )
{

  int MaxConvColour = d; // assumes member index matrix
  int MaxIterColour = 50; // given
  
  //Create template file Convergernce roots
  FILE *fpc = fopen("convPic.ppm", "wb");
  fprintf(fpc, "P3\n%d %d\n%d\n", size, size, MaxConvColour);

  //Create template file iterations
  FILE *fpi = fopen("iterationPic.ppm", "wb");
  fprintf(fpi, "P3\n%d %d\n%d\n", size, size, MaxIterColour);

  //Writing to file once
  int * AllConvColours = (int*) malloc(sizeof(int) * size * size * 3);
  int * AllIterColours = (int*) malloc(sizeof(int) * size * size * 3);

  for ( size_t j = 0; j < size; j++ )
    {
      for ( size_t i = 0; i < size*3; i+=3 )
	{
	  AllConvColours[j*size + i] = ConvMatrix[j][i];
	  AllConvColours[j*size + i+1] = ConvMatrix[j][i];
	  AllConvColours[j*size + i+2] = ConvMatrix[j][i];

	  AllIterColours[j*size + i] = IterMatrix[j][i];
	  AllIterColours[j*size + i+1] = IterMatrix[j][i];
	  AllIterColours[j*size + i+2] = IterMatrix[j][i];
	  
	}
    }
  fwrite(AllConvColours, sizeof(int), size*size*3, fpc);
  fwrite(AllIterColours, sizeof(int), size*size*3, fpi);

  free(AllIterColours);
  free(AllConvColours);
  
  fclose(fpc);
  fclose(fpi);

}

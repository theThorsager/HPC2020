#include <stdlib.h>
#include <stdio.h>

//storing write data on stack and writing all data at once -> stack overflow
//storing write data on heap and writing all data at once 
void WritePPM2(  int d,
		 int size,
		 int* a_conv,
		 int* AllConvColours,
		 int* a_iter,
		 int* AllIterColours,
		 int ppmDim
		 );

int main(int argc, char* argv[])
{
  
  int d = 7;
  int size = 100000;//testar med en miljon

  
  // making test convergence matrix full of zeros
  int * a_conv = (int*) calloc(size, sizeof(int));

  //matrix to fill with colours
  int * AllConvColours = (int*) malloc(sizeof(int) * size * 3);
  int * AllIterColours = (int*) malloc(sizeof(int) * size * 3);

  
  WritePPM2(d, size, a_conv, AllConvColours, a_conv, AllIterColours, size);
 
  free(a_conv);
  
  return 0;
}


void WritePPM2(  int d,
		 int size,
		 int* a_conv,
		 int* AllConvColours,
		 int* a_iter,
		 int* AllIterColours,
		 int ppmDim
		 )
{

  int MaxConvColour = d; // assumes member index matrix
  int MaxIterColour = 50; // given
  
  //Create template file Convergernce roots
  FILE *fpc = fopen("convPic.ppm", "wb"); /* b - binary mode */
  fprintf(fpc, "P3\n%d %d\n%d\n", ppmDim, ppmDim, MaxConvColour);

  //Create template file iterations
  FILE *fpi = fopen("iterationPic.ppm", "wb"); /* b - binary mode */
  fprintf(fpi, "P3\n%d %d\n%d\n", ppmDim, ppmDim, MaxIterColour);

  //Writing to file once
  //int AllConvColours[size*3];
  //int AllIterColours[size*3]; -> Stack overflow

  //int * AllConvColours = (int*) malloc(sizeof(int) * size * 3);
  //int * AllIterColours = (int*) malloc(sizeof(int) * size * 3);

  size_t j=0;
  for ( size_t i = 0; i < size; ++i )
	{
	  AllConvColours[j] = a_conv[i];
	  AllConvColours[j+1] = a_conv[i];
	  AllConvColours[j+2] = a_conv[i];

	  AllIterColours[j] = a_iter[i];
	  AllIterColours[j+1] = a_iter[i];
	  AllIterColours[j+2] = a_iter[i];
	  j+=3;
	}
  fwrite(AllConvColours, sizeof(int), size*3, fpc);
  fwrite(AllIterColours, sizeof(int), size*3, fpi);

  //free(AllIterColours);
  //free(AllConvColours);
  
  fclose(fpc);
  fclose(fpi);

}

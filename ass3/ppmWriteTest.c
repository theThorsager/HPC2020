#include <stdlib.h>
#include <stdio.h>

//storing write data on heap and writing all data at once 
void WritePPM2(  int size,
		 int* a_attr,
		 char* AllAttrColours,
		 FILE* fpc,
		 int* a_conv,
		 char* AllConvColours,
		 FILE* fpi,
		 int Colours[10][3]
		 );

int main(int argc, char* argv[])
{

  int d = 7;
  
  // Andreas Naming thing (remove since already exists)
  char fpc_name[30];
  char fpa_name[30];

  snprintf(fpc_name,sizeof(fpc_name),"newton_convergence_x%d.ppm",d);
  snprintf(fpa_name,sizeof(fpa_name),"newton_attractors_x%d.ppm",d); 
  //
  
  int size = 100000;
  int ppmDim = size;

  //making colour matrix to pick from in the write function
  int Colours[10][3] = {{2, 0, 0}, {2, 1, 0}, {2, 2, 0},
			{1, 2, 0}, {0, 2, 0}, {0, 2, 1},
			{0, 2, 2}, {0, 1, 2}, {0, 0, 2},
			{1, 0, 2}};
  
  //Create template file Attraction
  FILE* fpa = fopen(fpa_name, "w");
  fprintf(fpa, "P3\n%d %d\n%d\n", ppmDim, ppmDim, 2);

  //Create template file Convergence
  FILE* fpc = fopen(fpc_name, "w");
  fprintf(fpc, "P3\n%d %d\n%d\n", ppmDim, ppmDim, 50);

  // making test convergence matrix full of zeros
  int* a_attr = (int*) calloc(size, sizeof(int));
  int* a_conv= (int*) calloc(size, sizeof(int));

  //matrix to fill with colours
  char* AllAttrColours = malloc(sizeof(char) * size * 3 * 2);
  char* AllConvColours = malloc(sizeof(char) * size * 3 * 3);
  
  WritePPM2(size, a_attr, AllAttrColours, fpa, a_conv, AllConvColours, fpc, Colours);
  
  free(a_attr);
  free(a_conv);
  free(AllAttrColours);
  free(AllConvColours);

  fclose(fpa);
  fclose(fpc);
  
  return 0;
}


void WritePPM2(  int size,
		 int* a_attr,
		 char* AllAttrColours,
		 FILE* fpa,
		 int* a_conv,
		 char* AllConvColours,
		 FILE* fpc,
		 int Colours[10][3]
		 )
{
  size_t ja=0;
  size_t jc=0;
  
  for ( size_t i = 0; i < size; ++i )
	{
	  int k = a_attr[i]; 
	  AllAttrColours[ja] = Colours[k][0] + 48;
	  AllAttrColours[ja+1] = ' ';
	  AllAttrColours[ja+2] = Colours[k][1] + 48;
	  AllAttrColours[ja+3] = ' ';
	  AllAttrColours[ja+4] = Colours[k][2] + 48;
	  AllAttrColours[ja+5] = ' ';
	  /*
	  AllConvColours[j] = a_conv[i];
	  AllConvColours[j] = a_conv[i];
	  AllConvColours[j] = a_conv[i];
	  AllConvColours[j+1] = a_conv[i];
	  AllConvColours[j+1] = a_conv[i];
	  AllConvColours[j+1] = a_conv[i];
	  AllConvColours[j+2] = a_conv[i];
	  AllConvColours[j+2] = a_conv[i];
	  AllConvColours[j+2] = a_conv[i];
	  */
	  
	  ja+=6;
	}
  

  
  fwrite(AllAttrColours, sizeof(char), size*3*2, fpa);
  fwrite(AllConvColours, sizeof(char), size*3*3, fpc);
 
}

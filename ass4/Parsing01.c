#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void READ(float** temp, int* dim)
{
  char* filePath = "./diffusion.txt";
  FILE* file = fopen(filePath,"r");
  if ( file == NULL)
    {
    printf("Failed to open file sademoji!\n");
    exit(1);
    }
  
  int firstDataLineSize = 20;
  char* dimString = malloc(sizeof(char)* firstDataLineSize);
  fread(dimString, sizeof(char), firstDataLineSize,file);

  int dimsz = 2;
  // int dim[2] = {0, 0};
  int DimIndex = 0;
  
  int j = 0;
  int dimsizeskip;
  for ( size_t i = 0; i < firstDataLineSize; ++i )
    {
      if( dimString[i] == ' ' || dimString[i] == '\n')
	{
	  char dest[i-j+1];
	  dest[i-j] = '\0';
	  strncpy(dest, dimString + j,i-j);
	  dim[DimIndex] = atoi(dest);

	  DimIndex++;
	  if( DimIndex == 2)
	    {
	      dimsizeskip=i+1;
	      break;
	    }
	  j=i+1;
	  }
    }

  //create temp matrix full of zeros
  float* tempEntries = calloc((dim[0]+2)*(dim[1]+2), sizeof(float));
  temp = (float**)malloc(sizeof(float*)*(dim[0]+2));
  for ( size_t ix = 0, jx = 0; ix <dim[0]+2; ++ix, jx+=dim[1]+2)
    temp[ix] = tempEntries + jx;


  // find the length of the dimension part of the data

  //find the size of the data file without dimension line
  fseek(file,0,SEEK_END);
  int fileEnd = ftell(file);
  fseek(file,dimsizeskip,SEEK_SET);
  int fileStart = ftell(file);
  int filelength = fileEnd-fileStart;

  //read eveything in file
  char* data = malloc(sizeof(char) *filelength);
  fread(data, sizeof(char), filelength,file);

  j=0;
  int k=0;

  int X;
  int Y;
  for( size_t i = 0; i < filelength; i++)
    {      
      if( data[i] == ' ' || data[i] == '\n')
	{
	  char dest[i-j+1];
	  dest[i-j] = '\0';
	  strncpy(dest, data + j,i-j);
	  
	  switch (k)
	    {
	    case 0:
	      X = atoi(dest);
	      k=1;
	  break;
	    case 1:
	      Y = atoi(dest);
	      k=2;
	  break;
	    case 2:
	      temp[X+1][Y+1]= atof(dest);
	      k=0;
	  break;
	    }
	  j=i+1;
	}
    }
}

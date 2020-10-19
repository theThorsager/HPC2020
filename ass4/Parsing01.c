#include <stdio.h>
#include <stdlib.h>

void READ( FILE* file, )
{
  

}
int
main (int argc, char* argv[])
{

  //char* testData = "100 100\n44 64 9.355469544534684e+18\n82 99 2.643092747172943e+18\n29 29 5.266497277936674e+18";

  char* filePath = "./testData";
  FILE* file = fopen(filepath,"r");
  if ( file == NULL)
    {
    fprint("Failed to open file sademoji!\n");
    return -1;
    }
  
  int firstDataLineSize = 13;
  char* dimString = malloc(sizeof(char)* firstDataLineSize);
  fread(dimString, sizeof(char), firstDataLineSize,file);

  int dimsz = 2;
  int dim[2] = {0, 0};
  int DimIndex = 0;
  
  int j = 0;
  int dimsizeskip;
  for ( size_t = i; i<firstDataLineSize; ++i )
    {
      if( dimString[i] == ' ')
	{
	  char* dest;
	  
	  strncopy(dest, dimString + j,i-j);
	  dim[DimIndex] = atoi(dest);

	  DimIndex++;
	  if( DimIndex == 2)
	    {
	      dimsizeskip=i;
	      break;
	    }
	  j=i;
	  }
    }

  //create temp matrix full of zeros
  float* tempEntries = calloc((dim[0]+2)*(dim[1]+2), sizeof(float));
  float** temp = (float**)malloc(sizeof(float*)*dim[0]);
  for ( size_t ix = 0, jx = 0; ix <dim[0]+2; ++ix, jx+=dim[1]+2)
    temp[ix = tempEntries + jx;
  
  //find the size of the data file
  fseek(file,0,SEEK_END);
  int fileEnd = ftell(file);
  fseek(file,0,SEEK_SET);
  int fileStart = ftell(file);
  int filelength = fileEnd-fileStart;

  //read eveything in file
  char* data = malloc(sizeof(char) *filelength);
  fread(data, sizeof(char), filelength,file);

  int j=0;
  int k=0;

  int X;
  int Y;
  for( size_t i = 0; i < filelength; i++)
    {
      
      if( data[i] == ' ' && k==0)
	{
	  char* dest;
	  strncopy(dest, data + j,i-j);
	  X = atoi(dest);
	  j=i+1;
	  k++;
	}
      if( data[i] == ' ' && k==1)
	{
	  char* dest;
	  strncopy(dest, data + j,i-j);
	  Y = atoi(dest);
	  j=i+1;
	  k++;
	}

      if( data[i] == ' ' && k==2)
	{
	  char* dest;
	  strncopy(dest, data + j,i-j);
	  temp[X+1][Y+1]= atof(dest);
	  
	  j=i+1;
	  k=0;
	}
    }

   
  return 0;
}

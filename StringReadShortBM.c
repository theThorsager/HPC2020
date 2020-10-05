#include <stdio.h>
#include <stdlib.h>

// ------ INPUT ------
// one single string
void
myAtoi(char* str, short* A, short ASize)
{
 
  if( ASize % 1 != 0 )
    {
      printf("Could not read the char array as expected!\n");
    }
      
  for( size_t i = 0; i < ASize; ++i )
    {
      A[i]= 0 + (str[(i*8)+1] - '0') * 10000
	               + (str[(i*8)+2] - '0') * 1000 
                       + (str[(i*8)+4] - '0') * 100 
	               + (str[(i*8)+5] - '0') * 10
	               + (str[(i*8)+6] - '0');

      if(str[i*8] == '-')
	A[i] *= -1;
    }

}

int
main(int argc, char* argv[])
{

  char testString[] = "+01.330 -09.035 +03.489\n-03.718 +02.517 -05.995\n";
  short ASize = sizeof(testString)/sizeof(char)*8;
  short A[ASize];

  int iterations = 1000000;
  for (size_t i = 0; i < iterations; ++i)
    {
      myAtoi(testString,A,ASize);
    }
  
  for (size_t i=0; i<6; ++i)
    printf("%d\n", A[i]);

  return 0;
  
}


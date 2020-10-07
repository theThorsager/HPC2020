#include <stdio.h>
#include <stdlib.h>


struct coord{
  short int x;
  short int y;
  short int z;
};
  
  
void parse_file(FILE *file, char *fileread, int block_nr, int points_in_block){
  fseek(file, 24*block_nr*points_in_block,SEEK_SET);
  fread(fileread, sizeof(char),points_in_block*24,file);
  }

/*
int main(int argc,char* argv[]){
  int points_in_block=1;

  char fileread[100];

  int block_nr=1;

  FILE* file= fopen("point.txt","r");

  short int coordinates [3];

  parse_file(file, fileread, block_nr, points_in_block);


  printf("%s",fileread);


}
*/

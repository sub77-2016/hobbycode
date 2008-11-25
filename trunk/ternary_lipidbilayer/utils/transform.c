#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#define LINE_SIZE 1000

void showUsage(const char *c){
  printf("Usage: %s <file.gro>\n",c);
}

int main(int argn, char *args[]){
  FILE *mf;
  char *c;
  char linein[LINE_SIZE];
  float mx,my,mz;
  char res[256],atom[256];
  int id,num,i;
  
  if(argn!=2){
    showUsage(args[0]);
    exit(1);
  }
  
  mf=fopen(args[1],"r");
  if(mf==NULL){
    printf("error: unable to open the membrane file %s\n",args[1]);
    exit(1);
  }

  for (i=0;i<2;i++) {
  //while(fgets(linein,LINE_SIZE,mf)!=NULL){
    //c=&linein[strlen(linein)-48]; // would use -48 if velocities are also in .gro and -24 otherwise
    c=&linein[0];
    //if(sscanf(c,"%f %f %f",&mx,&my,&mz)!=3) continue;
    //sscanf(c,"%d %s %s %d",num,&res,&atom,id);
    //printf("%d %s %s %d\n",num,res,atom,id);
    printf("%d\n",(int)strlen(c));
    printf("%s\n",linein);
  }
  fclose(mf);

  return 0;
}

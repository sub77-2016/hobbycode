#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#define LINE_SIZE 1000
#define NUMATOM 12

void showUsage(const char *c){
  printf("Usage: %s <file.gro>\n",c);
}

int main(int argn, char *args[]){
  FILE *mf;
  char *c;
  char linein[LINE_SIZE];
  float mx,my,mz;
  //char res[256],atom[256];
  int numline = 0, numtop = 0, numbott = 0;
  int itop[1000], ibott[1000], i;
  double sum_z = 0, z_bar;
  double num_z = 0;
  
  if(argn!=2){
    showUsage(args[0]);
    exit(1);
  }
  
  mf=fopen(args[1],"r");
  if(mf==NULL){
    printf("error: unable to open the membrane file %s\n",args[1]);
    exit(1);
  }

  //for (i=0;i<2;i++) {
  while(fgets(linein,LINE_SIZE,mf)!=NULL){
    c=&linein[strlen(linein)-48]; // would use -48 if velocities are also in .gro and -24 otherwise
    if(sscanf(c,"%f %f %f",&mx,&my,&mz)!=3) continue;
    printf("%f %f %f\n",mx,my,mz);
    sum_z += mz;
    num_z += 1;
  }
  fclose(mf);

  z_bar = sum_z/num_z;

  printf("zbar = %f\n", z_bar);

  mf=fopen(args[1],"r");
  if(mf==NULL){
    printf("error: unable to open the membrane file %s\n",args[1]);
    exit(1);
  }

  //for (i=0;i<2;i++) {
  while(fgets(linein,LINE_SIZE,mf)!=NULL){
    ++numline;
    c=&linein[strlen(linein)-48]; // would use -48 if velocities are also in .gro and -24 otherwise
    if(sscanf(c,"%f %f %f",&mx,&my,&mz)!=3) continue;
    
    if( (numline-1)%NUMATOM == 0 ) {
      if (mz >= z_bar) {
	itop[numtop++] = numline;
      }
      else {
	ibott[numbott++] = numline;
      }
    }
  }
  fclose(mf);

  mf=fopen("result.txt","w+");
  if(mf==NULL){
    printf("error: unable to open the membrane file %s\n",args[1]);
    exit(1);
  }

  for(i=0;i<numtop;i++) fprintf(mf, "%d ", itop[i]); fprintf(mf, "\n===\n");
  for(i=0;i<numbott;i++) fprintf(mf, "%d ", ibott[i]); fclose(mf);

  return 0;
}

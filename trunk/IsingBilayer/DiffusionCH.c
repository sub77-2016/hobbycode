/* diffuS.c 
 * W. Namsaad
 * waipotn@yahoo.com
 * Solve 2D diffusion equation using explicit method
 * 15/08/2004
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "mygraph.h"

/* define the constant */
#define MAX_X	128
#define MAX_Y	128
#define MAX_T	1e9
#define WRITE	100

//Initial simulation params
#define XDIMINIT (100)
#define YDIMINIT (100)
#define REPEATINIT (10)

//Searchparams
#define TMAX  (0.10)
#define TMIN1 (0.01)
#define TSTP1 (0.01)
#define TMIN2 (0.001)
#define TSTP2 (0.001)

//Secondary inputs
int xdim = MAX_X;
int ydim = MAX_Y;

//Simulation book-keeping
int iterations = REPEATINIT;
int repeat = REPEATINIT;
int saverepeat = REPEATINIT;
int stabilize = 100000;
int iteration_max = MAX_T;

//Flow controls
int graphics = 0;
int Pause = 1;
int singlestep = 0;
int done = 0;
int automate = 0;

//Simulation parameters
#define D1 0.25
#define D2 0.25

#define A -1
#define B 1
#define K 0.9
#define Amp 0.001

double lambda = 0.001, s = 0.1, PARAM = 0;
double r_zero[2] = {0.,0.};

/* data structure */
double phi[MAX_X][MAX_Y], psi[MAX_X][MAX_Y], rho[MAX_X][MAX_Y];
double newu1[MAX_X][MAX_Y], newu2[MAX_X][MAX_Y], MU1[MAX_X][MAX_Y], MU2[MAX_X][MAX_Y];

/* declair the operation */
void init(void);
void writedata(double **arr, char *file_name, long t);
void progess(long t);
void EulerStep(void);


/* implement the functions */

/*******************************************
* initialize the concentration at t=0
********************************************/
void init(void)
{
  int i, j;
  double rnd;
  /* Initial */
  for(i=0; i<MAX_Y; i++)
    {
      for(j=0; j<MAX_X; j++)
	{	
	  phi[i][j] = r_zero[0];
	  psi[i][j] = r_zero[1];
	  rho[i][j] = 0.;
	  
	  MU1[i][j] = 0.;
	  MU2[i][j] = 0.;
	}
    }
  
  /* random */
  for(i=0; i<MAX_Y; i++)
    {
      for(j=0; j<MAX_X; j++)
	{	
	  rnd = (double) rand()/RAND_MAX; /* printf("%f\n",rnd); */
	  phi[i][j] += Amp*(2.*rnd-1.);
	  rnd = (double) rand()/RAND_MAX;
	  psi[i][j] += Amp*(2.*rnd-1.);
	}
    }	

  //Reset book-keeping
  if (repeat==0) 
    repeat = saverepeat;
  
  iterations = 0;
		
}

/*************************************************
* write output
**************************************************/
void EulerStep(){
  int i, j, i_prev, i_next, j_prev, j_next;

  /* Update MUs */
  for(i=0; i<MAX_Y; i++)
    {
      //Periodic BC. in X-dir
      i_prev = (i+MAX_X-1)%MAX_X;
      i_next = (i+1)%MAX_X;

      for(j=0; j<MAX_X; j++)
	{
	  //Periodic BC. in Y-dir
	  j_prev = (j+MAX_Y-1)%MAX_Y;
	  j_next = (j+1)%MAX_Y;	
		   
	  MU1[i][j] = -K*(phi[i][j_prev]+phi[i][j_next]+phi[i_prev][j]+phi[i_next][j]-4.*phi[i][j]);
	  MU2[i][j] = -K*(psi[i][j_prev]+psi[i][j_next]+psi[i_prev][j]+psi[i_next][j]-4.*psi[i][j]);
	      
	  MU1[i][j] += A*phi[i][j]+B*phi[i][j]*phi[i][j]*phi[i][j]+lambda*(phi[i][j]-psi[i][j]);
	  MU2[i][j] += A*psi[i][j]+B*psi[i][j]*psi[i][j]*psi[i][j]-lambda*(phi[i][j]-psi[i][j]);
	}
    } /* End MUs loop */


   /* Update Euler explicit */
  for(i=0; i<MAX_Y; i++)
    {
      //Periodic BC. in X-dir
      i_prev = (i+MAX_X-1)%MAX_X;
      i_next = (i+1)%MAX_X;

      for(j=0; j<MAX_X; j++)
	{
	  //Periodic BC. in Y-dir
	  j_prev = (j+MAX_Y-1)%MAX_Y;
	  j_next = (j+1)%MAX_Y;	
				
	  newu1[i][j] = phi[i][j]+s*D1*(MU1[i][j_prev]+MU1[i][j_next]+MU1[i_prev][j]+MU1[i_next][j]-4.*MU1[i][j]);
	  newu2[i][j] = psi[i][j]+s*D2*(MU2[i][j_prev]+MU2[i][j_next]+MU2[i_prev][j]+MU2[i_next][j]-4.*MU2[i][j]);
	}
    } /* End space loop */


  /* switch back */
  for(i=0; i<MAX_Y; i++)
    { 	
      for(j=0; j<MAX_X; j++)
	{
	  phi[i][j] = newu1[i][j];
	  psi[i][j] = newu2[i][j];

	  rho[i][j] = phi[i][j]+psi[i][j];
	}
    }
}

void writedata(double **arr, char *file_name, long t)
{
  int i, j;
  char name[100];
  char temp[100];
  FILE *fl;
  
  sprintf(name, file_name);
  sprintf(temp, "_%d.txt", t);
  strcat(name, temp);
  
  fl = fopen(name, "w");
  
  for(i=0; i<MAX_Y; i++)
    {
      for(j=0; j<MAX_X; j++)
	fprintf(fl, "%.9f\t", arr[i][j]);
      fprintf(fl, "\n");
    }
  
  fclose(fl);
}

/**************************************************************
* tell the percentage of progess
***************************************************************/
void progess(long t)
{	
  long mod = MAX_T/10;
  
  if ( !(t%mod) )
    printf("Progess %.2f percent\n", (double) 100*t/MAX_T);
}

const char* wtime_string(double sec)
{
#define BUF_SIZE 64
  static char buf[BUF_SIZE + 1];

  if (sec/60 < 1.0)
    snprintf(buf, BUF_SIZE, "%f sec", sec);
  else if (sec/(60*60) < 1.0) {
    const double min = sec/60;
    snprintf(buf, BUF_SIZE, "%f min", min);
  } else {
    const double hours = sec/(60*60);
    snprintf(buf, BUF_SIZE, "%f hours", hours);
  }

  return buf;
}

void check_unstable(void) {
  int c = isnormal(phi[xdim/2][ydim/2])&&isnormal(psi[xdim/2][ydim/2]);
  if (!c) {
    printf("It's not stable\n");
    s -= TSTP1;
    if (s<TMIN1) {
      s += TSTP1;
      s -= TSTP2;
    }
    if (s<TMIN2) {
      printf("\"t\" cannot be lowered enough to maintain stability.\n");
      done = 1;
    }
    saverepeat = repeat;
    repeat = 0;
  }
}

void iteration1(void){
  EulerStep();

  check_unstable();
}

void analysis(void){
}

//Graphics
void GetGraphics(void){
}

void GUI(void){	
  SetDefaultLineType(0);
  DefineGraphNxN_R("phi", &phi[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_R("psi", &psi[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_R("rho", &rho[0][0], &xdim, &ydim, NULL); 
  NewGraph();

  StartMenu("Ising Bilayer", 1);

  SetActiveGraph(0);
  DefineGraph(contour2d_,"Visualize");

  //StartMenu("Controls",0);
  DefineInt("iterations", &iterations);
  DefineInt("repeat", &repeat);
  DefineDouble("dt", &s);
  DefineDouble("phi_init", &r_zero[0]);
  DefineDouble("psi_init", &r_zero[1]);
  //DefineDouble("Temperature", &T);
  DefineDouble("Lambda", &lambda);
  //EndMenu();

  StartMenu("Secondary Inputs",0);
  DefineInt("xdim", &xdim);
  DefineInt("ydim", &ydim);
  EndMenu();

  DefineFunction("reinitialize",&init);

  DefineBool("Pause",&Pause);
  DefineBool("Single Step",&singlestep);
  DefineBool("Done",&done);	
  EndMenu();
}

// Never change if not neccesary
int main(int argc, char *argv[]){
  int n, newdata = 0;

  time_t BEGIN_T;
  char name[256], hname[128];
  FILE *out;
 
  srand(time(NULL));

  //printf("%d\n", argc);
  if(argc < 1) {
    printf("Usage: %s [GUI 1 or 0]\n", argv[0]);
  }
  else {
    //PARAM = atof(argv[1]);
    
    if (argc == 2)
      graphics = atoi(argv[1]);
  }

  if (graphics)
    GUI();
  
  if (!graphics)
    printf("Running PARAM = %f on %dx%d Lattice with %d iterations\n",PARAM,MAX_X,MAX_Y,iteration_max);

  if (!graphics) Pause = 0;
  
  init(); 

   //test for speed
  BEGIN_T = time(NULL);
  for (n=0;n<10;n++)
    iteration1();

  if (!graphics)
    printf("sec/iteration: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)/10) );

  init();

  //Just reset
  sprintf(name,"data-%f.txt",PARAM);
  out = fopen(name,"w+");

 //Write some information
  gethostname(hname,(size_t)sizeof(hname));

  fprintf(out,"#Size: %dx%d\n#PARAM: %f\n#itertions: %d\n#Repeat: %d\n#sec/iteration: %s\n#Host: %s\n#PID: %d\n#---BEGIN DATA---\n\n\n",MAX_X,MAX_Y,PARAM,iteration_max,repeat,wtime_string( (double)(time(NULL)-BEGIN_T)/10 ), hname, getpid() );

  fclose(out);  

  //Now start simulation
  BEGIN_T = time(NULL);

  while ( (iterations<iteration_max)&&(!done) ){//Begin Main Loop
    if (graphics) {
      Events(newdata);
      GetGraphics();
      DrawGraphs();
    } else {
      //done=1;
      //Pause=0;
    }

    if (!Pause||singlestep) {
      singlestep = 0;
      newdata = 1;
      
      for (n=0;n<repeat;n++) {
	iterations++;
	iteration1();
      }
      analysis();
      
      if (!graphics)
	repeat *= 1.1;

      if (!graphics)
	printf("ETA: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)*(iteration_max-iterations)/iterations) );

      if (repeat==0) 
	init();
    }
    else 
      sleep(1);

  }//End Main Loop

  return 0;
}


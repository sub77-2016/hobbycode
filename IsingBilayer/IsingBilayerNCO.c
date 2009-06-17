//2D Ising simulation of Lipid Membrane for Non-conserved Order Parameter
//It is based on standard spin-flip algorithm
//Waipot Ngamsaad (waipotn@yahoo.com)
//June 17, 2009

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "mygraph.h"

//Maximum Simulation Size
#define LMAX (256)
#define XMAX (LMAX)
#define YMAX (XMAX)
#define NMAX (XMAX*YMAX)
#define TEMPINIT (2.0)
#define LAMBDAINIT (0.0)

//Initial simulation params
#define XDIMINIT (100)
#define YDIMINIT (100)
#define REPEATINIT (1)

//Searchparams
#define TMAX  (0.10)
#define TMIN1 (0.01)
#define TSTP1 (0.01)
#define TMIN2 (0.001)
#define TSTP2 (0.001)

#ifndef Error
#define Error(Str) Fatal_error(Str)
#define Fatal_error(Str) fprintf(stderr,"%s\n",Str), exit(1)
#endif

#ifndef rnd
#define rnd(void) (double)rand()/(double)RAND_MAX
#endif

//Main inputs
double T = TEMPINIT, Told=TEMPINIT, lambda = LAMBDAINIT, lambdaold = LAMBDAINIT, PARAM = 0;
double r_zero[2] = {0.5,0.5};

//Secondary inputs
int x_zero = 0;
int xdim = XMAX;
int ydim = YMAX;

//Stability factors
double s = 0.1;

//Simulation book-keeping
int iterations = REPEATINIT;
int repeat = REPEATINIT;
int saverepeat = REPEATINIT;
int stabilize = 100000;
int iteration_max = 1000000;

//Flow controls
int graphics = 0;
int Pause = 1;
int singlestep = 0;
int done = 0;
int automate = 0;

//Global Variables
double rho[XMAX][YMAX];
double phi[YMAX][YMAX];
double psi[XMAX][YMAX];
double w[17][3];

void GenerateSpin(double spin[XMAX][YMAX], double ratio) {
  int i,j;
  //Set initial spin configuration
  for (i=0;i<xdim;i++) {
    for (j=0;j<ydim;j++) {
      spin[i][j] = rnd()<=ratio ? +1: -1;
    }
  }
}

void TotalSpin(double spin1[XMAX][YMAX],double spin2[XMAX][YMAX],double tot[XMAX][YMAX])
{
  int i,j;
  //Total spin
  for (i=0;i<xdim;i++) {
    for (j=0;j<ydim;j++) {  
      tot[i][j] = spin1[i][j] + spin2[i][j]; 
    }
  }
}

void ComputeBoltzmannProb(void){
  int dE;
  for (dE = -8; dE <= 8; dE=dE+4) {
    w[dE+8][0] = exp(-((double)dE-lambda)/T);
    w[dE+8][2] = exp(-((double)dE+lambda)/T);
  }
}

void Init(void) {
  int i, j;

  char name[256];
  FILE *out;

  //Zero arrays
  for (i=0;i<xdim;i++) {
    for (j=0;j<ydim;j++) {  
      phi[i][j] = 0; 
      psi[i][j] = 0; 
      rho[i][j] = 0;   
    }
  }

  //init spin
  GenerateSpin(phi,r_zero[0]);
  GenerateSpin(psi,r_zero[1]);

  TotalSpin(phi,psi,rho);

  //Compute Boltzmann probability ratios
  ComputeBoltzmannProb();

  //Reset book-keeping
  if (repeat==0) 
    repeat = saverepeat;
  
  iterations = 0;
  //x_zero = 0;

  sprintf(name,"data-%f.txt", PARAM);
  out = fopen(name,"w+");

  if (out==(FILE *)NULL) Error("\n Error: Cannot open data-*.txt!!! \n");

  fclose(out);
}

void OnTempChange()
{
  if ( (T!=Told)||(lambda!=lambdaold) )
    {
      Told = T;
      lambdaold = lambda;
      //re-compute Boltzmann probability ratios
      ComputeBoltzmannProb();
    }
}

int DeltaE(double spin[XMAX][YMAX],int x,int y,int L)
{
  int xm,xp,ym,yp,dE,left,right,up,down,center;

  //Periodic BC.
  xm  = (x+xdim-1)%xdim;
  xp  = (x+1)%xdim;
  ym  = (y+ydim-1)%ydim;
  yp  = (y+1)%ydim;

  center = spin[x][y];
  left = spin[xm][y];
  right = spin[xp][y];
  up = spin[x][yp];
  down = spin[x][ym];
    
  dE = 2*center*(left + right + up + down);

  return dE;
}

void Exchange(double spin1[XMAX][YMAX],double spin2[XMAX][YMAX],int N,int L,double *E,double *M,double *accept)
{
  /*one Monte Carlo step per spin  */
  int ispin,x,y,dE,choose=0;
  //Check If Temperature
  OnTempChange();
  for (ispin=1; ispin <= N; ispin++)
    {
      /* random x and y coordinates for trial spin  */
      x = L*rnd();
      y = L*rnd();
      dE = DeltaE(spin1,x,y,L);      
      choose = lambda != 0 ? 1+(int)(spin1[x][y]*spin2[x][y]): 1+(int)spin1[x][y];
      if (rnd() <= w[dE+8][choose])
	{
	  spin1[x][y] = -spin1[x][y];
	  *accept = *accept + 1;
	  *M = *M + 2*spin1[x][y];
	  *E = *E + dE;
	}
    }
}

void WriteFile() {
  int i,j;
  char name[256];

  FILE *out;

  sprintf(name,"image/image-%f-%08i.txt",PARAM,iterations);
  out = fopen(name,"w+");

  if (out==(FILE *)NULL) Error("\n Error: Cannot open ./image!!! \n");

  for (i=0;i<xdim;i++){
    for (j=0;j<ydim;j++){
      fprintf(out,"%f ", phi[i][j]);
    }
    fprintf(out,"\n");
  }
  fclose(out);
  
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
  double E=0,M=0,accept=0;

  Exchange(phi,psi,NMAX,LMAX,&E,&M,&accept);
  Exchange(psi,phi,NMAX,LMAX,&E,&M,&accept);
  TotalSpin(phi,psi,rho);

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

  StartMenu("Ising Bilayer",1);

  SetActiveGraph(0);
  DefineGraph(contour2d_,"Visualize");

  //StartMenu("Controls",0);
  DefineInt("iterations", &iterations);
  DefineInt("repeat", &repeat);
  //DefineInt("stabilize", &stabilize);
  DefineDouble("phi_init", &r_zero[0]);
  DefineDouble("psi_init", &r_zero[1]);
  DefineDouble("Temperature", &T);
  DefineDouble("Lambda", &lambda);
  //EndMenu();

  StartMenu("Secondary Inputs",0);
  DefineInt("xdim", &xdim);
  DefineInt("ydim", &ydim);
  EndMenu();

  DefineFunction("reinitialize",&Init);

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
    printf("Running PARAM = %f on %dx%d Lattice with %d iterations\n",PARAM,XMAX,YMAX,iteration_max);

  if (!graphics) Pause = 0;
  
  Init(); 

   //test for speed
  BEGIN_T = time(NULL);
  for (n=0;n<10;n++)
    iteration1();

  if (!graphics)
    printf("sec/iteration: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)/10) );

  Init();

  //Just reset
  sprintf(name,"data-%f.txt",PARAM);
  out = fopen(name,"w+");

 //Write some information
  gethostname(hname,(size_t)sizeof(hname));

  fprintf(out,"#Size: %dx%d\n#PARAM: %f\n#itertions: %d\n#Repeat: %d\n#sec/iteration: %s\n#Host: %s\n#PID: %d\n#---BEGIN DATA---\n\n\n",XMAX,YMAX,PARAM,iteration_max,repeat,wtime_string( (double)(time(NULL)-BEGIN_T)/10 ), hname, getpid() );

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
      //analysis();
      
      if (!graphics)
	repeat *= 1.1;

      if (!graphics)
	printf("ETA: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)*(iteration_max-iterations)/iterations) );

      if (repeat==0) 
	Init();
    }
    else 
      sleep(1);

  }//End Main Loop

  return 0;
}

//2D Ising simulation of Lipid Membrane for Coserved Order Parameter
//It is based on Kawasaki spin-exchange algorithm
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
//Spin arrays
double phi[XMAX][YMAX]; 
double psi[XMAX][YMAX];
double rho[XMAX][YMAX]; //Total Spin

//Spin Coordinate List
int nup1=0, nup2=0, ndown1=0, ndown2=0;
int spinup1[NMAX][2];
int spindown1[NMAX][2];
int spinup2[NMAX][2];
int spindown2[NMAX][2];

//Boltzmann Weigth
double w[17][3];

void GenerateSpin(double spin[XMAX][YMAX], int spinup[NMAX][2], int spindown[NMAX][2], int *nup, int *ndown, double ratio) {
  int i, j;
  *nup = 0;
  *ndown = 0;
  //Set initial spin configuration
  for (i=0; i<xdim; i++) {
    for (j=0; j<ydim; j++) {
      spin[i][j] = rnd() <= ratio ? +1: -1;
      //Separate into Spin-type Lists
      if (spin[i][j] == 1){
	spinup[*nup][0] = i;
	spinup[*nup][1] = j;
	*nup += 1;
      }
      else{
	spindown[*ndown][0] = i;
	spindown[*ndown][1] = j;
	*ndown += 1;
      }
    }
  }
  printf("nup = %d, ndown = %d\n", *nup, *ndown);
}

void TotalSpin(double spin1[XMAX][YMAX], double spin2[XMAX][YMAX], double tot[XMAX][YMAX])
{
  int i, j;
  //Total spin
  for (i=0 ;i<xdim; i++) {
    for (j=0; j<ydim; j++) {  
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

  //Zero arrays (if neccessary)
  for (i=0; i<xdim; i++) {
    for (j=0; j<ydim; j++) {  
      phi[i][j] = 0; 
      psi[i][j] = 0; 
      rho[i][j] = 0;   
    }
  }

  //init spin
  GenerateSpin(phi, spinup1, spindown1, &nup1, &ndown1, r_zero[0]);
  GenerateSpin(psi, spinup2, spindown2, &nup2, &ndown2, r_zero[1]);

  TotalSpin(phi, psi, rho);

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

void Exchange(double spin1[XMAX][YMAX], double spin2[XMAX][YMAX], int spinup[NMAX][2], int spindown[NMAX][2], int nup, int ndown, int N, int L, double *E, double *M, double *accept)
{
  int spini, Ei, Ef, dE, sum_mn, sum_pq;
  double prob;
  //Check If Temperature
  OnTempChange();
  for (spini = 1; spini <= N; spini++)
    {
      /* randomly choose coordinates for trial spin  */
      const int iup = nup*rnd();
      const int idown = ndown*rnd(); //printf("iup = %d, idown = %d\n", iup, idown);
      const int m = spinup[iup][0];
      const int n = spinup[iup][1];
      const int p = spindown[idown][0];
      const int q = spindown[idown][1];
      /* search their neibors with periodic b.c. */
      const int mm  = (m+xdim-1)%xdim;
      const int mp  = (m+1)%xdim;
      const int nm  = (n+ydim-1)%ydim;
      const int np  = (n+1)%ydim;
      const int pm  = (p+xdim-1)%xdim;
      const int pp  = (p+1)%xdim;
      const int qm  = (q+ydim-1)%ydim;
      const int qp  = (q+1)%ydim;
      //printf("(m,n)::(%d,%d)->(%d,%d),(%d,%d),(%d,%d),(%d,%d)\n", m,n,mm,n,mp,n,m,nm,m,np);
      //printf("(p,q)::(%d,%d)->(%d,%d),(%d,%d),(%d,%d),(%d,%d)\n", p,q,pm,q,pp,q,p,qm,p,qp);
      /* calculate energy before exchange */
      sum_mn = spin1[mm][n]+spin1[mp][n]+spin1[m][nm]+spin1[m][np] +lambda*spin2[m][n];
      sum_pq = spin1[pm][q]+spin1[pp][q]+spin1[p][qm]+spin1[p][qp] +lambda*spin2[p][q];   
      Ei = -spin1[m][n]*sum_mn -spin1[p][q]*sum_pq;   
      /* Exchange spin */ 
      spin1[m][n] = -spin1[m][n]; 
      spin1[p][q] = -spin1[p][q];  
      //For testing ONLY
      //spin1[m][n] = 0;
      //spin1[p][q] = 0;
      /* Calculate energy after exchange */   
      Ef = -spin1[m][n]*sum_mn -spin1[p][q]*sum_pq;              
      dE = Ef - Ei; //printf("dE = %d\n", dE);

      // Exchange or Not ?  
      if ( dE <= 0 ){
	spindown[idown][0] = m;
	spindown[idown][1] = n;
	spinup[iup][0] = p;
	spinup[iup][1] = q;
      }
      else{
	prob = exp(-(double)dE/T); //printf("rnd() = %f, prob = %f\n", rnd(), prob);
	if ( rnd() <= prob){ // accept exchange
	  spindown[idown][0] = m;
	  spindown[idown][1] = n;
	  spinup[iup][0] = p;
	  spinup[iup][1] = q;
	  //printf("Accepted\n");
	}
	else{//decline exchange
	  spin1[m][n] = -spin1[m][n]; //restore at this site
	  spin1[p][q] = -spin1[p][q]; 
	  //printf("Rejected\n");
	}
      }// End exchange  
    }// End sweep
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

  Exchange(phi, psi, spinup1, spindown1, nup1, ndown1, NMAX, LMAX, &E, &M, &accept);
  Exchange(psi, phi, spinup2, spindown2, nup2, ndown2, NMAX, LMAX, &E, &M, &accept);
  TotalSpin(phi, psi, rho);

  //check_unstable();
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

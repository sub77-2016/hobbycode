//D2Q9 LB simulation of Monolayer with Friction
//Waipot Ngamsaad (waipotn@yahoo.com)
//12 Oct. 2008

/*
Index Legend D2Q9
.
.           y
.           |
.           4
.         7 | 6
.          \|/
.        1--0--2--x
.          /|\
.         5 | 8
.           3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include <mygraph.h>

//Maximum Simulation Size
#define XMAX (129)
#define YMAX (129)
#define PRANK  (2)
//Maximum History Size
#define HMAX (100000)
//Initial simulation params
#define XDIMINIT (100)
#define YDIMINIT (100)
#define BORDINIT (5)
#define REPEATINIT (100)
//Maximum random number size
#define RANDMAX (2147483647)
#define PI (3.14159265358979323846264338327)
//Searchparams
#define VELMAX  (0.010)
#define VELMIN  (0.002)
#define VELMLT  (2.0)
#define VELSTP  (0.001)

#define TMAX  (0.10)
#define TMIN1 (0.01)
#define TSTP1 (0.01)
#define TMIN2 (0.001)
#define TSTP2 (0.001)

#define fourOnine (4./9.)
#define oneOnine  (1./9.)
#define oneOthirtysix (1./36.)

#ifndef Error
#define Error(Str) Fatal_error(Str)
#define Fatal_error(Str) fprintf(stderr,"%s\n",Str), exit(1)
#endif

//Main inputs
double Amp1 = 1E-4;
double a_ = -1., b_ = 1., kappa_  = 0.9, Nu2 = 0.;
double tau0 = 1., tau1 = 1.;
double UX1 = 0.0, UY1 = 0.0;
double r_zero[PRANK] = {0.,1.};

//Secondary inputs
int x_zero = 0;
int xdim = XMAX;
int ydim = YMAX;
int hdim = 1000;
int border = BORDINIT;

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

//Set init
int setinit = 0;
int initvert = 10;

//Global Variables
double f[9][XMAX][YMAX][PRANK];

double rho[XMAX][YMAX];

double u[XMAX][YMAX][2];
double u_g[XMAX][YMAX][2];

double phi[XMAX][YMAX];
double phi_dx2[XMAX][YMAX];

double mu[XMAX][YMAX];

double timepassed[HMAX];
double totalmass[HMAX];
double domainwidth_vert[HMAX];
double domainwidth_vert_SD[HMAX];
double domainwidth_horiz[HMAX];
double domainwidth_horiz_SD[HMAX];

void write_file() {
  int i,j;
  char name[256];

  FILE *out;

  sprintf(name,"image/image-%f-%08i.txt",Nu2,iterations);
  out = fopen(name,"w+");

  //if (out==NULL) Error("\n Error: Cannot open ./image!!! \n");

  for (i=0;i<XMAX;i++){
    for (j=0;j<YMAX;j++){
      fprintf(out,"%f ",phi[i][j]);
    }
    fprintf(out,"\n");
  }
  fclose(out);
  
}

void mean_v(double L) {
  double ft1=0.0, ft2=0.0;
  int i,j;

  char name[256];
  FILE *out;

  sprintf(name,"bottom_V-%f.txt", Nu2);
  out = fopen(name,"a+");

  for (i=0;i<XMAX;i++){
    for (j=0;j<YMAX;j++){
      //const double r = rho[i][j];
      ft1 +=      (u_g[i][j][0])*(u_g[i][j][0])+(u_g[i][j][1])*(u_g[i][j][1]);
      ft2 += sqrt((u_g[i][j][0])*(u_g[i][j][0])+(u_g[i][j][1])*(u_g[i][j][1]));
    }
  }

  fprintf(out,"%d %f %.12f %.12f\n", iterations, L, sqrt(ft1/(double)(XMAX*YMAX)), ft2/(double)(XMAX*YMAX) );
  fclose(out);
}

void length_scale(void) {
  double cut=0.0, lengtht, length1, length2, tet, lengthA, lengthB;
  int Nt, no1, no2;

  char name[256];

  FILE *out;

  sprintf(name,"length_scale-%f.txt", Nu2);
  out = fopen(name,"a+");

  GetContourData(&(phi[0][0]),XMAX,YMAX,cut,&Nt,&no1,&no2,&lengtht,&length1,&length2,&tet,&lengthA,&lengthB);
 
  if ( lengtht!=0 ) 
    fprintf(out,"%d %f\n", iterations, (double)(XMAX*YMAX)/lengtht);
 
  if ( lengtht!=0 ) 
    mean_v((double)XMAX*YMAX/lengtht);

  //write_file();

  fclose(out);
}

void init(void) {
  int i, j, n;
  double r, p;

  char name[256];
  FILE *out;

  //Zero arrays
  for (i=0;i<XMAX;i++) {
    for (j=0;j<YMAX;j++) {
      
      u[i][j][0] = UX1;
      u[i][j][1] = UY1;
      
      u_g[i][j][0] = 0.;
      u_g[i][j][1] = 0.;
      
      rho[i][j] = 0.;
   
      phi[i][j] = 0.;
      phi_dx2[i][j] = 0.;      
      
      mu[i][j] = 0.; 
    }
  }

  for (n=0;n<HMAX;n++){
    timepassed[n] = 0.0;
    totalmass[n] = 0.0;
    domainwidth_vert[n] = 0.0;
    domainwidth_vert_SD[n] = 0.0;
    domainwidth_horiz[n] = 0.0;
    domainwidth_horiz_SD[n] = 0.0;
  }

  //Set initial moments
  for (i=0;i<XMAX;i++) {
    for (j=0;j<YMAX;j++) {

      phi[i][j] = r_zero[0] +Amp1*(1.*rand()/RANDMAX -0.5);
      rho[i][j] = r_zero[1] +Amp1*(1.*rand()/RANDMAX -0.5);	
 
      p = phi[i][j];
      r = rho[i][j];
 	    
      //Init PDF.
      //For Phi     
      f[0][i][j][0] = (2./3.)*p;
	    
      f[2][i][j][0] = (1./24.)*p;
      f[1][i][j][0] = (1./24.)*p;
      f[4][i][j][0] = (1./24.)*p;
      f[3][i][j][0] = (1./24.)*p;
	
      f[6][i][j][0] = (1./24.)*p;
      f[7][i][j][0] = (1./24.)*p;
      f[5][i][j][0] = (1./24.)*p;
      f[8][i][j][0] = (1./24.)*p;

      //For Rho     
      f[0][i][j][1] = (2./3.)*r;
	    
      f[2][i][j][1] = (1./24.)*r;
      f[1][i][j][1] = (1./24.)*r;
      f[4][i][j][1] = (1./24.)*r;
      f[3][i][j][1] = (1./24.)*r;
	
      f[6][i][j][1] = (1./24.)*r;
      f[7][i][j][1] = (1./24.)*r;
      f[5][i][j][1] = (1./24.)*r;
      f[8][i][j][1] = (1./24.)*r;

    }
  }//END INIT PDF

  for (n=0;n<HMAX;n++) {
    timepassed[n] = 0.0;
    totalmass[n] = 0.0;
    domainwidth_vert[n] = 0.0;
    domainwidth_vert_SD[n] = 0.0;
    domainwidth_horiz[n] = 0.0;
    domainwidth_horiz_SD[n] = 0.0;
  }

  //Reset book-keeping
  if (repeat==0) 
    repeat = saverepeat;
  
  iterations = 0;
  //x_zero = 0;

  sprintf(name,"bottom_V-%f.txt", Nu2);
  out = fopen(name,"w+");

  sprintf(name,"length_scale-%f.txt", Nu2);
  out = fopen(name,"w+");

  fclose(out);

}

void collision1(void) {
  int i, im, ip, j, jm, jp;

  double r, p, dxp, dyp, ddp;
  double ux, uy, uxx, uyy, uxy, usq, Txx, Tyy, Txy, pI, Mtt;
  double Fx, Fy, Fxx, Fyy, Fxy, Fsq;

  //double ux_g, uy_g;

  const double ohm0 = 1./tau0;
  const double ohm1 = 1./tau1;

  const double a = a_*s;
  const double b = b_*s;
  const double kappa = kappa_*s;
  
  //Compute Macroscopic Vars.
  for (i=0;i<XMAX;i++) {
    for (j=0;j<YMAX;j++) {
      phi[i][j] = f[0][i][j][0]+f[1][i][j][0]+f[2][i][j][0]+f[3][i][j][0]+f[4][i][j][0]+f[5][i][j][0]+f[6][i][j][0]+f[7][i][j][0]+f[8][i][j][0];
      
      rho[i][j] = f[0][i][j][1]+f[1][i][j][1]+f[2][i][j][1]+f[3][i][j][1]+f[4][i][j][1]+f[5][i][j][1]+f[6][i][j][1]+f[7][i][j][1]+f[8][i][j][1];
    }
  }

  //Collision main loop
  for (i=0;i<XMAX;i++) {
    //Periodic BC. in X-dir
    im  = (i+XMAX-1)%XMAX;
    ip  = (i+1)%XMAX;

    for (j=0;j<YMAX;j++) {
      //Periodic BC. in Y-dir
      jm  = (j+YMAX-1)%YMAX;
      jp  = (j+1)%YMAX;

      //Density
      r = rho[i][j];  //rho 
      p = phi[i][j];  //phi

      //Compute velocity fields
      const double ux_t = (f[2][i][j][1]-f[1][i][j][1]+f[6][i][j][1]-f[5][i][j][1]+f[8][i][j][1]-f[7][i][j][1])/r;
      const double uy_t = (f[4][i][j][1]-f[3][i][j][1]+f[6][i][j][1]-f[5][i][j][1]+f[7][i][j][1]-f[8][i][j][1])/r;
      //For visualized
      u[i][j][0] = ux_t; u[i][j][1] = uy_t;

      //Compute Gradient
      dxp = (1./2.)*(phi[ip][j]-phi[im][j]);
      dyp = (1./2.)*(phi[i][jp]-phi[i][jm]);

      //Compute Lapacian
      ddp = phi[im][j]+phi[ip][j]+phi[i][jm]+phi[i][jp]-4.*phi[i][j];   
      //For visualized
      phi_dx2[i][j] = ddp; 

      //Compute Pressure Tensor
      //Pressure Top
      pI  = 0.5*a*p*p +0.75*b*p*p*p*p -kappa*( p*ddp +0.5*(dxp*dxp+dyp*dyp) ) +r/3.;
      
      Txx = pI +kappa*dxp*dxp;
      Tyy = pI +kappa*dyp*dyp;
      Txy =     kappa*dxp*dyp;

      //Compute Chemical Potential
      Mtt = a*p +b*p*p*p -kappa*ddp;
      //For visualized
      mu[i][j] = Mtt;
      //Pre-factor for mu
      Mtt *= r;

      //Compute friction force
      const double etas = Nu2/(1.+Nu2/(2.*r)); 
      const double FX_f = -etas*ux_t;
      const double FY_f = -etas*uy_t; 

      //For visualized 
      u_g[i][j][0] = ux_t +0.5*FX_f/r; 
      u_g[i][j][1] = uy_t +0.5*FY_f/r;

      //ux_g = u_g[i][j][0];
      //uy_g = u_g[i][j][1];

      //*** Update for Top layer ***
      //Update PDF for Phi 
      Fx = p*FX_f/r;
      Fy = p*FY_f/r;

      ux  = ux_t; uy = uy_t;

      uxx = p*ux*ux +Mtt;
      uyy = p*uy*uy +Mtt;
      uxy = 2.*p*ux*uy;
      usq = uxx+uyy;

      Fxx = 2.*Fx*ux;
      Fyy = 2.*Fy*uy;
      Fxy = 2.*(Fx*uy+Fy*ux);
      Fsq = Fxx+Fyy;

      ux *= p;
      uy *= p;

      f[0][i][j][0] += ohm0*(fourOnine*((9./4.)*p -1.5*usq) -f[0][i][j][0])
	-fourOnine*1.5*Fsq;

      f[2][i][j][0] += ohm0*(oneOnine*( +3.*ux +4.5*uxx -1.5*usq) -f[2][i][j][0])
	+oneOnine*( 3.*Fx +4.5*Fxx -1.5*Fsq);
      f[1][i][j][0] += ohm0*(oneOnine*( -3.*ux +4.5*uxx -1.5*usq) -f[1][i][j][0])
	+oneOnine*(-3.*Fx +4.5*Fxx -1.5*Fsq);
      f[4][i][j][0] += ohm0*(oneOnine*( +3.*uy +4.5*uyy -1.5*usq) -f[4][i][j][0])
	+oneOnine*( 3.*Fy +4.5*Fyy -1.5*Fsq);
      f[3][i][j][0] += ohm0*(oneOnine*( -3.*uy +4.5*uyy -1.5*usq) -f[3][i][j][0])
	+oneOnine*(-3.*Fy +4.5*Fyy -1.5*Fsq);

      f[6][i][j][0] += ohm0*(oneOthirtysix*( +3.*(ux+uy) +4.5*(uxx+uxy+uyy)  -1.5*usq) -f[6][i][j][0])
	+oneOthirtysix*(3.*(Fx+Fy)  +4.5*(Fxx+Fxy+Fyy) -1.5*Fsq);
      f[7][i][j][0] += ohm0*(oneOthirtysix*( +3.*(-ux+uy) +4.5*(uxx-uxy+uyy) -1.5*usq) -f[7][i][j][0])
	+oneOthirtysix*(3.*(-Fx+Fy) +4.5*(Fxx-Fxy+Fyy) -1.5*Fsq);
      f[5][i][j][0] += ohm0*(oneOthirtysix*( +3.*(-ux-uy) +4.5*(uxx+uxy+uyy) -1.5*usq) -f[5][i][j][0])
	+oneOthirtysix*(3.*(-Fx-Fy) +4.5*(Fxx+Fxy+Fyy) -1.5*Fsq);
      f[8][i][j][0] += ohm0*(oneOthirtysix*( +3.*(ux-uy) +4.5*(uxx-uxy+uyy)  -1.5*usq) -f[8][i][j][0])
	+oneOthirtysix*(3.*(Fx-Fy)  +4.5*(Fxx-Fxy+Fyy) -1.5*Fsq);

      //Update PDF for Rho Top
      Fx = FX_f;
      Fy = FY_f;
 
      ux  = ux_t; uy = uy_t;

      uxx = r*ux*ux +Txx;
      uyy = r*uy*uy +Tyy;
      uxy = 2.*r*ux*uy +2.*Txy;
      usq = uxx+uyy; 

      Fxx = 2.*Fx*ux;
      Fyy = 2.*Fy*uy;
      Fxy = 2.*(Fx*uy+Fy*ux);
      Fsq = Fxx+Fyy;

      ux *= r;
      uy *= r;

      f[0][i][j][1] += ohm1*(fourOnine*((9./4.)*r -1.5*usq) -f[0][i][j][1])
	-fourOnine*1.5*Fsq;

      f[2][i][j][1] += ohm1*(oneOnine*( +3.*ux +4.5*uxx -1.5*usq) -f[2][i][j][1])
	+oneOnine*( 3.*Fx +4.5*Fxx -1.5*Fsq);
      f[1][i][j][1] += ohm1*(oneOnine*( -3.*ux +4.5*uxx -1.5*usq) -f[1][i][j][1])
	+oneOnine*(-3.*Fx +4.5*Fxx -1.5*Fsq);
      f[4][i][j][1] += ohm1*(oneOnine*( +3.*uy +4.5*uyy -1.5*usq) -f[4][i][j][1])
	+oneOnine*( 3.*Fy +4.5*Fyy -1.5*Fsq);
      f[3][i][j][1] += ohm1*(oneOnine*( -3.*uy +4.5*uyy -1.5*usq) -f[3][i][j][1])
	+oneOnine*(-3.*Fy +4.5*Fyy -1.5*Fsq);

      f[6][i][j][1] += ohm1*(oneOthirtysix*( +3.*(ux+uy) +4.5*(uxx+uxy+uyy)  -1.5*usq) -f[6][i][j][1])
	+oneOthirtysix*(3.*(Fx+Fy)  +4.5*(Fxx+Fxy+Fyy) -1.5*Fsq);
      f[7][i][j][1] += ohm1*(oneOthirtysix*( +3.*(-ux+uy) +4.5*(uxx-uxy+uyy) -1.5*usq) -f[7][i][j][1])
	+oneOthirtysix*(3.*(-Fx+Fy) +4.5*(Fxx-Fxy+Fyy) -1.5*Fsq);
      f[5][i][j][1] += ohm1*(oneOthirtysix*( +3.*(-ux-uy) +4.5*(uxx+uxy+uyy) -1.5*usq) -f[5][i][j][1])
	+oneOthirtysix*(3.*(-Fx-Fy) +4.5*(Fxx+Fxy+Fyy) -1.5*Fsq);
      f[8][i][j][1] += ohm1*(oneOthirtysix*( +3.*(ux-uy) +4.5*(uxx-uxy+uyy)  -1.5*usq) -f[8][i][j][1])
	+oneOthirtysix*(3.*(Fx-Fy)  +4.5*(Fxx-Fxy+Fyy) -1.5*Fsq);

    }
  }//End Collision Loop
}

void streaming1(void) {
  int i,k;
  double outx1[PRANK*YMAX], outx2[PRANK*YMAX], outy3[PRANK*XMAX], outy4[PRANK*XMAX];
  double outx5[PRANK*(YMAX-1)], outx6[PRANK*(YMAX-1)], outx7[PRANK*(YMAX-1)], outx8[PRANK*(YMAX-1)];
  double outy5[PRANK*(XMAX-1)], outy6[PRANK*(XMAX-1)], outy7[PRANK*(XMAX-1)], outy8[PRANK*(XMAX-1)];
  double corner5[PRANK], corner6[PRANK], corner7[PRANK], corner8[PRANK];

  //Save outgoing faces
  memcpy(&outx1[0], &f[1][0][0][0], YMAX*PRANK*sizeof(double));
  memcpy(&outx2[0], &f[2][XMAX-1][0][0], YMAX*PRANK*sizeof(double));

  for (i=0,k=0;i<PRANK*XMAX;i+=PRANK) {
    memcpy(&outy3[i], &f[3][k][0][0], PRANK*sizeof(double));
    memcpy(&outy4[i], &f[4][k][YMAX-1][0], PRANK*sizeof(double));
    k++;
  }

  memcpy(&outx5[0], &f[5][0][1][0], (YMAX-1)*PRANK*sizeof(double));
  memcpy(&outx7[0], &f[7][0][0][0], (YMAX-1)*PRANK*sizeof(double));

  memcpy(&outx6[0], &f[6][XMAX-1][0][0], (YMAX-1)*PRANK*sizeof(double));
  memcpy(&outx8[0], &f[8][XMAX-1][1][0], (YMAX-1)*PRANK*sizeof(double));

  for (i=0,k=0;i<PRANK*(XMAX-1);i+=PRANK) {
    memcpy(&outy5[i], &f[5][k+1][0][0], PRANK*sizeof(double));
    memcpy(&outy7[i], &f[7][k+1][YMAX-1][0], PRANK*sizeof(double));
    k++;
  }

  for (i=PRANK*(XMAX-2),k=XMAX-2;i>=0;i-=PRANK) {
    memcpy(&outy6[i], &f[6][k][YMAX-1][0], PRANK*sizeof(double));
    memcpy(&outy8[i], &f[8][k][0][0], PRANK*sizeof(double));
    k--;
  }

  memcpy(&corner5[0], &f[5][0][0][0], PRANK*sizeof(double));
  memcpy(&corner7[0], &f[7][0][YMAX-1][0], PRANK*sizeof(double));
  memcpy(&corner6[0], &f[6][XMAX-1][YMAX-1][0], PRANK*sizeof(double));
  memcpy(&corner8[0], &f[8][XMAX-1][0][0], PRANK*sizeof(double));
  
  //Streaming Inner PDF.
  memmove(&f[1][0][0][0], &f[1][1][0][0], (XMAX - 1)*YMAX*PRANK*sizeof(double));
  memmove(&f[2][1][0][0], &f[2][0][0][0], (XMAX - 1)*YMAX*PRANK*sizeof(double));
  memmove(&f[3][0][0][0], &f[3][0][1][0], (YMAX*XMAX - 1)*PRANK*sizeof(double));
  memmove(&f[4][0][1][0], &f[4][0][0][0], (YMAX*XMAX - 1)*PRANK*sizeof(double));

  memmove(&f[5][0][0][0], &f[5][1][1][0], ((XMAX - 1)*YMAX - 1)*PRANK*sizeof(double));
  memmove(&f[7][0][1][0], &f[7][1][0][0], ((XMAX - 1)*YMAX - 1)*PRANK*sizeof(double));
  memmove(&f[6][1][1][0], &f[6][0][0][0], ((XMAX - 1)*YMAX - 1)*PRANK*sizeof(double));  
  memmove(&f[8][1][0][0], &f[8][0][1][0], ((XMAX - 1)*YMAX - 1)*PRANK*sizeof(double));

  //Wrap Periodic BC.
  memcpy(&f[2][0][0][0], &outx2[0], YMAX*PRANK*sizeof(double));  //mass conservation
  memcpy(&f[1][XMAX-1][0][0], &outx1[0], YMAX*PRANK*sizeof(double));  //mass conservation
	
  for (i=0,k=0;i<PRANK*XMAX;i+=PRANK) {
    memcpy(&f[4][k][0][0], &outy4[i], PRANK*sizeof(double));  //mass conservation
    memcpy(&f[3][k][YMAX-1][0], &outy3[i], PRANK*sizeof(double));  //mass conservation
    k++;
  }

  memcpy(&f[5][XMAX-1][0][0], &outx5[0], (YMAX-1)*PRANK*sizeof(double));
  memcpy(&f[7][XMAX-1][1][0], &outx7[0], (YMAX-1)*PRANK*sizeof(double));

  memcpy(&f[6][0][1][0], &outx6[0], (YMAX-1)*PRANK*sizeof(double));
  memcpy(&f[8][0][0][0], &outx8[0], (YMAX-1)*PRANK*sizeof(double));

  for (i=0,k=0;i<PRANK*(XMAX-1);i+=PRANK) {
    memcpy(&f[5][k][YMAX-1][0], &outy5[i], PRANK*sizeof(double));
    memcpy(&f[7][k][0][0], &outy7[i], PRANK*sizeof(double));
    k++;
  }

  for (i=PRANK*(XMAX-2),k=XMAX-2;i>=0;i-=PRANK) {
    memcpy(&f[6][k+1][0][0], &outy6[i], PRANK*sizeof(double));
    memcpy(&f[8][k+1][YMAX-1][0], &outy8[i], PRANK*sizeof(double));
    k--;
  }

  //Corners
  memcpy(&f[5][XMAX-1][YMAX-1][0], &corner5[0], PRANK*sizeof(double));
  memcpy(&f[7][XMAX-1][0][0], &corner7[0], PRANK*sizeof(double));
  memcpy(&f[6][0][0][0], &corner6[0], PRANK*sizeof(double));
  memcpy(&f[8][0][YMAX-1][0], &corner8[0], PRANK*sizeof(double));
}

void check_unstable(void) {
  int c = isnormal(rho[XMAX/2][YMAX/2])&&isnormal(phi[XMAX/2][YMAX/2]);
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

void iteration1(void){
  collision1();
  streaming1();
  
  check_unstable();
}

void analysis(void){
  length_scale();
}

//Graphics
void GetGraphics(void){
}

void GUI(void){	
  DefineGraphNxN_R("rho", &rho[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_R("phi", &phi[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_R("mu", &mu[0][0], &xdim, &ydim, NULL);
  //DefineGraphNxN_R("phi_dx4(1)", &phi_dx4[0][0], &xdim, &ydim, NULL);
  //DefineGraphNxN_R("phi_dx4(2)", &phi2_dx4[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_RxR("u",&u_g[0][0][0],&xdim,&ydim,NULL);
  NewGraph();

  StartMenu("D2Q9 LB Monolayer",1);

  StartMenu("Main Inputs",0);
  DefineDouble("phi_zero",&r_zero[0]);
  DefineDouble("rho_zero",&r_zero[1]);
  DefineDouble("Amp",&Amp1);
  DefineDouble("a",&a_);
  DefineDouble("b",&b_);
  DefineDouble("kappa",&kappa_);
  DefineDouble("Nu2",&Nu2);
  DefineDouble("tau0",&tau0);
  DefineDouble("tau1",&tau1);
  DefineDouble("UX_tp",&UX1);
  DefineDouble("UY_tp",&UY1);
  EndMenu();

  StartMenu("Secondary Inputs",0);
  DefineInt("xdim", &xdim);
  DefineInt("ydim", &ydim);
  EndMenu();

  StartMenu("Stability Factors",0);
  DefineDouble("s",&s);
  EndMenu();

  StartMenu("Controls",0);
  DefineInt("iterations", &iterations);
  DefineInt("repeat", &repeat);
  DefineInt("stabilize", &stabilize);
  EndMenu();

  StartMenu("Set Initializations",0);
  DefineBool("Use Set Init", &setinit);
  DefineInt("Num Vert Domains", &initvert);
  EndMenu();

  SetActiveGraph(0);
  DefineGraph(contour2d_,"Visualize");
     
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
  if(argc < 2) {
    Error("Usage: mono2-noX eta_s [GUI 1 or 0]\n");
  }
  else {
    Nu2 = atof(argv[1]);
    
    if (argc == 3)
      graphics = atoi(argv[2]);
  }
  
  if (graphics)
    GUI();

  if (!graphics)
    printf("Running Nu = %f on %dx%d Lattice with %d iterations\n",Nu2,XMAX,YMAX,iteration_max);

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
  sprintf(name,"bottom_V-%f.txt", Nu2);
  out = fopen(name,"w+");

  sprintf(name,"length_scale-%f.txt", Nu2);
  out = fopen(name,"w+");

  //Write some information
  gethostname(hname,sizeof(hname));

  fprintf(out,"#Size: %dx%d\n#K: %f\n#Nu2: %f\n#itertions: %d\n#Repeat: %d\n#sec/iteration: %s\n#Host: %s\n#PID: %d\n#---BEGIN DATA---\n\n\n",XMAX,YMAX,kappa_,Nu2,iteration_max,repeat,wtime_string( (double)(time(NULL)-BEGIN_T)/10 ), hname, getpid() );

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
      
      if (!graphics)
	repeat *= 1.1;

      if (!graphics)
	printf("ETA: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)*(iteration_max-iterations)/iterations) );

      if (repeat==0) 
	init();

      length_scale();
    }
    else 
      sleep(1);

  }//End Main Loop

  return 0;
}

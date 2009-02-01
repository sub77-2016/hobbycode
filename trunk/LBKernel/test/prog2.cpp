extern "C" {
#include <stdio.h>
#include <math.h>
#include <mygraph.h>
}

#define SIZE 100
static int size=SIZE,Repeat=1000,done=0,sstep=0,pause=1;
static double f0[SIZE],f1[SIZE],f2[SIZE], omega=1, T=0.3,Amplitude=1;
static double density[SIZE];
static int densityreq=0;

int xt = 3, yt = 3;
double test[3][3] = {{1,2,3},{4,5,6},{7,8,9}};


void init(){
  int i;
  for (i=0;i<size;i++){
    density[i]=(2+Amplitude*sin(2*M_PI*i/size));
    f0[i]=density[i]*(1-T);
    f1[i]=density[i]*T*0.5;
    f2[i]=density[i]*T*0.5;
  }
}

void init2(){
  int i;
  for (i=0;i<size;i++){
    if (2*i>=size) density[i]=2+Amplitude; else density[i]=2-Amplitude;
    f0[i]=density[i]*(1-T);
    f1[i]=density[i]*T*0.5;
    f2[i]=density[i]*T*0.5;
  }
}

void iterate(){
  int i;
  double tmp1,tmp2;
  for (i=0;i<size;i++){
    density[i]=f0[i]+f1[i]+f2[i];
    f0[i]+=omega*(density[i]*(1-T)-f0[i]);
    f1[i]+=omega*(density[i]*T*0.5-f1[i]);
    f2[i]+=omega*(density[i]*T*0.5-f2[i]);
  }
  tmp1=f1[size-1];
  tmp2=f2[0];
  for (i=1;i<size;i++){
    f1[size-i]=f1[size-i-1];
    f2[i-1]=f2[i];
  }
  f1[0]=tmp1;
  f2[size-1]=tmp2;
}


void GUI(){
  DefineGraphNxN_R("test", &(test[0][0]), &xt, &yt, 0);
  //DefineGraphN_R("Density --",density,&size,&densityreq);
  StartMenu("GUI",1);
    DefineDouble("T",&T);
    DefineDouble("omega",&omega);
    StartMenu("Restart",0);
      DefineMod("size",&size,SIZE+1);
      DefineDouble("Amplitude",&Amplitude);
      DefineFunction("Restart sin",&init);
      DefineFunction("Restart step",&init2);
    EndMenu();
    //DefineGraph(curve2d_,"Density graph");
    DefineGraph(contour2d_,"Density graph");
    DefineBool("Pause",&pause);
    DefineBool("Single step",&sstep);
    DefineInt("Repeat",&Repeat);    DefineBool("Done",&done);
  EndMenu();
}

int main(){
  int i;
  init();
  GUI();
  while (!done){
    Events(1); /* Whenever there are new data the argument of 
		  Events() should be nonzero. This will set the
		  requests for data so that you can calculate them
		  on demand only. For this simple program you can
		  always set it to one. */
    DrawGraphs();
    if (!pause || sstep){
      sstep=0;
      for (i=0;i<Repeat;i++) iterate();
    } else {
      //sleep(1);
/*when the program is waiting it returns the 
		 CPU time to the operating system */
    }
  }
  return 0;
}

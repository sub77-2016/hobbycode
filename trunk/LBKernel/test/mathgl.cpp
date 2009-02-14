#include <mgl/mgl_fltk.h>

int N = 1;
const float dt = 1./(float)N;
float t = 0;

void resample(int, void *)
{
	t += dt;
}

int sample(mglGraph *gr, void *)
{
	//int N = 100;
	//const float dt = 1./(float)N;

	mglData y(50);

	//for (int k = 0; k < N; k++)
	//{
		//t += dt;

    		for(int i=0;i<50;i++)   
			y.a[i] = sin(M_PI*i/24. + 0.5 * t);

		//gr->NewFrame();
        	gr->Box();
		gr->Plot(y);
		//gr->EndFrame();
	//}

        //return gr->GetNumFrame();
	return N;
}

int main(int argc,char **argv)
{
        mglGraphFLTK gr;
        gr.Window(argc,argv,sample,"MathGL examples",NULL,resample);
        return mglFlRun();
}


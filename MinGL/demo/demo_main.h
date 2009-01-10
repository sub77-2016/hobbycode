#include "../src/SDLViewer.h"

/*********************Static Libraries***********************/
#ifdef WIN32

// This is a compiler directive that includes libraries (For Visual Studio)
//#pragma comment(lib, "odbc32.lib")
//#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "wsock32.lib")
//#pragma comment(lib, "zlibd.lib")
//#pragma comment(lib, "regexd.lib")
//#pragma comment(lib, "pngd.lib")
//#pragma comment(lib, "jpegd.lib")
//#pragma comment(lib, "tiffd.lib")

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glut32.lib")

#endif

// 1. test viewer
void test_viewer(void);

// 2. test simulator
void test_sim(void);

// 3. test simulator
void test_solid(void);


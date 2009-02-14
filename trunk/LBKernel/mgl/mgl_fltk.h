/* mgl_fltk.h is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//-----------------------------------------------------------------------------
#ifndef _MGL_FLTK_H_
#define _MGL_FLTK_H_
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
# define FL_DLL
#endif

#include <FL/Fl.H>
#include <Fl/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Menu_Bar.H>
#include "mgl/mgl_zb.h"
//-----------------------------------------------------------------------------
class Fl_MathGL;
int mglFlRun();
//-----------------------------------------------------------------------------
/// Class allows the window creation for displaying plot bitmap with the help of FLTK library
/** ATTENTION!!! All frames are saved in memory. So animation with many frames require a lot memory and CPU time (for example, for mouse rotation).*/
class mglGraphFLTK : public mglGraphZB
{
friend class Fl_MathGL;
public:
using mglGraphAB::Window;
	int sshow;		///< Current state of animation switch (toggle button)
	Fl_MathGL *FMGL;///< Control which draw graphics
	int CurFig;		///< Current figure in the list.
	Fl_Window *Wnd;	///< Pointer to window

	mglGraphFLTK();
	~mglGraphFLTK();

	void SetSize(int w,int h);
	void EndFrame();
	const unsigned char *GetBits();
	void Clf(mglColor Back=NC);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Служебные ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/// Create a window for plotting. Now implemeted only for GLUT.
	void Window(int argc, char **argv, int (*draw)(mglGraph *gr, void *p),
						const char *title,void *par=NULL,
	  					void (*reload)(int next, void *p)=NULL);
	/// Switch on/off transparency (do not overwrite switches in user drawing function)
	void ToggleAlpha();
	/// Switch on/off lighting (do not overwrite switches in user drawing function)
	void ToggleLight();
	void ToggleZoom();	///< Switch on/off zooming by mouse
	void ToggleRotate();///< Switch on/off rotation by mouse
	void ToggleNo();	///< Switch off all zooming and rotation
	void Update();		///< Update picture by calling user drawing function
	void ReLoad(bool o);///< Reload user data and update picture
	void Adjust();		///< Adjust size of bitmap to window size
	void NextFrame();	///< Show next frame (if one)
	void PrevFrame();	///< Show previous frame (if one)
	void Animation();	///< Run slideshow (animation) of frames

protected:
	unsigned char *GG;	///< images for all frames (may be too LARGE !!!)

	Fl_Button *alpha_bt, *light_bt, *rotate_bt, *zoom_bt, *anim_bt;
	Fl_Counter *tet, *phi;
	Fl_Scroll	*scroll;
	Fl_Menu_Bar	*menu;

	int alpha;		///< Current state of alpha switch (toggle button)
	int light;		///< Current state of light switch (toggle button)
	bool zoom, rotate;
};
//-----------------------------------------------------------------------------
/// Class is FLTK widget which display MathGL graphics
class Fl_MathGL : public Fl_Widget
{
public:
	Fl_Valuator	*tet_val;	///< pointer to external tet-angle validator
	Fl_Valuator	*phi_val;	///< pointer to external phi-angle validator
	mglGraphAB *graph;		///< pointer to grapher
	void *draw_par;			///< Parameters for drawing function mglGraph::DrawFunc.
	/// Drawing function for window procedure. It should return the number of frames.
	int (*draw_func)(mglGraph *gr, void *par);

	Fl_MathGL(int x, int y, int w, int h, char *label=0);
	~Fl_MathGL();

	/// Update (redraw) plot
	void update(mglGraph *gr=0);
	/// Set drawing functions from the class
	void set_draw(mglDraw *dr);
	/// Set angles for additional plot rotation
	inline void set_angle(float t, float p){	tet = t;	phi = p;	};
	/// Set bitwise flags for general state (1-Alpha, 2-Light)
	inline void set_state(int f)			{	flag = f;	};
	/// Set flags for handling mouse
	void set_state(bool z, bool r)	{	zoom = z;	rotate = r;	};
	/// Set zoom in/out region
	inline void set_zoom(float X1, float Y1, float X2, float Y2)
	{	x1 = X1;	x2 = X2;	y1 = Y1;	y2 = Y2;	};
	/// Get zoom region
	inline void get_zoom(float *X1, float *Y1, float *X2, float *Y2)
	{	*X1 = x1;	*X2 = x2;	*Y1 = y1;	*Y2 = y2;	};
	/// Get pointer to grapher
	inline mglGraph *get_graph()	{	return graph;	};
	/// Set popup menu pointer
	inline void set_popup(const Fl_Menu_Item *pmenu, Fl_Widget *w, void *v)
	{	popup = pmenu;	wpar = w;	vpar = v;	}
protected:
	const Fl_Menu_Item *popup;	///< pointer to popup menu items
	Fl_Widget *wpar;			///< widget for popup menu
	void *vpar;					///< parameter for popup menu
	float tet,phi;				///< rotation angles
	float x1,x2,y1,y2;			///< zoom in region
	bool zoom, rotate;			///< flag for handle mouse
	int flag;					///< bitwise flag for general state (1-Alpha, 2-Light)
	int x0,y0,xe,ye;			///< mouse position
	char pos[128];

	void draw();				///< quick drawing function
	int handle(int code);		///< handle mouse events
	void resize(int x, int y, int w, int h);	///< resize control
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------

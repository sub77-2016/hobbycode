/* mgl_fltk.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <mathgl.abalakin@gmail.com>
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
#ifdef USE_GETTEXT
	#include <libintl.h>
#else
	#define gettext(x)	(x)
#endif

#include <FL/Fl_Pixmap.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <unistd.h>
//#include <pthread.h>

#include "mgl/mgl_fltk.h"
#include "mgl/mgl_eps.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
#include "xpm/alpha_on.xpm"
#include "xpm/light_on.xpm"
#include "xpm/zoom_on.xpm"
#include "xpm/show_on.xpm"
#include "xpm/rotate_on.xpm"
#include "xpm/show_sl.xpm"
#include "xpm/next_sl.xpm"
#include "xpm/prev_sl.xpm"
#include "xpm/left_1.xpm"
#include "xpm/right_1.xpm"
#include "xpm/down_1.xpm"
#include "xpm/norm_1.xpm"
#include "xpm/zoom_1.xpm"
#include "xpm/up_1.xpm"
#include "xpm/alpha.xpm"
#include "xpm/light.xpm"
#include "xpm/zoom_in.xpm"
#include "xpm/zoom_out.xpm"
#include "xpm/rotate.xpm"
#include "xpm/ok.xpm"
//-----------------------------------------------------------------------------
Fl_Pixmap xpm_a1(alpha_xpm), xpm_a2(alpha_on_xpm);
Fl_Pixmap xpm_l1(light_on_xpm), xpm_l2(light_xpm);
Fl_Pixmap xpm_z1(zoom_in_xpm), xpm_z2(zoom_on_xpm);
Fl_Pixmap xpm_s1(show_sl_xpm), xpm_s2(show_on_xpm);
Fl_Pixmap xpm_r1(rotate_xpm), xpm_r2(rotate_on_xpm);
//-----------------------------------------------------------------------------
//
//		class Fl_MathGL
//
//-----------------------------------------------------------------------------
Fl_MathGL::Fl_MathGL(int x, int y, int w, int h, char *label) : Fl_Widget(x,y,w,h,label)
{
	graph = 0;
	tet=phi=x1=y1=0;	x2=y2=1;
	zoom = rotate = false;
	flag=x0=y0=xe=ye=0;
	tet_val = phi_val = 0;
	draw_par = 0;	draw_func = 0;
}
//-----------------------------------------------------------------------------
Fl_MathGL::~Fl_MathGL()	{}
//-----------------------------------------------------------------------------
int mgl_draw_class(mglGraph *gr, void *p);
void Fl_MathGL::set_draw(mglDraw *dr)
{	draw_func = mgl_draw_class;	draw_par = dr;	}
void Fl_MathGL::draw()
{
	if(zoom && x0!=xe && y0!=ye)
		fl_rect(x0<xe?x0:xe, y0<ye?y0:ye, abs(xe-x0), abs(ye-y0));
	const unsigned char *g = graph ? graph->GetBits() : 0;
	if(g)
		fl_draw_image(g, x(), y(), graph->GetWidth(), graph->GetHeight(), 3);
}
//-----------------------------------------------------------------------------
void Fl_MathGL::update(mglGraph *gr)
{
	if(gr==0)	gr=graph;
	if(gr==0)	return;
	gr->DefaultPlotParam();
	gr->Alpha(flag&1);
	gr->Light(flag&2);
	gr->View(tet,phi);
	gr->Zoom(x1,y1,x2,y2);
	gr->DrawFace = !rotate;

	gr->Message = new char[2048];	gr->Message[0] = 0;
	if(draw_func)	draw_func(gr, draw_par);
	if(gr->Message[0] != 0)			fl_message(gr->Message);
	delete []gr->Message;			gr->Message = 0;

	if(gr==graph && (graph->GetWidth()!=w() || graph->GetHeight()!=h()))
		size(graph->GetWidth(), graph->GetHeight());
	redraw();
}
//-----------------------------------------------------------------------------
void Fl_MathGL::resize(int x, int y, int w, int h)
{
	Fl_Widget::resize(x,y,w,h);
}
//-----------------------------------------------------------------------------
int Fl_MathGL::handle(int code)
{
	if(popup && code==FL_PUSH && Fl::event_button()==FL_RIGHT_MOUSE)
	{
		const Fl_Menu_Item *m = popup->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
		if(m)	m->do_callback(wpar, vpar);
	}
	if(graph->ShowMousePos && !zoom && !rotate && code==FL_PUSH && Fl::event_button()==FL_LEFT_MOUSE)
	{
		mglPoint p = graph->CalcXYZ(Fl::event_x()-x(), Fl::event_y()-y());
		char s[128];
		sprintf(s,"x=%g, y=%g, z=%g",p.x,p.y,p.z);
		draw();	fl_color(FL_BLACK);		fl_draw(s,40,70);
	}
	if((!zoom && !rotate) || Fl::event_button()!=FL_LEFT_MOUSE)
	{
		if(code==FL_FOCUS || code==FL_UNFOCUS)	return 1;
		if(code==FL_KEYUP)
		{
			int key=Fl::event_key();
			if(!strchr(" .,wasdrfx",key))	return 0;
			if(key==' ')
			{
				update();	return 1;
			}
			if(key=='w')
			{
				tet += 10;
				if(tet_val)	tet_val->value(tet);
				update();	return 1;
			}
			if(key=='s')
			{
				tet -= 10;
				if(tet_val)	tet_val->value(tet);
				update();	return 1;
			}
			if(key=='a')
			{
				phi += 10;
				if(phi_val)	phi_val->value(phi);
				update();	return 1;
			}
			if(key=='d')
			{
				phi -= 10;
				if(phi_val)	phi_val->value(phi);
				update();	return 1;
			}
			if(key=='x')
			{
				mglGraphFLTK *g=dynamic_cast<mglGraphFLTK *>(graph);
				if(g && g->FMGL==this)
				{	g->Wnd->hide();	return 1;	}
				else	return 0;
//				exit(0);
			}
			if(key==',')
			{
				mglGraphFLTK *g=dynamic_cast<mglGraphFLTK *>(graph);
				if(g && g->FMGL==this)
				{	g->PrevFrame();	return 1;	}
				else	return 0;
			}
			if(key=='.')
			{
				mglGraphFLTK *g=dynamic_cast<mglGraphFLTK *>(graph);
				if(g && g->FMGL==this)
				{	g->NextFrame();	return 1;	}
				else	return 0;
			}
			if(key=='r')
			{
				flag = (flag&2) + ((~(flag&1))&1);
				update();	return 1;
			}
			if(key=='f')
			{
				flag = (flag&1) + ((~(flag&2))&2);
				update();	return 1;
			}
		}
		return 0;
	}
	if(code==FL_PUSH)
	{
		xe=x0=Fl::event_x();	ye=y0=Fl::event_y();
	}
	if(code==FL_DRAG)
	{
		xe=Fl::event_x();	ye=Fl::event_y();
		float ff = 240/sqrt(float(w()*h()));
		if(rotate)
		{
			phi += int((x0-xe)*ff);
			tet += int((y0-ye)*ff);
			if(phi>180)	phi-=360;		if(phi<-180)	phi+=360;
			if(tet>180)	tet-=360;		if(tet<-180)	tet+=360;
			x0 = xe;	y0 = ye;
			update();
		}
		redraw();
	}
	if(code==FL_RELEASE)
	{
		if(zoom)
		{
			int w1=w(),h1=h();
			float _x1,_x2,_y1,_y2;
			_x1 = x1+(x2-x1)*(x0-x())/float(w1);
			_y1 = y2-(y2-y1)*(ye-y())/float(h1);
			_x2 = x1+(x2-x1)*(xe-x())/float(w1);
			_y2 = y2-(y2-y1)*(y0-y())/float(h1);
			x1=_x1;		x2=_x2;		y1=_y1;		y2=_y2;
			if(x1>x2)	{	_x1=x1;	x1=x2;	x2=_x1;	}
			if(y1>y2)	{	_x1=y1;	y1=y2;	y2=_x1;	}
			update();
		}
		else
		{
			if(tet_val)	tet_val->value(tet);
			if(phi_val)	phi_val->value(phi);
		}
		redraw();
	}
	return 1;
}
//-----------------------------------------------------------------------------
//
//		class mglGraphFLTK
//
//-----------------------------------------------------------------------------
mglGraphFLTK::mglGraphFLTK() : mglGraphZB()
{
	GG = 0;		Wnd = 0;
	NumFig = 0;	CurFig = -1;
	alpha = light = sshow = 0;
}
//-----------------------------------------------------------------------------
mglGraphFLTK::~mglGraphFLTK()
{	if(GG) free(GG);	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::SetSize(int w,int h)
{
	if(GG)	free(GG);	GG = 0;
	mglGraphZB::SetSize(w,h);
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::EndFrame()
{
	CurFig = CurFrameId-1;
	if(!GG)
	{
		GG = (unsigned char *)malloc(3*Width*Height);
		NumFig = 1;		CurFig = 0;
	}
	else if(CurFig>NumFig-1)
	{
		GG = (unsigned char *)realloc(GG,3*(NumFig+1)*Width*Height);
		NumFig++;
	}
	Finish();
	memcpy(GG + CurFig*Width*Height*3,G,3*Width*Height);
	CurFig++;
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::Clf(mglColor Back)
{
	if(AutoClf)	mglGraphZB::Clf(Back);
}
//-----------------------------------------------------------------------------
const unsigned char *mglGraphFLTK::GetBits()
{
	Finish();
	unsigned char *g = G;
	if(GG && NumFig>0 && CurFig<NumFig && CurFig>=0)
		g = GG + CurFig*Width*Height*3;
	return g;
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::NextFrame()
{
	if(NumFig>0)	{	CurFig = CurFig>NumFig-1 ? 0 : CurFig+1;	FMGL->redraw();	}
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::PrevFrame()
{
	if(NumFig>0)	{	CurFig = CurFig<0 ? NumFig-1 : CurFig-1;	FMGL->redraw();	}
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ToggleAlpha()
{
	alpha = 1-alpha;
	alpha_bt->value(alpha);
	alpha_bt->image(alpha?xpm_a2:xpm_a1);	alpha_bt->redraw();
	Fl_Menu_Item *m = (Fl_Menu_Item *)menu->find_item(gettext("Graphics/Alpha"));
	if(m && alpha)	m->set();
	if(m && !alpha)	m->clear();
	Update();
}
void alpha_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->ToggleAlpha();	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ToggleLight()
{
	light = 1-light;
	light_bt->value(light);
	light_bt->image(light?xpm_l2:xpm_l1);	light_bt->redraw();
	Fl_Menu_Item *m = (Fl_Menu_Item *)menu->find_item(gettext("Graphics/Light"));
	if(m && light)	m->set();
	if(m && !light)	m->clear();
	Update();
}
void light_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->ToggleLight();	}
//-----------------------------------------------------------------------------
void draw_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->Update();	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ToggleNo()
{
	zoom_bt->value(0);		zoom_bt->image(xpm_z1);		zoom_bt->redraw();
	rotate_bt->value(0);	rotate_bt->image(xpm_r1);	rotate_bt->redraw();

	tet->value(0);			phi->value(0);
	zoom = rotate = false;
	FMGL->set_zoom(0,0,1,1);
	Update();
}
void norm_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->ToggleNo();	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ToggleZoom()
{
	rotate_bt->value(0);	rotate_bt->image(xpm_r1);	rotate_bt->redraw();
	FMGL->set_state(zoom_bt->value(), false);
	zoom_bt->image(zoom_bt->value()?xpm_z2:xpm_z1);
	zoom_bt->redraw();
}
void zoom_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->ToggleZoom();	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ToggleRotate()
{
	zoom_bt->value(0);	zoom_bt->image(xpm_z1);	zoom_bt->redraw();
	FMGL->set_state(false, rotate_bt->value()!=0);
	rotate_bt->image(rotate_bt->value()?xpm_r2:xpm_r1);
	rotate_bt->redraw();
	if(!rotate_bt->value())	Update();
}
void rotate_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->ToggleRotate();	}
//-----------------------------------------------------------------------------
void mglGraphFLTK::Update()
{
	FMGL->set_state(zoom_bt->value(), rotate_bt->value());
	FMGL->set_state(alpha + 2*light);
	FMGL->set_angle(tet->value(), phi->value());
	CurFig=0;
	FMGL->update();
}
//-----------------------------------------------------------------------------
void export_png_cb(Fl_Widget*, void* v)
{
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.png", 0);
	if(!fname || !fname[0])	return;
	((mglGraphFLTK*)v)->WritePNG(fname);
}
//-----------------------------------------------------------------------------
void export_bps_cb(Fl_Widget*, void* v)
{
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.eps", 0);
	if(!fname || !fname[0])	return;
	((mglGraphFLTK*)v)->WriteEPS(fname);
}
//-----------------------------------------------------------------------------
void export_pngn_cb(Fl_Widget*, void* v)
{
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.png", 0);
	if(!fname || !fname[0])	return;
	((mglGraphFLTK*)v)->WritePNG(fname,0,false);
}
//-----------------------------------------------------------------------------
void export_jpeg_cb(Fl_Widget*, void* v)
{
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.jpg", 0);
	if(!fname || !fname[0])	return;
	((mglGraphFLTK*)v)->WriteJPEG(fname);
}
//-----------------------------------------------------------------------------
void export_svg_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.svg", 0);
	if(!fname || !fname[0])	return;
	mglGraphPS *ps = new mglGraphPS(e->GetWidth(), e->GetHeight());
	ps->SetFont(e->GetFont());
	e->FMGL->update(ps);		ps->WriteSVG(fname);
	delete ps;
}
//-----------------------------------------------------------------------------
void export_eps_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	char *fname = fl_file_chooser(gettext("Save File As?"), "*.eps", 0);
	if(!fname || !fname[0])	return;
	mglGraphPS *ps = new mglGraphPS(e->GetWidth(), e->GetHeight());
	ps->SetFont(e->GetFont());
	e->FMGL->update(ps);
	ps->WriteEPS(fname);
	delete ps;
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::ReLoad(bool o)
{	if(LoadFunc)	{	LoadFunc(o, FuncPar);	Update();	}	}
void oncemore_cb(Fl_Widget*, void*v)	{	((mglGraphFLTK*)v)->ReLoad(true);	}
//-----------------------------------------------------------------------------
void su_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (y2-y1)/3;	y1 += d;	y2 += d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void sd_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (y2-y1)/3;	y1 -= d;	y2 -= d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void sr_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (x2-x1)/3;	x1 += d;	x2 += d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void sl_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (x2-x1)/3;	x1 -= d;	x2 -= d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void sz_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (y2-y1)/4;	y1 += d;	y2 -= d;
	d = (x2-x1)/4;	x1 += d;	x2 -= d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void so_cb(Fl_Widget*, void* v)
{
	mglGraphFLTK* e = (mglGraphFLTK*)v;
	float x1,x2,y1,y2,d;
	e->FMGL->get_zoom(&x1,&y1,&x2,&y2);
	d = (y2-y1)/2;	y1 -= d;	y2 += d;
	d = (x2-x1)/2;	x1 -= d;	x2 += d;
	e->FMGL->set_zoom(x1,y1,x2,y2);
	e->Update();
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::Adjust()
{
	SetSize(scroll->w(), scroll->h());
	FMGL->size(scroll->w(), scroll->h());
	Update();
}
void adjust_cb(Fl_Widget*, void*v)	{	((mglGraphFLTK*)v)->Adjust();	}
//-----------------------------------------------------------------------------
void quit_cb(Fl_Widget*, void*v)	{	((mglGraphFLTK*)v)->Wnd->hide();	}
//-----------------------------------------------------------------------------
void snext_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->NextFrame();	}
//-----------------------------------------------------------------------------
void sprev_cb(Fl_Widget*, void* v)	{	((mglGraphFLTK*)v)->PrevFrame();	}
//-----------------------------------------------------------------------------
void time_cb(void *v)
{
	mglGraphFLTK *e = (mglGraphFLTK*)v;
	if(!e->sshow)	return;
	e->NextFrame();
	Fl::repeat_timeout(e->Delay, time_cb, v);
}
//-----------------------------------------------------------------------------
void mglGraphFLTK::Animation()
{
	sshow = NumFig<2 ? 0 : 1-sshow;
	anim_bt->value(sshow);
	anim_bt->image(sshow?xpm_s2:xpm_s1);	anim_bt->redraw();
	Fl_Menu_Item *m = (Fl_Menu_Item *)menu->find_item(gettext("Animate/Slideshow"));
	if(m && sshow)	m->set();
	if(m && !sshow)	m->clear();
	if(sshow)	Fl::add_timeout(Delay, time_cb, this);
}
void sshow_cb(Fl_Widget *, void *v)	{	((mglGraphFLTK*)v)->Animation();	}
void no_cb(Fl_Widget *, void *)	{}
//-----------------------------------------------------------------------------
Fl_Menu_Item pop_graph[15] = {
	{ gettext("Export"), 0, no_cb, 0, FL_SUBMENU,0,0,0,0},
		{ gettext("... as PNG"),	0, export_png_cb,0,0,0,0,0,0 },
		{ gettext("... as PNG (solid)"),	0, export_pngn_cb,0,0,0,0,0,0 },
		{ gettext("... as JPEG"),	0, export_jpeg_cb,0,0,0,0,0,0 },
		{ gettext("... as SVG"),	0, export_svg_cb,0,0,0,0,0,0 },
		{ gettext("... as vector EPS"),	0, export_eps_cb,0,0,0,0,0,0 },
		{ gettext("... as bitmap EPS"),	0, export_bps_cb, 0, FL_MENU_DIVIDER,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0 },
	{ gettext("Copy graphics"),	0, 0, 0, FL_MENU_INACTIVE|FL_MENU_DIVIDER,0,0,0,0},
	{ gettext("Normal view"),	0, norm_cb,0,0,0,0,0,0 },
	{ gettext("Redraw plot"),	0, draw_cb,0,0,0,0,0,0 },
	{ gettext("Adjust size"),	0, adjust_cb,0,0,0,0,0,0 },
	{ gettext("Reload data"),	0, oncemore_cb,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0 }
};
//-----------------------------------------------------------------------------
Fl_Menu_Item menuitems[] = {
	{ gettext("Export"), 0, 0, 0, FL_SUBMENU,0,0,0,0 },
		{ gettext(".. as PNG"),	FL_ALT + 'p', export_png_cb,0,0,0,0,0,0 },
		{ gettext(".. as PNG (solid)"),	FL_ALT + 'f', export_pngn_cb,0,0,0,0,0,0 },
		{ gettext(".. as JPEG"),	FL_ALT + 'j', export_jpeg_cb,0,0,0,0,0,0 },
		{ gettext(".. as SVG"),	FL_ALT + 's', export_svg_cb,0,0,0,0,0,0 },
		{ gettext(".. as vector EPS"),	FL_ALT + 'e', export_eps_cb,0,0,0,0,0,0 },
		{ gettext(".. as bitmap EPS"),	0, export_bps_cb, 0, FL_MENU_DIVIDER,0,0,0,0 },
		{ gettext("Exit"),			FL_ALT + 'x', quit_cb,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0 },
	{ gettext("Animate"), 0, 0, 0, FL_SUBMENU,0,0,0,0 },
		{ gettext("Slideshow"),	FL_CTRL + FL_F + 5, sshow_cb, 0, FL_MENU_TOGGLE,0,0,0,0 },
		{ gettext("Next frame"),0, snext_cb,0,0,0,0,0,0 },
		{ gettext("Prev frame"),0, sprev_cb,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0 },
	{ gettext("Graphics"), 0, 0, 0, FL_SUBMENU,0,0,0,0 },
		{ gettext("Alpha"),			FL_ALT + 'a', alpha_cb, 0, FL_MENU_TOGGLE,0,0,0,0 },
		{ gettext("Light"),			FL_ALT + 'l', light_cb, 0, FL_MENU_TOGGLE,0,0,0,0 },
		{ gettext("Copy graphics"),	0, 0, 0, FL_MENU_INACTIVE|FL_MENU_DIVIDER,0,0,0,0},
		{ gettext("Normal view"),	FL_ALT + ' ', norm_cb,0,0,0,0,0,0 },
		{ gettext("Redraw plot"),FL_F + 5, draw_cb,0,0,0,0,0,0 },
		{ gettext("Adjust size"),	FL_F + 6, adjust_cb,0,0,0,0,0,0 },
		{ gettext("Reload data"),	0, oncemore_cb,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0 }
};
//-----------------------------------------------------------------------------
void mglGraphFLTK::Window(int argc, char **argv, int (*draw)(mglGraph *gr, void *p), const char *title, void *par, void (*reload)(int next, void *p))
{
	NumFig=0;	CurFig=0;
	CurFrameId = 0;
	int n = draw(this,par);
	if(n<NumFig && n>=0)	NumFig = n;
	DrawFunc = draw;		FuncPar = par;
	LoadFunc = reload;
	if(Wnd)	{	Wnd->label(title);	Wnd->show();	return;	}

	Fl_Window *w1=new Fl_Double_Window(0,0,630,460,title);
	menu = new Fl_Menu_Bar(0, 0, 630, 30);
	menu->copy(menuitems, this);

	Fl_Group *g = new Fl_Group(0,30,410,30);
	Fl_Button *o;

	alpha_bt = new Fl_Button(0, 31, 25, 25);		alpha_bt->type(FL_TOGGLE_BUTTON);
	alpha_bt->image(xpm_a1);	alpha_bt->callback(alpha_cb,this);
	alpha_bt->tooltip(gettext("Switch on/off transparency in the picture"));
	alpha_bt->box(FL_PLASTIC_UP_BOX);			alpha_bt->down_box(FL_PLASTIC_DOWN_BOX);
	light_bt = new Fl_Button(25, 31, 25, 25);		light_bt->type(FL_TOGGLE_BUTTON);
	light_bt->image(xpm_l1);	light_bt->callback(light_cb,this);
	light_bt->tooltip(gettext("Switch on/off lightning in the picture"));
	light_bt->box(FL_PLASTIC_UP_BOX);			light_bt->down_box(FL_PLASTIC_DOWN_BOX);

	rotate_bt = new Fl_Button(55, 31, 25, 25);	rotate_bt->type(FL_TOGGLE_BUTTON);
	rotate_bt->image(xpm_r1);	rotate_bt->callback(rotate_cb,this);
	rotate_bt->tooltip(gettext("Rotate picture by holding left mouse button"));
	rotate_bt->box(FL_PLASTIC_UP_BOX);			rotate_bt->down_box(FL_PLASTIC_DOWN_BOX);
	zoom_bt = new Fl_Button(80, 31, 25, 25);		zoom_bt->type(FL_TOGGLE_BUTTON);
	zoom_bt->image(xpm_z1);	zoom_bt->callback(zoom_cb,this);
	zoom_bt->tooltip(gettext("Zoom in selected region of the picture"));
	zoom_bt->box(FL_PLASTIC_UP_BOX);				zoom_bt->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(105, 31, 25, 25);	o->tooltip(gettext("Return picture to normal zoom"));
	o->image(new Fl_Pixmap(zoom_out_xpm));	o->callback(norm_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);

	o = new Fl_Button(135, 31, 25, 25);	o->tooltip(gettext("Refresh the picture"));
	o->image(new Fl_Pixmap(ok_xpm));	o->callback(draw_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);

	tet = new Fl_Counter(170, 31, 90, 25, 0);	tet->callback(draw_cb,this);
	phi = new Fl_Counter(265, 31, 90, 25, 0);	phi->callback(draw_cb,this);
	tet->lstep(10);	tet->step(1);	tet->range(-180,180);
	tet->tooltip(gettext("Theta angle (tilt z-axis)"));
	phi->lstep(10);	phi->step(1);	phi->range(-180,180);
	phi->tooltip(gettext("Phi angle (rotate in x*y plane)"));
	tet->box(FL_PLASTIC_UP_BOX);			phi->box(FL_PLASTIC_UP_BOX);
	g->end();	g->resizable(0);

	g = new Fl_Group(0,60,30,260);
	o = new Fl_Button(1, 60, 25, 25);		o->tooltip(gettext("Shift the picture up"));
	o->image(new Fl_Pixmap(up_1_xpm));		o->callback(su_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 85, 25, 25);		o->tooltip(gettext("Shift the picture left"));
	o->image(new Fl_Pixmap(left_1_xpm));	o->callback(sl_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 110, 25, 25);		o->tooltip(gettext("Zoom in the picture"));
	o->image(new Fl_Pixmap(zoom_1_xpm));	o->callback(sz_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 135, 25, 25);		o->tooltip(gettext("Zoom out the picture"));
	o->image(new Fl_Pixmap(norm_1_xpm));	o->callback(so_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 160, 25, 25);		o->tooltip(gettext("Shift the picture right"));
	o->image(new Fl_Pixmap(right_1_xpm));	o->callback(sr_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 185, 25, 25);		o->tooltip(gettext("Shift the picture down"));
	o->image(new Fl_Pixmap(down_1_xpm));	o->callback(sd_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);

	o = new Fl_Button(1, 215, 25, 25);		o->tooltip(gettext("Show previous frame in slideshow"));
	o->image(new Fl_Pixmap(prev_sl_xpm));	o->callback(sprev_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);
	anim_bt = new Fl_Button(1, 240, 25, 25);		anim_bt->type(FL_TOGGLE_BUTTON);
	anim_bt->image(xpm_s1);	anim_bt->callback(sshow_cb,this);
	anim_bt->tooltip(gettext("Run/Stop slideshow (graphics animation)"));
	anim_bt->box(FL_PLASTIC_UP_BOX);		anim_bt->down_box(FL_PLASTIC_DOWN_BOX);
	o = new Fl_Button(1, 265, 25, 25);		o->tooltip(gettext("Show next frame in slideshow"));
	o->image(new Fl_Pixmap(next_sl_xpm));	o->callback(snext_cb,this);
	o->box(FL_PLASTIC_UP_BOX);	o->down_box(FL_PLASTIC_DOWN_BOX);

	g->end();	g->resizable(0);

	scroll = new Fl_Scroll(30, 60, 600, 400);
	//scroll->begin();
	FMGL = new Fl_MathGL(30, 60, 600, 400);
	FMGL->tet_val = tet;
	FMGL->phi_val = phi;
	FMGL->set_popup(pop_graph,w1,this);
	FMGL->graph = this;
	FMGL->draw_func = draw;
	FMGL->draw_par = par;
	scroll->end();

	w1->end();
	Wnd = w1;
	w1->resizable(scroll);	//w->graph);

	char *tmp[1];	tmp[0]=new char[1];	tmp[0][0]=0;
	w1->show(argv ? argc:0, argv ? argv:tmp);
	delete []tmp[0];
}
//-----------------------------------------------------------------------------
int mglFlRun()	{	return Fl::run();	}
//-----------------------------------------------------------------------------
HMGL mgl_create_graph_fltk(HMDR dr, const char *title)
{
	mglGraphFLTK *g = new mglGraphFLTK;
	g->Window(0,0,dr,title);
	return g;
}
//-----------------------------------------------------------------------------
HMGL mgl_create_graph_fltk(int (*draw)(HMGL gr, void *p), const char *title, void *par)
{
	mglGraphFLTK *g = new mglGraphFLTK;
	g->Window(0,0,draw,title,par);
	return g;
}
//-----------------------------------------------------------------------------
void *mgl_fl_tmp(void *)	{	mglFlRun();	return 0;	}
void mgl_fltk_run()	{	mglFlRun();	}
/*void mgl_fltk_thread()
{
	static pthread_t tmp;
	pthread_create(&tmp, 0, mgl_fl_tmp, 0);
	pthread_detach(tmp);
}*/
//-----------------------------------------------------------------------------
uintptr_t mgl_create_graph_fltk_(int (*draw)(uintptr_t *gr), const char *title, int l)
{
	mglGraphFLTK *g = new mglGraphFLTK;
	char *s = new char[l+1];	memcpy(s,title,l);	s[l]=0;
	g->Window(0,0,mgl_fortran_func,s,(void*)draw);
	delete []s;
	return uintptr_t(g);
}
//-----------------------------------------------------------------------------
void mgl_fltk_run_()	{	mglFlRun();	}
//void mgl_fltk_thread_()	{	mgl_fltk_thread();	}
//-----------------------------------------------------------------------------

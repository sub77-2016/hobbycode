/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */

#ifndef ANIMATIONVIEW_H
#define ANIMATIONVIEW_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wx.h"
#include "wx/glcanvas.h"
 
#ifdef __WXMAC__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif
 
#ifndef WIN32
#include <unistd.h> // FIXME: ¿This work/necessary in Windows?
#endif

#include "Animation.h"
#include "Camera.h"

#define MALE_BVH   "data/SLMale.bvh"
#define FEMALE_BVH "data/SLFemale.bvh"

class AnimationView : public wxGLCanvas {
public:
  /**
   * Singleton accessor to this class.
   */
  static AnimationView* getInstancePtr(){ return m_instance; };

  typedef enum { MALE, FEMALE, NUM_FIGURES } FigureType;

  AnimationView(wxFrame *parent, Animation *anim = 0, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("wxGLCanvas"));

  ~AnimationView();

  void setAnimation(Animation *anim);  
  Animation *getAnimation() { return animation; }
  bool isSkeletonOn() { return skeleton; }
  FigureType getFigure() { return figType; }
  void setFigure(FigureType type);
  void showSkeleton() { skeleton = true; }
  void hideSkeleton() { skeleton = false; }
  void selectPart(const char *part);
  const char *getSelectedPart();
  void getChangeValues(double *x, double *y, double *z);

  // only one graphic method
  void render();

private:
  typedef enum { MODE_PARTS, MODE_SKELETON, MODE_ROT_AXES };
  static const char figureFiles[NUM_FIGURES][256];
  Animation *animation;
  int last_x, last_y;  
  Camera *camera;
  double changeX, changeY, changeZ;
  BVHNode *joints[NUM_FIGURES];
  bool skeleton;
  bool selecting;
  int selectName;
  int partHighlighted;
  int partSelected;
  int partDragging;
  int dragX, dragY;
  int drawMode;
  bool xSelect, ySelect, zSelect;
  FigureType figType;
  bool initialized;  

  void draw();
  void drawFloor();
  void drawFigure();
  void drawPart(int frame, BVHNode *motion, BVHNode *joints, int mode);
  void setProjection();
  void setModelView();
  void setBodyMaterial();
  void resetCamera();
  void clearSelected();
  int pickPart(int x, int y);
  void drawCircle(int axis, float radius, int width);

  int w(){ return GetSize().x; };
  int h(){ return GetSize().x; };
  bool valid(){ return initialized; };
  void redraw(){ Refresh(); };

protected:
   //void redraw(wxIdleEvent& event);
   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnEraseBackground(wxEraseEvent& event);
   void OnMouse(wxMouseEvent& event);

   void mouseMoved(wxMouseEvent& event);
   void mouseDown(wxMouseEvent& event);
   void mouseWheelMoved(wxMouseEvent& event);
   void mouseReleased(wxMouseEvent& event);
   void rightClick(wxMouseEvent& event);
   void mouseLeftWindow(wxMouseEvent& event);
   void keyPressed(wxKeyEvent& event);
   void keyReleased(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()

  /**
   * The one and only CIntroScreen object.
   * @seealso getInstance()
   */
  static AnimationView *m_instance;
};

#endif

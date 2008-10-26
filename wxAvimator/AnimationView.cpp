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

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "Main.h"
#include "AnimationView.h"
#include "SLParts.h"

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

const char AnimationView::figureFiles[NUM_FIGURES][256] = { 
  MALE_BVH,
  FEMALE_BVH
};


BEGIN_EVENT_TABLE(AnimationView, wxGLCanvas)
    //EVT_IDLE	(AnimationView::redraw)
    EVT_SIZE	(AnimationView::OnSize)
    EVT_PAINT	(AnimationView::OnPaint)
    EVT_ERASE_BACKGROUND	(AnimationView::OnEraseBackground)
    EVT_MOUSE_EVENTS	(AnimationView::OnMouse)
    EVT_KEY_DOWN	(AnimationView::keyPressed)
    EVT_KEY_UP		(AnimationView::keyReleased)

/*
EVT_MOTION(AnimationView::mouseMoved)
EVT_LEFT_DOWN(AnimationView::mouseDown)
EVT_MOUSEWHEEL(AnimationView::mouseWheelMoved)
EVT_LEFT_UP(AnimationView::mouseReleased)
EVT_RIGHT_DOWN(AnimationView::rightClick)
EVT_LEAVE_WINDOW(AnimationView::mouseLeftWindow)
*/
END_EVENT_TABLE()

/*
 * Set our instance pointer to NULL - this will ensure the
 * singleton accessor works as expected.
 */
AnimationView* AnimationView::m_instance = NULL;

AnimationView::AnimationView(wxFrame *parent, Animation *anim, wxWindowID id, 
				const wxPoint& pos, const wxSize& size, 
				long style, const wxString& name) : 
  wxGLCanvas(parent, id, pos, parent->GetSize(), 
	style|wxFULL_REPAINT_ON_RESIZE|WX_GL_DOUBLEBUFFER|WX_GL_RGBA|WX_GL_DEPTH_SIZE, name), 
  animation(NULL), camera(NULL), figType(FEMALE), 
  skeleton(false), selecting(false),
  partHighlighted(0), partDragging(0), partSelected(0),
  dragX(0), dragY(0),
  changeX(0), changeY(0), changeZ(0),
  xSelect(false), ySelect(false), zSelect(false), initialized(false) 
{
  // Singleton 
  m_instance = static_cast<AnimationView*>(this);
   
  char fileName[256];
  char limFile[256];

  camera = new Camera();

  sprintf(limFile, "%s/%s", execPath, LIMITS_FILE);
  for (int i=0; i<NUM_FIGURES; i++) {
    sprintf(fileName, "%s/%s", execPath, figureFiles[i]);
    joints[i] = animRead(fileName, limFile);
  }

  setFigure(figType);
  setAnimation(anim);	
}

AnimationView::~AnimationView() 
{
  if (animation) delete(animation);
}

void AnimationView::setAnimation(Animation *anim) 
{ 
  if (animation) delete animation;
  animation = anim; 

  redraw();
}

void AnimationView::drawFloor() 
{
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_QUADS);
  for (int i=-10; i<10; i++) {
    for (int j=-10; j<10; j++) {
      if ((i+j) % 2)
	glColor4f(0.1, 0.1, 0.1, 1); 
      else
	glColor4f(0.6, 0.6, 0.6, 1); 
      glVertex3f(i*40, 0, j*40); glVertex3f(i*40, 0, (j+1)*40);      
      glVertex3f((i+1)*40, 0, (j+1)*40); glVertex3f((i+1)*40, 0, j*40);      
    }
  }
  glEnd();
}

void AnimationView::setProjection()
{
  gluPerspective(60.0, ((float)w())/h(), 0.01, 2000);
}

void AnimationView::setBodyMaterial()
{
  GLfloat ambientA[] = { 0.9, 0.667, 0.561, 1 };
  GLfloat diffuseA[] = { 0.9, 0.667, 0.561, 0 };
  GLfloat specularA[] = { 0.6, 0.6, 0.6, 0.0 };
  GLfloat shininessA = 100.0;  

  glMaterialfv(GL_FRONT, GL_AMBIENT, ambientA);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseA);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specularA);
  glMaterialf(GL_FRONT, GL_SHININESS, shininessA);
}

double goalX = 0, goalY = 0;

void AnimationView::draw() 
{
  GLfloat position0 [] = { 0.0, 80.0, 100.0, 1.0 };
  GLfloat ambient0[] = { 0.2, 0.2, 0.2, 1 };
  GLfloat diffuse0[] = { .5, .5, .5, 0.2 };
  GLfloat specular0[] = { 0.5, 0.5, 0.2, 0.5 };
  
  GLfloat position1 [] = { 0.0, 80.0, -100.0, 1.0 };
  GLfloat ambient1[] = { 0.2, 0.2, 0.2, 1 };
  GLfloat diffuse1[] = { 0.5, 0.5, 0.5, 1 };
  GLfloat specular1[] = { 1, 1, 1, 1 };

  wxPaintDC dc(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
  SetCurrent();

  if (!valid()) {
    initialized = true;    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, w(), h());
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    //  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    //  glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    
    glEnable(GL_NORMALIZE);
    glEnable(GL_FOG);
    {
      GLfloat fogColor[4] = {0.5, 0.5, 0.5, 0.3};
      int fogMode = GL_EXP; // GL_EXP2, GL_LINEAR
      glFogi (GL_FOG_MODE, fogMode);
      glFogfv (GL_FOG_COLOR, fogColor);
      glFogf (GL_FOG_DENSITY, 0.005);
      glHint (GL_FOG_HINT, GL_DONT_CARE);
      glFogf (GL_FOG_START, 200.0);
      glFogf (GL_FOG_END, 2000.0);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setProjection();
  }

  glClearColor(0.5, 0.5, 0.5, 0.3); /* fog color */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  camera->setModelView();
 
  glLightfv(GL_LIGHT0, GL_POSITION, position0);
  glLightfv(GL_LIGHT1, GL_POSITION, position1);

  drawFloor();

  if (animation) {
    drawFigure();
  }  

    glFlush();
    SwapBuffers();
}

int AnimationView::pickPart(int x, int y)
{
  static const int bufSize = (Animation::MAX_PARTS + 10) * 4;
  GLuint selectBuf[bufSize];
  GLuint *p, num, name = 0;
  GLint hits;
  GLint viewport[4];
  GLint depth = ~0;

  glGetIntegerv (GL_VIEWPORT, viewport);
  glSelectBuffer (bufSize, selectBuf);
  (void) glRenderMode (GL_SELECT);
  glInitNames();
  glPushName(0);
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  gluPickMatrix (x, (viewport[3] - y), 5.0, 5.0, viewport);
  setProjection();
  camera->setModelView();
  drawFigure();
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glFlush ();
  hits = glRenderMode (GL_RENDER);
  p = selectBuf;
  for (int i=0; i < hits; i++) {
    num = *(p++);
    if (*p < depth) {
      depth = *p;
      name = *(p+2);
    }
    p+=2;
    for (int j=0; j < num; j++) { *(p++); }
  }
  return name;
}

void AnimationView::drawFigure()
{
    glShadeModel(GL_SMOOTH);
    setBodyMaterial();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
    glTranslatef(0, 2, 0);
    selectName = 0;
    glEnable(GL_DEPTH_TEST);
    drawPart(animation->getFrame(), animation->getMotion(), joints[figType], 
	     MODE_PARTS);
    selectName = 0;
    glEnable(GL_COLOR_MATERIAL);
    drawPart(animation->getFrame(), animation->getMotion(), joints[figType], 
	     MODE_ROT_AXES);
    selectName = 0;
    glDisable(GL_DEPTH_TEST);
    drawPart(animation->getFrame(), animation->getMotion(), joints[figType], 
	     MODE_SKELETON);
}

void AnimationView::drawPart(int frame, BVHNode *motion, BVHNode *joints, int mode)
{
  float x, y, z, value, color[4];
  int offset = 0;
  GLint renderMode;
  bool selecting;

  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  selecting = (renderMode == GL_SELECT);
  if (motion && joints) {
    selectName++;
    glPushMatrix();
    glTranslatef(joints->offset[0], joints->offset[1], joints->offset[2]);
    if (!Animation::isSecondLifeJoint(motion)) {
      selectName++;
      motion = motion->child[0];
    }
    if (mode == MODE_SKELETON && skeleton && !selecting) {
      glColor4f(0,1,1,1);
      glLineWidth(1);
      glBegin(GL_LINES);
        glVertex3f(-joints->offset[0], -joints->offset[1], -joints->offset[2]);
        glVertex3f(0,0,0);
      glEnd();
      glColor4f(0, 1, 0, 1);
//      if (joints->type != BVH_ROOT)
//	glutSolidSphere(1, 16, 16);
    }
    if (joints->type == BVH_ROOT) {
      for (int i=0; i<motion->numChannels; i++) {
	value = motion->frame[frame][i];
	switch(motion->channelType[i]) {
	case BVH_XPOS: glTranslatef(value, 0, 0); break;
	case BVH_YPOS: glTranslatef(0, value, 0); break;
	case BVH_ZPOS: glTranslatef(0, 0, value); break;
	}
      }
    }
    for (int i=0; i<motion->numChannels; i++) {
      if (motion->ikOn)
	value = motion->frame[frame][i] + motion->ikRot[i];
      else
	value = motion->frame[frame][i];
      switch(motion->channelType[i]) {
      case BVH_XROT: glRotatef(value, 1, 0, 0); break;
      case BVH_YROT: glRotatef(value, 0, 1, 0);	break;
      case BVH_ZROT: glRotatef(value, 0, 0, 1);	break;
      }
      if (mode == MODE_ROT_AXES && !selecting && partSelected == selectName) {
	switch(motion->channelType[i]) {
	case BVH_XROT: drawCircle(0, 10, xSelect ? 4 : 1); break;
	case BVH_YROT: drawCircle(1, 10, ySelect ? 4 : 1); break;
	case BVH_ZROT: drawCircle(2, 10, zSelect ? 4 : 1); break;
	}
      }
    }
    if (mode == MODE_PARTS) {
      if (selecting) glLoadName(selectName);
      if (animation->getMirrored() && 
	  (animation->getPartMirror(partSelected) == selectName ||
	   partSelected == selectName))
	glColor4f(1.0, 0.635, 0.059, 1);
      else if (partSelected == selectName)
	glColor4f(0.6, 0.3, 0.3, 1);
      else if (partHighlighted == selectName)
	glColor4f(0.6, 0.6, 0.6, 1);
	  else if (animation->isKeyFrame(motion))
	glColor4f(0.4, 0.5, 0.3, 1);
      else
	glColor4f(0.6, 0.5, 0.5, 1);
      if (animation->getIK(motion->name)) {
	glGetFloatv(GL_CURRENT_COLOR, color);
	glColor4f(color[0], color[1], color[2]+0.3, color[3]);
      }
      figType==MALE ? drawSLMalePart(motion->name):drawSLFemalePart(motion->name);
    }
    for (int i=0; i<motion->numChildren; i++) {
      drawPart(frame, motion->child[i], joints->child[i], mode);
    }
    glPopMatrix();
  }
}

void AnimationView::drawCircle(int axis, float radius, int width)
{
  GLint circle_points = 100;
  float angle;

  glLineWidth(width);
  switch (axis) {
  case 0: glColor4f(1,0,0,1);break;
  case 1: glColor4f(0,1,0,1);break;
  case 2: glColor4f(0,0,1,1);break;
  }
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < circle_points; i++) {
    angle = 2*M_PI*i/circle_points;
    switch (axis) {
    case 0: glVertex3f(0, radius*cos(angle), radius*sin(angle));break;
    case 1: glVertex3f(radius*cos(angle), 0, radius*sin(angle));break;
    case 2: glVertex3f(radius*cos(angle), radius*sin(angle), 0);break;
    }
  }
  glEnd();
  glBegin(GL_LINES);
  switch (axis) {
  case 0: glVertex3f(-10, 0, 0); glVertex3f(10, 0, 0); break;
  case 1: glVertex3f(0, -10, 0); glVertex3f(0, 10, 0); break;
  case 2: glVertex3f(0, 0, -10); glVertex3f(0, 0, 10); break;
  }  
  glEnd();
  
}

void AnimationView::setFigure(FigureType type)
{
  if (type >= 0 && type < NUM_FIGURES)
    figType = type;
}

const char *AnimationView::getSelectedPart()
{
  return animation->getPartName(partSelected);
}

void AnimationView::selectPart(const char *part)
{
  partSelected = animation->getPartIndex(part);
  redraw();
}

void AnimationView::getChangeValues(double *x, double *y, double *z)
{
  *x = changeX;
  *y = changeY;
  *z = changeZ;
}

void AnimationView::render()
{
  draw();
}

// Events
/*
void AnimationView::redraw(wxIdleEvent& event)
{
  draw();
  event.RequestMore();
}
*/

void AnimationView::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    draw();
}

void AnimationView::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
     wxGLCanvas::OnSize(event);
     redraw();
    // Reset the OpenGL view aspect
    //setProjection();
}

void AnimationView::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

/*
int AnimationView::handle(int event) {
  int shift_state = Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT);
  int selected = 0;

  dragX = Fl::event_x() - last_x;
  dragY = Fl::event_y() - last_y;
  last_x=Fl::event_x();
  last_y=Fl::event_y();
  switch(event) {
  case FL_MOVE:
    partHighlighted = pickPart(last_x, last_y);
    redraw();
    return 1;
  case FL_PUSH:
    selected = pickPart(last_x, last_y);
    if (Fl::event_clicks()) {
      if (shift_state & FL_SHIFT)
	animation->setMirrored(true);
      else if (shift_state & FL_CTRL && selected) 
	animation->toggleKeyFrame(animation->getPartName(selected));
      else if (selected)
	animation->setIK(animation->getPartName(selected), 
			 !animation->getIK(animation->getPartName(selected)));
    }
    else if (selected != partSelected) 
      animation->setMirrored(false);
    partDragging = partSelected = selected;
    if (selected) {
      changeX = changeY = changeZ = 0;
      dragX = dragY = 0;
      //do_callback(this);
    }
    // MainWindow now
    do_callback(this);
    fflush(stdout);
    redraw();
    return 1;
  case FL_RELEASE:
    partDragging = 0;
    return 1;
  case FL_DRAG:
    if (partDragging) {
      changeX = changeY = changeZ = 0;
      if (shift_state & FL_SHIFT) { changeX = dragY; }
      if (shift_state & FL_ALT) { changeY = dragX; }
      else if (shift_state & FL_CTRL) { changeZ = -dragX; }
      do_callback(this);
    }
    else {
      if (shift_state & FL_SHIFT)
	camera->pan(dragX/2, dragY/2,0);
      else if (shift_state & FL_ALT) {
	camera->pan(0, 0, dragY);
	camera->rotate(0, dragX);
      }
      else
        camera->rotate(dragY, dragX);
    }
    redraw();
    return 1;
  case FL_MOUSEWHEEL:
    camera->pan(0,0,Fl::event_dy()*10);
    redraw();
    return 1;
  case FL_KEYDOWN:
    switch(Fl::event_key()) {
    case FL_Page_Up: camera->pan(0,0,-5); redraw(); return 1;
    case FL_Page_Down: camera->pan(0,0,5); redraw(); return 1;
    case FL_Shift_L:
    case FL_Shift_R: xSelect = true; redraw(); return 1;
    case FL_Alt_L:
    case FL_Alt_R: ySelect = true; redraw(); return 1;
    case FL_Control_L:
    case FL_Control_R: zSelect = true; redraw(); return 1;
    }
    return 0;
  case FL_KEYUP:
    switch(Fl::event_key()) {
    case FL_Shift_L:
    case FL_Shift_R: xSelect = false; redraw(); return 1;
    case FL_Alt_L:
    case FL_Alt_R: ySelect = false; redraw(); return 1;
    case FL_Control_L:
    case FL_Control_R: zSelect = false; redraw(); return 1;
    }
    return 0;
  default:
    return Fl_Gl_Window::handle(event);
  }
}
*/

void AnimationView::OnMouse(wxMouseEvent& event)
{
  int selected = 0;

  dragX = event.GetX() - last_x;
  dragY = event.GetY() - last_y;
  last_x = event.GetX();
  last_y = event.GetY();

  if ( event.Moving() ) {
    partHighlighted = pickPart(last_x, last_y);
    redraw();
  } 
  else if ( event.LeftDown() ) 
  {
    selected = pickPart(last_x, last_y);
    if ( event.LeftDown() ) {
      if ( event.ShiftDown() )
	animation->setMirrored(true);
      else if ( event.ControlDown() && selected) 
	animation->toggleKeyFrame(animation->getPartName(selected));
      else if (selected)
	animation->setIK(animation->getPartName(selected), 
			 !animation->getIK(animation->getPartName(selected)));
    }
    else if (selected != partSelected) 
      animation->setMirrored(false);
    partDragging = partSelected = selected;
    if (selected) {
      changeX = changeY = changeZ = 0;
      dragX = dragY = 0;
      //do_callback(this);
    }
    // MainWindow now
    //do_callback(this);
    fflush(stdout);
    redraw();
  }
  else if ( event.LeftUp() )
  {
    partDragging = 0;
  }
  else if ( event.Dragging() ) 
  {
   if (partDragging) {
      changeX = changeY = changeZ = 0;
      if ( event.ShiftDown() ) { changeX = dragY; }
      if ( event.AltDown() ) { changeY = dragX; }
      else if ( event.ControlDown() ) { changeZ = -dragX; }
      //do_callback(this);
    }
   else {
      if ( event.ShiftDown() )
	camera->pan(dragX/2, dragY/2,0);
      else if ( event.AltDown() ) {
	camera->pan(0, 0, dragY);
	camera->rotate(0, dragX);
      }
      else
        camera->rotate(dragY, dragX);
    }
    redraw();
  }// end dragging
  else if ( event.GetWheelRotation() != 0 )
  {
    const int dy = event.GetWheelRotation()/event.GetWheelDelta();
    camera->pan(0,0,dy*10);
    redraw();
  }
} // end OnMouse()

void AnimationView::keyPressed(wxKeyEvent& event) 
{
  switch( event.GetKeyCode() ) {
    case WXK_PAGEUP: camera->pan(0,0,-5); redraw();
    case WXK_PAGEDOWN: camera->pan(0,0,5); redraw();
    //case FL_Shift_L:
    case WXK_SHIFT: xSelect = true; redraw();
    //case FL_Alt_L:
    //case FL_Alt_R: ySelect = true; redraw();
    //case FL_Control_L:
    case WXK_CONTROL: zSelect = true; redraw();
  }
}

void AnimationView::keyReleased(wxKeyEvent& event) 
{
  switch( event.GetKeyCode() ) {
    //case FL_Shift_L:
    case WXK_SHIFT: xSelect = false; redraw();
    //case FL_Alt_L:
    //case FL_Alt_R: ySelect = false; redraw();
    //case FL_Control_L:
    case WXK_CONTROL: zSelect = false; redraw();
  }
}

void AnimationView::mouseMoved(wxMouseEvent& event) 
{
}

void AnimationView::mouseDown(wxMouseEvent& event) 
{
}

void AnimationView::mouseWheelMoved(wxMouseEvent& event) 
{
}

void AnimationView::mouseReleased(wxMouseEvent& event) 
{
}

void AnimationView::rightClick(wxMouseEvent& event) 
{
}

void AnimationView::mouseLeftWindow(wxMouseEvent& event) 
{
}


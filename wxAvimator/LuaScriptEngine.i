%module ScriptEngine
%include "typemaps.i"

%{
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
#include "AnimationView.h"
#include "MainApplication.h"


MainApplication* getMainApplication() {
  return MainApplication::getInstancePtr();
};

AnimationView* getAnimationView() {
  return AnimationView::getInstancePtr();
};

Animation* getAnimation() {
  return Animation::getInstancePtr();
};

%}

/*
template <typename T> class Singleton {
	static T& getInstance();
	static T* getInstancePtr();
};
%template(CSayCheeseSton) Singleton<CSayCheese>;
*/

class Animation {
 public:
  typedef enum { IK_LHAND=0, IK_RHAND, IK_LFOOT, IK_RFOOT, NUM_IK } IKPartType;

  Animation(const char *bvhFile = NULL);
  ~Animation();
  void loadBVH(const char *bvhFile);
  void saveBVH(const char *bvhFile);
  int stepForward();
};

class AnimationView {
public:

  typedef enum { MALE, FEMALE, NUM_FIGURES } FigureType;

  AnimationView(wxFrame *parent, Animation *anim = 0, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("wxGLCanvas"));

  ~AnimationView();

  void setAnimation(Animation *anim);  
  Animation *getAnimation();
  void selectPart(const char *part);
  const char *getSelectedPart();
  void getChangeValues(double *x, double *y, double *z);

  void render();
};

class MainApplication {
 public:
  MainApplication( const wxString& title, const wxPoint& pos, const wxSize& size );
  AnimationView *animVwr;
 void Show(bool);
};

MainApplication* getMainApplication();
AnimationView* getAnimationView();
Animation* getAnimation();


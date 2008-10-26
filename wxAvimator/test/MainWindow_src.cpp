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

#include <stdlib.h>
#include <FL/Fl_File_Chooser.H>
#include "MainWindow.h"

void cb_timeout(void *v);

void deactivateInputs(MainWindow *w) 
{
  w->xRotRoller->deactivate();
  w->yRotRoller->deactivate();
  w->zRotRoller->deactivate();
  w->xRotValue->deactivate();
  w->yRotValue->deactivate();
  w->zRotValue->deactivate();
  w->xPosRoller->deactivate();
  w->yPosRoller->deactivate();
  w->zPosRoller->deactivate();
  w->xPosValue->deactivate();
  w->yPosValue->deactivate();
  w->zPosValue->deactivate();
  w->keyBtn->deactivate();
}

void activateInputs(MainWindow *w) 
{
  w->xRotRoller->activate();
  w->yRotRoller->activate();
  w->zRotRoller->activate();
  w->xRotValue->activate();
  w->yRotValue->activate();
  w->zRotValue->activate();
  w->xPosRoller->activate();
  w->yPosRoller->activate();
  w->zPosRoller->activate();
  w->xPosValue->activate();
  w->yPosValue->activate();
  w->zPosValue->activate();
  w->keyBtn->activate();
}

void updateKeyBtn(MainWindow *w) {
  if (w->view->getAnimation()->isKeyFrame(w->view->getSelectedPart())) {
    w->keyBtn->image(w->keyImage);
  } else {
    w->keyBtn->image(w->nokeyImage);
  }

  w->keyBtn->redraw();
}

void updateInputs(MainWindow *w) 
{
  double x=0, y=0, z=0;
  Animation *anim = w->view->getAnimation();

  if (anim) {
    double xMin, xMax, yMin, yMax, zMin, zMax;
    anim->getRotation(w->partChoice->text(), &x,&y,&z);
    anim->getRotationLimits(w->partChoice->text(), &xMin, &xMax, &yMin, &yMax,
			    &zMin, &zMax);
    w->xRotRoller->value(x);
    w->xRotRoller->bounds(xMin, xMax);
    w->yRotRoller->value(y);
    w->yRotRoller->bounds(yMin, yMax);
    w->zRotRoller->value(z);
    w->zRotRoller->bounds(zMin, zMax);
    w->xRotValue->value(x);
    w->yRotValue->value(y);
    w->zRotValue->value(z);
    w->frameSlider->maximum(anim->getNumberOfFrames());
    w->frameSlider->value(anim->getFrame() + 1);
    w->frameSlider->redraw();
    w->frameValue->value(anim->getFrame() + 1);
    w->numFramesValue->value(anim->getNumberOfFrames());
    if (anim->useRotationLimits()) w->limits->setonly();
  }
  if (!strcmp(w->partChoice->text(), "hip")) {
    if (!w->xPosValue->visible()) w->xPosValue->show();
    if (!w->yPosValue->visible()) w->yPosValue->show();
    if (!w->zPosValue->visible()) w->zPosValue->show();
    if (!w->xPosRoller->visible()) w->xPosRoller->show();
    if (!w->yPosRoller->visible()) w->yPosRoller->show();
    if (!w->zPosRoller->visible()) w->zPosRoller->show();
    anim->getPosition("hip", &x,&y,&z);
    w->xPosRoller->value(x);
    w->yPosRoller->value(y);
    w->zPosRoller->value(z);
    w->xPosValue->value(x);
    w->yPosValue->value(y);
    w->zPosValue->value(z);
  }
  else {
    if (w->xPosValue->visible()) w->xPosValue->hide();
    if (w->yPosValue->visible()) w->yPosValue->hide();
    if (w->zPosValue->visible()) w->zPosValue->hide();
    if (w->xPosRoller->visible()) w->xPosRoller->hide();
    if (w->yPosRoller->visible()) w->yPosRoller->hide();
    if (w->zPosRoller->visible()) w->zPosRoller->hide();
  }
  w->playBtn->image(w->playing ? w->pauseImage : w->playImage);
  w->playBtn->redraw();

  updateKeyBtn(w);

  if (w->playing) 
    deactivateInputs(w);
  else
    activateInputs(w);
  if (w->frameDataValid)
    w->editPasteFrame->activate();
  else
    w->editPasteFrame->deactivate();

  if (w->view->getAnimation()->frameTime() != 0.0)
    w->fpsValue->value(1 / (w->view->getAnimation()->frameTime()));
}

void cb_timeout(void *v)
{
  MainWindow *w = (MainWindow *)v;
  if (w->playing) {
    Animation *anim = w->view->getAnimation();
    if (anim) {
      w->view->redraw();
      anim->stepForward();
      //      if (!anim->stepForward())
      //	w->playing = false;

      if (anim->getFrame() == (anim->getNumberOfFrames() - 1) && !w->loop)
        w->playing = false;
      else
      Fl::repeat_timeout(anim->frameTime(), cb_timeout, w);

      updateInputs(w);
      return;
    }
  }
  //Fl::repeat_timeout(1/30.0, cb_timeout, w);
}

void newFile(MainWindow *w)
{
  static char title[256];

  strcpy(w->currentFile, UNTITLED_NAME); 
  sprintf(title, "%s [%s]", APPLICATION_NAME, UNTITLED_NAME);
  w->label(title);
  w->view->setAnimation(new Animation());  
  w->partChoice->value(1);
  w->playing = false;
  updateInputs(w);
}

void MainWindow::initialize() 
{
  char fileName[256];
  sprintf(fileName, "%s/%s", execPath, PLAY_IMAGE);
  playImage = new Fl_PNG_Image(fileName);
  sprintf(fileName, "%s/%s", execPath, PAUSE_IMAGE);
  pauseImage = new Fl_PNG_Image(fileName);
  sprintf(fileName, "%s/%s", execPath, KEY_IMAGE);
  keyImage = new Fl_PNG_Image(fileName);
  sprintf(fileName, "%s/%s", execPath, NOKEY_IMAGE);
  nokeyImage = new Fl_PNG_Image(fileName);
  frameDataValid = false;
  newFile(this);
  //Fl::add_timeout(1/30.0, cb_timeout, this);
}

int MainWindow::handle(int event) 
{
  switch (event) {
  case FL_KEYDOWN:
  case FL_KEYUP: view->handle(event); break;
  }
  return Fl_Double_Window::handle(event);
}

void cb_New(Fl_Menu_*o, void*)
{
  MainWindow *w = (MainWindow *)(o->parent());
  newFile(w);
}

void saveAs(MainWindow *w) {
  static char title[256];
  char *file = fl_file_chooser("Save Animation File", "*.bvh", w->currentFile);
  if (file) {
    strcpy(w->currentFile, file);
    sprintf(title, "%s [%s]", APPLICATION_NAME, w->currentFile);
    w->label(title);
    w->view->getAnimation()->saveBVH(file);
  }
}

void save(MainWindow *w) {
  if (!strcmp(w->currentFile, UNTITLED_NAME)) {
    saveAs(w);
  }
  else {
    w->view->getAnimation()->saveBVH(w->currentFile);
  }
}


void userQuit(MainWindow *w) {
  Animation *anim = w->view->getAnimation();

  if (anim) {
    switch (fl_choice("Would you like to save the current animation?", "Cancel", "Yes","No")) {
      case 0: // CANCEL
        return;
      case 1: // YES
        save(w);
        break;
      case 2: // NO
        break;
    }
  }

  w->hide();
}

void cb_Exit(Fl_Menu_*o, void*v)
{
  userQuit((MainWindow *)(o->parent()));
}

void cb_MainWindow(Fl_Double_Window *o, void *v) {
  if (Fl::event()==FL_SHORTCUT && Fl::event_key()==FL_Escape)
    return; // ignore the FLTK default that escape closes the window

  userQuit((MainWindow *)o);
}

void cb_Open(Fl_Menu_*o, void*)
{
  MainWindow *w = (MainWindow *)(o->parent());
  static char title[256];
  char *file = fl_file_chooser("Select Animation File", "*.bvh", NULL);
  if (file) {
    strcpy(w->currentFile, file);
    w->view->setAnimation(new Animation(file));
    sprintf(title, "%s [%s]", APPLICATION_NAME, w->currentFile);
    w->label(title);
    w->partChoice->value(1);
    updateInputs(w);
    /*
      playBtn->label("Play");
      partChc->clear();
      addJoints(w->partChc, w->viewWin->getAnimation()->getJoints());
      partChc->value(1);
      partChc_cb(w->partChc, w);
      playState = STOPPED;
    */
  }
}

void cb_Save(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  save(w);
}

void cb_SaveAs(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  saveAs(w);
}

void cb_FigureMale(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->setFigure(AnimationView::MALE);
  w->view->redraw();
}

void cb_FigureFemale(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->setFigure(AnimationView::FEMALE);
  w->view->redraw();
}

void cb_JointsShow(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->showSkeleton();
  w->view->redraw();
}

void cb_JointsHide(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->hideSkeleton();
  w->view->redraw();
}

void cb_LimitsOn(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->getAnimation()->useRotationLimits(true);
  w->view->redraw();
  updateInputs(w);
}

void cb_LimitsOff(Fl_Menu_*o, void*v)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->getAnimation()->useRotationLimits(false);
  w->view->redraw();
  updateInputs(w);
}

void cb_Loop(Fl_Menu_ *o, void *v) {
  MainWindow *w = (MainWindow *)(o->parent());
  const Fl_Menu_Item *menuItem = o->mvalue();
  w->loop = o->mvalue()->value();
}

void cb_fpsValue(Fl_Value_Input *o, void*) {
  MainWindow *w = ((MainWindow*) (o->parent()->parent()->parent()->parent()));
  double fps = w->fpsValue->value();

  w->view->getAnimation()->setFrameTime(1/fps);
}

void cb_RotRoller(Fl_Roller*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  double x = w->xRotRoller->value();
  double y = w->yRotRoller->value();
  double z = w->zRotRoller->value();
  w->xRotValue->value(x);
  w->yRotValue->value(y);
  w->zRotValue->value(z);
  if (w->partChoice->text()) {
    w->view->getAnimation()->setRotation(w->partChoice->text(), x, y, z);
    w->view->redraw();
  }

  updateKeyBtn(w);
}

void cb_RotValue(Fl_Value_Input*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  double x = w->xRotValue->value();
  double y = w->yRotValue->value();
  double z = w->zRotValue->value();

  if (x < -180) x = -180;  if (y < -180) y = -180;  if (z < -180) z = -180;
  if (x > 180) x = 180;  if (y > 180) y = 180;  if (z > 180) z = 180;
  w->xRotRoller->value(x);
  w->yRotRoller->value(y);
  w->zRotRoller->value(z);
  if (w->partChoice->text()) {
    w->view->getAnimation()->setRotation(w->partChoice->text(), x, y, z);
    w->view->redraw();
  }

  updateKeyBtn(w);
}

void cb_PosRoller(Fl_Roller*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  double x = w->xPosRoller->value();
  double y = w->yPosRoller->value();
  double z = w->zPosRoller->value();
  w->xPosValue->value(x);
  w->yPosValue->value(y);
  w->zPosValue->value(z);
  if (w->partChoice->text()) {
    w->view->getAnimation()->setPosition(w->partChoice->text(), x, y, z);
    w->view->redraw();
  }

  updateKeyBtn(w);
}

void cb_PosValue(Fl_Value_Input*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  double x = w->xPosValue->value();
  double y = w->yPosValue->value();
  double z = w->zPosValue->value();
  double min_x = w->xPosRoller->minimum();
  double min_y = w->yPosRoller->minimum();
  double min_z = w->zPosRoller->minimum();
  double max_x = w->xPosRoller->maximum();
  double max_y = w->yPosRoller->maximum();
  double max_z = w->zPosRoller->maximum();

  if (x<min_x) x = min_x;  if (y<min_y) y = min_y;  if (z<min_z) z = min_z;
  if (x>max_x) x = max_x;  if (y>max_y) y = max_y;  if (z>max_z) z = max_z;

  w->xPosRoller->value(x);
  w->yPosRoller->value(y);
  w->zPosRoller->value(z);
  w->xPosValue->value(x);
  w->yPosValue->value(y);
  w->zPosValue->value(z);
  if (w->partChoice->text()) {
    w->view->getAnimation()->setPosition(w->partChoice->text(), x, y, z);
    w->view->redraw();
  }

  updateKeyBtn(w);
}

void cb_PartChoice(Fl_Choice*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  w->view->selectPart(o->text());
  updateInputs(w);
}

void cb_FrameSlider(Fl_Slider*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()));
  double frame = o->value() - 1;
  w->playing = false;
  w->view->getAnimation()->setFrame((int)frame);
  updateInputs(w);
  w->view->redraw();
}

void cb_PlayBtn(Fl_Button* o, void* v)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()));
  w->playing = !w->playing;
  if (w->playing) {
     Fl::add_timeout(w->view->getAnimation()->frameTime(), cb_timeout, w);
  }

  updateInputs(w);
}

void cb_KeyBtn(Fl_Button* o, void* v)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()));

  w->view->getAnimation()->toggleKeyFrame(w->view->getSelectedPart());
  w->view->redraw();
  updateKeyBtn(w);
}

void cb_AnimationView(AnimationView *o, void *v)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()));
  const char *partName = o->getSelectedPart();
  const Fl_Menu_Item *item = NULL;
  double dx, dy, dz, x, y, z;
  double xMin, xMax, yMin, yMax, zMin, zMax;

  if (partName) {
    item = w->partChoice->find_item(partName);
  }

  if (item) {
    w->partChoice->value(item);
    cb_PartChoice(w->partChoice, NULL);
    o->getChangeValues(&dx, &dy, &dz);
	o->getAnimation()->getRotationLimits(partName, &xMin, &xMax,
					 &yMin, &yMax, &zMin, &zMax);
    w->xRotRoller->bounds(xMin, xMax);
    w->yRotRoller->bounds(yMin, yMax);
    w->zRotRoller->bounds(zMin, zMax);
    x = w->xRotRoller->value()+dx;
    y = w->yRotRoller->value()+dy;
    z = w->zRotRoller->value()+dz;
    if (x < xMin) x = xMin;
    if (x > xMax) x = xMax;
    if (y < yMin) y = yMin;
    if (y > yMax) y = yMax;
    if (z < zMin) z = zMin;
    if (z > zMax) z = zMax;
    w->xRotRoller->value(x);
    w->yRotRoller->value(y);
    w->zRotRoller->value(z);

	// cb_RotRoller updates the figure and sets a keyframe.  It should only be called
	// when a part is moved, thus automatically setting a keyframe.  Users are used
	// to this functionality from previous releases, so we should keep this the same.
	//
	if (fabs(dx+dy+dz) > 0.01)
	  cb_RotRoller(w->xRotRoller, NULL);
  } else {
    // make key button reflect that nothing is selected
    updateKeyBtn(w);
  }  
}

void cb_NumFramesValue(Fl_Value_Input*o, void*)
{
  MainWindow *w = ((MainWindow*)(o->parent()->parent()->parent()->parent()));
  w->view->getAnimation()->setNumberOfFrames((int)(o->value()));
  updateInputs(w);
}

void cb_CopyFrame(Fl_Menu_*o, void*)
{
  MainWindow *w = (MainWindow *)(o->parent());
  w->view->getAnimation()->getFrameData(w->frameData);
  w->frameDataValid = true;
  updateInputs(w);
}

void cb_PasteFrame(Fl_Menu_*o, void*)
{
  MainWindow *w = (MainWindow *)(o->parent());
  if (w->frameDataValid)
    w->view->getAnimation()->setFrameData(w->frameData);
}



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

#ifndef FIGURE_H
#define FIGURE_H

class Figure {
public:
  virtual drawPart(char *name);
  
protected:
  virtual draw_hip() { };
  virtual draw_abdomen() { };
  virtual draw_chest() { };
  virtual draw_neck() { };
  virtual draw_head() { };
  virtual draw_lCollar() { };
  virtual draw_lShldr() { };
  virtual draw_lForeArm() { };
  virtual draw_lHand() { };
  virtual draw_rCollar() { };
  virtual draw_rShldr() { };
  virtual draw_rForeArm() { };
  virtual draw_rHand() { };
  virtual draw_lThigh() { };
  virtual draw_lShin() { };
  virtual draw_lFoot() { };
  virtual draw_rThigh() { };
  virtual draw_rShin() { };
  virtual draw_rFoot() { };
}

#endif

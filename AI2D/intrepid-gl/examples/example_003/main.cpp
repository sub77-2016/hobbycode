// +----------------------------------------------------------------------+
// |Intrepid 2D game library                                              |
// |                                                                      |
// |2007                                                                  |
// |                                                                      |
// |Originally written by Eduardo H. Cruz and Juliano H. Foleiss          |
// +----------------------------------------------------------------------+
// | Under GPL License                                                    |
// +----------------------------------------------------------------------+
// | This program is free software; you can redistribute it and/or modify |
// |   it under the terms of the GNU General Public License as published  |
// |   by the Free Software Foundation; either version 2 of the License,  |
// |   or (at your option) any later version.                             |
// |                                                                      |
// |   This program is distributed in the hope that it will be useful,    |
// |   but WITHOUT ANY WARRANTY; without even the implied warranty of     |
// |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      |
// |   GNU General Public License for more details.                       |
// |                                                                      |
// |   You should have received a copy of the GNU General Public License  |
// |   along with this program; if not, write to the Free Software        |
// |   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           |
// |   02111-1307  USA                                                    |
// +----------------------------------------------------------------------+

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "header/libgame.h"

void play_game();

int w, h; //width and height of the window

int main(int argc, char *argv[])
{
  w = 640; 
  h = 480; 
		
  if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    //Couldn't initialize  
    exit(1); 

  if( SDL_SetVideoMode( w, h, 16, SDL_OPENGL  ) == NULL )exit(1);   

  if (TTF_Init() == -1)
    exit(1);

  //Setting up OpenGL

  //Set clear color
  glClearColor( 0, 0, 0, 0 );
    
  //Set projection
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0, w, h, 0, -1, 1 );
    
  //Initialize modelview matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
    
  //If there was any errors
  if( glGetError() != GL_NO_ERROR )
    {
      return 0;    
    }
    
  //Set caption
  SDL_WM_SetCaption( "Intrepid 2D Test", NULL );     
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
                   
  glClear(GL_COLOR_BUFFER_BIT);

  play_game();

  SDL_Quit();
  exit(0);
}

void play_game()
{
  //creating scene object
  lg_physics_scene scene;
  lg_scene_param scenep;
  
  //setting up the scene
  scenep.start_x = 0;
  scenep.start_y = 0;
  scenep.scene_w = w;
  scenep.scene_h = h;
  scenep.canvas_w = w;
  scenep.canvas_h = h;
  scenep.bits = 32;
  scenep.fname = "mario.bmp"; 
  scene.set(&scenep);
  
  //creating mouse 
  lg_surface mouse_img;
  lg_color blue, white;
 
  //we want to consider blue as the transparent color, so we set it to:
  blue.r = 0;
  blue.g = 0;
  blue.b = 255;

  //load the hand bitmap
  mouse_img.load_image("cursor.bmp", LF_CO_TRANSPARENT, &blue);
  
  //setup the mouse
  lg_set_mouse(&mouse_img);

  //creating the button
  lg_object button;
  lg_surface button_img;
  lg_object_param objp;
  
  //setting some button parameters
  objp.id = 0;
  objp.active = LGOBJ_ACTIVE;
  objp.visible = LGOBJ_VISIBLE;
  objp.x = 50;
  objp.y = 100;
  objp.ang = 0.0f;

  button.set(&objp, &scene);    
  button.set_video_handler(&button_img);
  button_img.load_image("button1.bmp", 0, NULL);
  
  //adding the button to sceme
  scene.add_object(&button);  

  //load the main character
  lg_physics_obj crono;
  lg_phyobject_param objphy_param;
  lg_sprite_framebased crono_sp;
  lg_vector *crono_velocity;
  
  //create the main character
  objphy_param.obj.id = 4;
  objphy_param.obj.x = 30;
  objphy_param.obj.y = 0;
  objphy_param.obj.active = LGOBJ_ACTIVE;
  objphy_param.obj.visible = LGOBJ_VISIBLE;

  objphy_param.mass = 10.0f;
  objphy_param.shape = LG_SHAPE_PIXEL;
  
  //we want to set white as the transparent color
  white.b = 255;
  white.r = 255;
  white.g = 255;
  
  //setting crono up
  crono.set_phy(&objphy_param);
  crono.set_video_handler(&crono_sp);
  crono_sp.load_sprites("crono.bmp", 6, 3, &white, LF_CO_TRANSPARENT);

  //setting up the main character's sprite
  int sp_seq4_0[] = { 0, 1, 2, 3, 4, 5, -1 };
  int sp_seq4_1[] = { 6, 7, 8, 9, 10, 11, -1 };  
  int sp_seq4_2[] = { 12, -1 };    
  crono_sp.add_state(0,5,0,sp_seq4_0);
  crono_sp.add_state(1,5,0,sp_seq4_1);  
  crono_sp.add_state(2,5,0,sp_seq4_2);  
  crono_sp.set_state(0);
  scene.add_physics_obj(&crono);
  
  //creating walls and other physics stuff
  lg_vector gravity_direction;
  lg_phy_force_const gravity;
  lg_phy_wall_line_stopAll wall2, wall3, wall4, wall5;
  lg_phy_wall_line_allowDirection wall1;
  lg_phy_wall_polygon overfloor;

  //sets the gravity downwards.
  gravity_direction.set(0.0f, 1.0f, 0.0f);  //(x,y,z) <i,j,k>, you get the idea...
  gravity.set(&gravity_direction, 2000.0f);
  
  //adds gravity to the scene...
  scene.add_force(&gravity);
  
  //sets up a wall that lets you through from the bottom
  wall1.set(155, 430, 1445, 430, 0);
  wall1.set_allowed(0.0f, -1.0f); // this is the direction you're allowed to pass from
  
  //sets the coordinates of the floors
  wall2.set(-1000, 515, 2000, 515, 0);
  wall3.set(505, 345, 1090, 345, 0);
  wall4.set(0, 330, 175, 430, 0);

  //the invisible vertical wall...
  wall5.set(505, 430, 505, 345, 10); 

  //This is the floating rectangle thing..
  overfloor.set_sens(15);
  overfloor.start();
  overfloor.add_node(1085, 220);
  overfloor.add_node(1085, 178);
  overfloor.add_node(1273, 178);
  overfloor.add_node(1273, 220);
  overfloor.add_node(1085, 220);
  overfloor.end();

  //add the physics objects to the scene
  scene.add_wall(&wall1);
  scene.add_wall(&wall2);
  scene.add_wall(&wall3);
  scene.add_wall(&wall4);
  scene.add_wall(&overfloor);
  scene.add_wall(&wall5);
  
  //some stuff needed for the main loop
  int done = 0;                //determine whether the game is over, or not
  int keypressed = 0;
  SDL_Event event;
  lg_object *mouse, *objid;
  int start, elapsed;        //timer variables
  int offset_x, offset_y;    //camera offset
  int crono_w, crono_h;
  Uint8 *keys;
  
  //doing some maintenance stuff
  start = elapsed = 1;
  lg_get_mouseobj(&mouse);
  crono.get_speed_handler(&crono_velocity);
  crono_velocity->set(0.0f,0.0f,0.0f);
  crono.get_size(&crono_w, &crono_h);

  int mx, my, mx_scene, my_scene;
  SDL_ShowCursor(0);
		  
  //main loop
  do {
      start = SDL_GetTicks();
	  
	  SDL_GetMouseState(&mx, &my);  	  
	  
	  //get the current mouse position on the current scenescene
	  lg_set_mousepos_at_scene(&scene);
	  mouse->get_pos(&mx_scene, &my_scene);
	  
	  //SDL stuff
      SDL_PollEvent(&event);
      switch (event.type) {
        case SDL_QUIT:
	      done=1;
	    break;	

		//checking if quit demo button is being pressed
	    case SDL_MOUSEBUTTONDOWN:
	      objid = scene.get_obj_bypos(mx_scene, my_scene);
	      if (objid == &button)
	        done=1;
	    break;
      }
      
      //here's where you can put the code to process game logic.
	  
	  //Get keyboard input
	  keys = SDL_GetKeyState(NULL);
	  
	  //Do key processing
	  if (keys[SDLK_RIGHT]==SDL_PRESSED) {
		if (crono_sp.get_state() != 1)
				crono_sp.set_state(1);
		float x,y,z;
		crono_velocity->get(&x,&y,&z);
		crono_velocity->set(300.0f, y, 0.0f);
	  }
	  else if (keys[SDLK_LEFT]==SDL_PRESSED) {
		if (crono_sp.get_state() != 0)
				crono_sp.set_state(0);
		float x,y,z;
		crono_velocity->get(&x,&y,&z);
		crono_velocity->set(-300.0f, y, 0.0f);
	  }	
	  else { //no keys are being pressed. therefore, stop crono from moving sideways
		float x4, y4, z4;
		crono_velocity->get(&x4, &y4, &z4);
		crono_velocity->set(0.0f, y4, 0.0f);
		crono_sp.set_state(2);
	  }
	  //jump!
	  if (keys[SDLK_SPACE]==SDL_PRESSED && SDL_GetTicks() > keypressed+400) {
		keypressed = SDL_GetTicks();
		float x,y,z;
		crono_velocity->get(&x,&y,&z);	
		crono_velocity->set(x, -700.0f, 0.0f);
   	  } 

	  
	  //if it's time, process the scene's physics
	  //Process physics only if some time has passed since the beginning of the game loop
	  //and if a lag didn't occur....
	  if (elapsed > 0 && elapsed < 40)
		scene.process_physics( (float)elapsed/1000.0f );

	  //focus camera on crono
  	  crono.get_pos(&offset_x, &offset_y);
   	  offset_x-=(w/2);
	  offset_y-=(h/2);
	  offset_x+=(crono_w/2);
	  offset_y+=(crono_h/2);
	  scene.set_cam_pos(offset_x, offset_y);

      //Render the scene
      scene.render();
      
	  glLoadIdentity();	
	  lg_set_mousepos(mx, my);
      lg_print_mouse();

      //Do OpenGL stuff
      SDL_GL_SwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  elapsed = SDL_GetTicks() - start;
	  //keep it running at a constant framerate
	  while(elapsed < 15)
	  	elapsed = SDL_GetTicks() - start;
	  
  } while(!done);

  TTF_Quit();
  SDL_Quit();  
}





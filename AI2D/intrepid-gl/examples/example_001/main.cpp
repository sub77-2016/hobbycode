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
  lg_scene scene;
  lg_scene_param scenep;
  
  //setting up the scene
  scenep.start_x = 0;
  scenep.start_y = 0;
  scenep.scene_w = w;
  scenep.scene_h = h;
  scenep.canvas_w = w;
  scenep.canvas_h = h;
  scenep.bits = 32;
  scene.set(&scenep);
  
  //creating hand object
  lg_object hand;
  lg_surface hand_img;
  lg_object_param objp;
  lg_color transp_color;
  
  //setting up a hand object
  objp.id = 0;
  objp.active = LGOBJ_ACTIVE;
  objp.visible = LGOBJ_VISIBLE;
  objp.x = 50;
  objp.y = 100;
  objp.ang = 0.0f;

  //we want to consider blue as the transparent color, so we set it to:
  transp_color.r = 0;
  transp_color.g = 0;
  transp_color.b = 255;

  //load the hand bitmap
  hand_img.load_image("cursor.bmp", LF_CO_TRANSPARENT, &transp_color);

  //initialize the object
  hand.set(&objp, &scene);
  //set the video handler for the hand object    
  hand.set_video_handler(&hand_img);

  //adds the hand to the scene  
  scene.add_object(&hand);
  
  int done=0;
  SDL_Event event;
  
  //main loop
  do {
      //SDL stuff
      SDL_PollEvent(&event);
      switch (event.type) {
        case SDL_QUIT:
	  done=1;
	  break;	
      }
      
      //here's where you can put the code to process game logic.
      
      //Render the scene
      scene.render();
      
      //Do OpenGL stuff
      SDL_GL_SwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } while(!done);

  TTF_Quit();
  SDL_Quit();  
}





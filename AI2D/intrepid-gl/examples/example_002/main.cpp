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
  w = 298; 
  h = 384; 
		
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
  scenep.fname = "fundo.bmp"; 
  scene.set(&scenep);
  
  //creating hand object
  lg_object monster, monster2, message;
  lg_surface monster_img, monster2_img, message_img;
  lg_object_param objp;
  lg_color transp_color;
  
  // *********************************************************
  // Sets the first monster
  
  objp.id = 0;
  objp.active = LGOBJ_ACTIVE;
  objp.visible = LGOBJ_VISIBLE;
  objp.x = 50;
  objp.y = 100;
  objp.ang = 0.0f;

  //we want to consider blue as the transparent color, so we set it to:
  transp_color.r = 255;
  transp_color.g = 255;
  transp_color.b = 255;

  //load the hand bitmap
  monster_img.load_image("monster.bmp", LF_CO_TRANSPARENT, &transp_color);

  //initialize the object
  monster.set(&objp, &scene);
  //set the video handler for the hand object    
  monster.set_video_handler(&monster_img);

  //adds the hand to the scene  
  scene.add_object(&monster);
  
  // *********************************************************
  // Sets the second monster

  objp.id = 1;
  objp.active = LGOBJ_ACTIVE;
  objp.visible = LGOBJ_VISIBLE;
  objp.x = 100;
  objp.y = 150;
  objp.ang = 0.0f;

  //we want to consider blue as the transparent color, so we set it to:
  transp_color.r = 255;
  transp_color.g = 255;
  transp_color.b = 255;

  //load the hand bitmap
  monster2_img.load_image("monster2.bmp", LF_CO_TRANSPARENT, &transp_color);

  //initialize the object
  monster2.set(&objp, &scene);
  //set the video handler for the hand object    
  monster2.set_video_handler(&monster2_img);

  //adds the hand to the scene  
  scene.add_object(&monster2);

  // *********************************************************
  // Sets the message

  objp.id = 2;
  objp.active = LGOBJ_ACTIVE;
  objp.visible = LGOBJ_VISIBLE;
  objp.x = 0;
  objp.y = 0;
  objp.ang = 0.0f;

  TTF_Font *font;
  font = TTF_OpenFont( "lazy.ttf", 28);

  //initialize the object
  message.set(&objp, &scene);
  //set the video handler for the hand object    
  message.set_video_handler(&message_img);

  //adds the hand to the scene  
  scene.add_object(&message);

  // *********************************************************

  int done=0;
  SDL_Event event;
  
  int x, y;

  char msg_coll[] = "collision !!!";
  char msg_nocoll[] = "no collision";
  
  //main loop
  do {
	  monster.get_pos(&x, &y);
		  
      //SDL stuff
      SDL_PollEvent(&event);
      switch (event.type) {
			  
        case SDL_QUIT:
	      done=1;
		  break;
		  
		case SDL_KEYDOWN:
		  switch( event.key.keysym.sym ) {
            case SDLK_UP: y--; break;
            case SDLK_DOWN: y++; break;
            case SDLK_LEFT: x--; break;
            case SDLK_RIGHT: x++; break;    
			case SDLK_ESCAPE: done=1;break;
          }
	
      }

	  monster.set_pos(x, y);
	  
      if (obj_check_coll_pixel(&monster, &monster2) )
       	message_img.create_text(msg_coll, font);
	  else
      	message_img.create_text(msg_nocoll, font);
        
      //here's where you can put the code to process game logic.
      
      //Render the scene
      scene.render();

	  // clean up the text memory
	  message_img.destroy();
      
      //Do OpenGL stuff
      SDL_GL_SwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  SDL_Delay(10);
  } while(!done);

  TTF_Quit();
  SDL_Quit();  
}

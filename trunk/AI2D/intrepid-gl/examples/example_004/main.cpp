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
  // loading the pack file
  lg_pack_file *pack;

  pack = lg_open_pack("pack.pak");
  if (!pack) // in version 0.2 no verification is being done if the pack was opened successfully, but
		     // always put this "if" because this bug will be corrected
		 exit(1);
		
  //creating menu object
  lg_menu menu;
  lg_scene_param menup;
  
  //setting up the menu
  menup.start_x = 0;
  menup.start_y = 0;
  menup.scene_w = w;
  menup.scene_h = h;
  menup.canvas_w = w;
  menup.canvas_h = h;
  menup.bits = 32;
  menu.set(&menup);

  SDL_Surface *menu_back;
  menu_back = lg_pack_to_surface(pack, "back.bmp");
  menu.set_back(menu_back);
  SDL_FreeSurface(menu_back);
  
  //creating mouse 
  lg_surface mouse_img;
  lg_color blue;

  //we want to consider blue as the transparent color, so we set it to:
  blue.r = 0;
  blue.g = 0;
  blue.b = 255;

  lg_color red = {255, 0, 0};
  lg_color white = {255, 255, 255};

  //load the cursor bitmap
  mouse_img.load_image(pack, "cursor.bmp", LF_CO_TRANSPARENT, &blue);
  
  //setup the mouse
  lg_set_mouse(&mouse_img);

  /*****************************************************/
  
  //creating checkbox
  lg_checkbox mybox, box2;
  lg_checkbox_param objp;
		  
  objp.obj.id = 0;
  objp.obj.active = LGOBJ_ACTIVE;
  objp.obj.visible = LGOBJ_VISIBLE;
  objp.obj.x = 50;
  objp.obj.y = 55;
  objp.obj.ang = 0.0f;

  objp.flags = RENDER_OPENGL | OPENGL_BACK_SQUARE_SOLID;
//  objp.flags = RENDER_OPENGL | OPENGL_BACK_CIRC_SOLID;
  objp.size = 20;
  
  objp.bg_color.r = 0;
  objp.bg_color.g = 100;
  objp.bg_color.b = 100;

  objp.border_color.r = 255;
  objp.border_color.g = 255;
  objp.border_color.b = 255;

  objp.thickness = 1;

  mybox.set_checkbox(&objp);

  menu.add_input(&mybox);
  
  objp.obj.id = 3;
  objp.obj.x = 120;
  box2.set_checkbox(&objp);
  menu.add_input(&box2);
  
  /*****************************************************/
  
  //creating radiobuttons

  lg_radioClass radio_class1;
  lg_radiobutton radio1, radio2;
  lg_radiobutton_class_param rclass;
  lg_radiobutton_param rparam; 
		  
  rclass.id = 0;

//  rclass.flags = RENDER_OPENGL | OPENGL_BACK_SQUARE_SOLID;
  rclass.flags = RENDER_OPENGL | OPENGL_BACK_CIRC_SOLID;
  rclass.size = 10;
  
  rclass.bg_color.r = 0;
  rclass.bg_color.g = 100;
  rclass.bg_color.b = 100;

  rclass.border_color.r = 255;
  rclass.border_color.g = 255;
  rclass.border_color.b = 255;

  rclass.thickness = 1;

  radio_class1.set(&rclass);

/////////////

  rparam.obj.id = 1;
  rparam.obj.active = LGOBJ_ACTIVE;
  rparam.obj.visible = LGOBJ_VISIBLE;
  rparam.obj.x = 50;
  rparam.obj.y = 150;
  rparam.obj.ang = 0.0f;

  rparam.value = 1;

  radio1.set_radio(&rparam, &radio_class1);
  
  menu.add_input(&radio1);

  rparam.obj.id = 2;
  rparam.obj.x = 120;
  rparam.value = 2;
  radio2.set_radio(&rparam, &radio_class1);
  menu.add_input(&radio2);
 
  /*****************************************************/
  
  //creating textbox
  lg_textbox_param tparam;
  lg_textbox text1;
  TTF_Font *font;
  font = lg_pack_to_font(pack, "lazy.ttf", 14);
  if (!font)
		 return;

  tparam.obj.id = 5;
  tparam.obj.active = LGOBJ_ACTIVE;
  tparam.obj.visible = LGOBJ_VISIBLE;
  tparam.obj.x = 130;
  tparam.obj.y = 450;
  tparam.obj.ang = 0.0f;
  
//  tparam.flags = RENDER_OPENGL | OPENGL_BACK_SQUARE_SOLID;
  tparam.flags = RENDER_OPENGL | OPENGL_BACK_SQUARE;
  tparam.w = 150;
  tparam.h = 20;
  tparam.maxChars = 30;
  tparam.font = font;
  
  tparam.bg_color.r = 0;
  tparam.bg_color.g = 100;
  tparam.bg_color.b = 100;

  tparam.border_color.r = 255;
  tparam.border_color.g = 255;
  tparam.border_color.b = 255;

  tparam.font_color.r = 255;
  tparam.font_color.g = 255;
  tparam.font_color.b = 255;
  
  tparam.thickness = 1;

  text1.set_textbox(&tparam);

  menu.add_input(&text1);
  
  /*****************************************************/
  
  //creating button
  lg_button button1;
  lg_button_param buttonp;
		  
  buttonp.obj.id = 8;
  buttonp.obj.active = LGOBJ_ACTIVE;
  buttonp.obj.visible = LGOBJ_VISIBLE;
  buttonp.obj.x = 100;
  buttonp.obj.y = 200;
  buttonp.obj.ang = 0.0f;

  buttonp.font = font;
  buttonp.text = "reset radio";
  
  buttonp.flags = RENDER_OPENGL | OPENGL_BACK_SQUARE_SOLID;
//  buttonp.flags = RENDER_OPENGL | OPENGL_BACK_CIRC_SOLID;
  buttonp.w = 100;
  buttonp.h = 30;
/////////
  buttonp.OUTbg_color.r = 0;
  buttonp.OUTbg_color.g = 100;
  buttonp.OUTbg_color.b = 100;

  buttonp.OUTborder_color.r = 255;
  buttonp.OUTborder_color.g = 255;
  buttonp.OUTborder_color.b = 255;
  
  buttonp.OUTfont_color.r = 255;
  buttonp.OUTfont_color.g = 255;
  buttonp.OUTfont_color.b = 255;
////////////
  buttonp.OVERbg_color.r = 0;
  buttonp.OVERbg_color.g = 150;
  buttonp.OVERbg_color.b = 150;

  buttonp.OVERborder_color.r = 255;
  buttonp.OVERborder_color.g = 255;
  buttonp.OVERborder_color.b = 255;
  
  buttonp.OVERfont_color.r = 255;
  buttonp.OVERfont_color.g = 255;
  buttonp.OVERfont_color.b = 255;
////////////
  buttonp.CLICKbg_color.r = 0;
  buttonp.CLICKbg_color.g = 250;
  buttonp.CLICKbg_color.b = 250;

  buttonp.CLICKborder_color.r = 255;
  buttonp.CLICKborder_color.g = 255;
  buttonp.CLICKborder_color.b = 255;
  
  buttonp.CLICKfont_color.r = 255;
  buttonp.CLICKfont_color.g = 255;
  buttonp.CLICKfont_color.b = 255;
////////////
  buttonp.thickness = 1;

  button1.set_button(&buttonp);

  menu.add_input(&button1);
  
  /*****************************************************/
  // printing the text
  lg_object t_options, t_op1, t_op2, t_sex, t_male, t_female, t_name;
  lg_surface v_options, v_op1, v_op2, v_sex, v_male, v_female, v_name;
  lg_object_param objp2;
  
  // Sets the message

  objp2.id = 100;
  objp2.active = LGOBJ_ACTIVE;
  objp2.visible = LGOBJ_VISIBLE;
  objp2.x = 60;
  objp2.y = 30;
  objp2.ang = 0.0f;

  lg_color fcolor = { 255, 255, 255 };
  
  t_options.set(&objp2, &menu);
  t_options.set_video_handler(&v_options);
  v_options.create_text("Options", font, &fcolor);
  menu.add_object(&t_options);
  
  objp2.x = 75;
  objp2.y = 55;
  objp2.id = 101;
  t_op1.set(&objp2, &menu);
  t_op1.set_video_handler(&v_op1);
  v_op1.create_text("Opt1", font, &fcolor);
  menu.add_object(&t_op1);

  objp2.x = 150;
  objp2.y = 55;
  objp2.id = 102;
  t_op2.set(&objp2, &menu);
  t_op2.set_video_handler(&v_op2);
  v_op2.create_text("Opt2", font, &fcolor);
  menu.add_object(&t_op2);

  objp2.x = 50;
  objp2.y = 125;
  objp2.id = 103;
  t_sex.set(&objp2, &menu);
  t_sex.set_video_handler(&v_sex);
  v_sex.create_text("Sex", font, &fcolor);
  menu.add_object(&t_sex);

  objp2.x = 75;
  objp2.y = 150;
  objp2.id = 104;
  t_male.set(&objp2, &menu);
  t_male.set_video_handler(&v_male);
  v_male.create_text("Male", font, &fcolor);
  menu.add_object(&t_male);

  objp2.x = 150;
  objp2.y = 150;
  objp2.id = 105;
  t_female.set(&objp2, &menu);
  t_female.set_video_handler(&v_female);
  v_female.create_text("Female", font, &fcolor);
  menu.add_object(&t_female);

  objp2.x = 85;
  objp2.y = 450;
  objp2.id = 106;
  t_name.set(&objp2, &menu);
  t_name.set_video_handler(&v_name);
  v_name.create_text("Name:", font, &fcolor);
  menu.add_object(&t_name);
  
  /*****************************************************/
  // printing the status text
  lg_object ts_op1, ts_op2, ts_sex, ts_name;
  lg_surface vs_op1, vs_op2, vs_sex, vs_name;

  objp2.x = 300;
  objp2.y = 100;
  objp2.id = 200;
  ts_name.set(&objp2, &menu);
  ts_name.set_video_handler(&vs_name);
  menu.add_object(&ts_name);
  
  objp2.x = 300;
  objp2.y = 120;
  objp2.id = 210;
  ts_op1.set(&objp2, &menu);
  ts_op1.set_video_handler(&vs_op1);
  menu.add_object(&ts_op1);
  
  objp2.x = 300;
  objp2.y = 140;
  objp2.id = 211;
  ts_op2.set(&objp2, &menu);
  ts_op2.set_video_handler(&vs_op2);
  menu.add_object(&ts_op2);

  objp2.x = 300;
  objp2.y = 160;
  objp2.id = 212;
  ts_sex.set(&objp2, &menu);
  ts_sex.set_video_handler(&vs_sex);
  menu.add_object(&ts_sex);

  char opt1_on[] = "opt1 on";
  char opt1_off[] = "opt1 off";

  char opt2_on[] = "opt2 on";
  char opt2_off[] = "opt2 off";

  char male_[] = "you are a man";
  char female_[] = "you are a woman";
  char donot_[] = "you are ...";

  char name_[] = "your name is ";
  char name_2[100];

  /*****************************************************/  
  
  int done=0;
  SDL_Event event;
  lg_object *mouse;
  int mx, my;
  Uint8 *keys;
  
  lg_get_mouseobj(&mouse);

  SDL_EnableKeyRepeat(200, 100);
  
  SDL_ShowCursor(0);
printf("%i\n", SDLK_a);
  //main loop
  do {

	  SDL_GetMouseState(&mx, &my);  	  
	  keys = SDL_GetKeyState(NULL);

      //SDL stuff

	  lg_set_mousepos_at_scene(&menu);

      while (done==0 && SDL_PollEvent(&event))
	  menu.process(keys, mx, my, &event);
      switch (event.type) {
      	case SDL_QUIT:
	  		done=1;
	  break;	
      }
      
	  if (button1.check_clicked()) {
		  button1.reset();
		  radio_class1.uncheck_all();
	  }
	  
      if (mybox.get_checked())  vs_op1.create_text(opt1_on, font, &fcolor);
	  else vs_op1.create_text(opt1_off, font, &fcolor);
      
      if (box2.get_checked())  vs_op2.create_text(opt2_on, font, &fcolor);
	  else vs_op2.create_text(opt2_off, font, &fcolor);
	  
      if (radio1.get_checked())  vs_sex.create_text(male_, font, &fcolor);
	  else if (radio2.get_checked()) vs_sex.create_text(female_, font, &fcolor);
	  else vs_sex.create_text(donot_, font, &fcolor);

	  strcpy(name_2, name_);
	  strcat(name_2, text1.get_value());
	  vs_name.create_text(name_2, font, &fcolor);
	  
      //Render the menu
	  menu.render();

//	  lg_gldraw_Circ (100, 100, 200, &blue);
//	  lg_gldraw_solidCirc (200, 100, 200, &blue);	  

	  //Render the mouse
	  glLoadIdentity();	
	  lg_set_mousepos(mx, my);
      lg_print_mouse();

      //Do OpenGL stuff
      SDL_GL_SwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  SDL_Delay(20);
			  
  } while(!done);

  // close the pack file
  lg_close_pack(pack);
  
  TTF_Quit();
  SDL_Quit();  
}





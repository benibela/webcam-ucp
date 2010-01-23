#include <iostream>
#include <GL/glut.h>

#include "app.h"

// function declarations
void keyboard_func(unsigned char key, int x, int y);
void display_func();
void idle_func();
void resize_func(int w, int h);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void handle_main_menu(int selection);
void handle_model_menu(int selection);
void handle_draw_mode_menu(int selection);
void handle_separation_menu(int selection);
void handle_camera_output_menu(int selection);
void handle_camera_filter_menu(int selection);

// main application 
App main_app; 

enum MENU_ENTRIES {MENU_ANIMATION = 0, MENU_LIGHTING, MENU_CENTERED_PROJECTION, MENU_KILL_BACKGROUND, MENU_CALIBRATE_POSITION };

int main(int argc, char** argv)
{
  // initialize glut
  glutInit(&argc, argv);

  // specify the display mode to be RGBA, depth buffer and double buffering 
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );

  // define the size
  glutInitWindowSize(1024,576);

  // create the window and set title 
  glutCreateWindow("OpenGL - Anaglyph");

  // callbacks
  glutDisplayFunc(display_func);
  glutKeyboardFunc(keyboard_func);
  glutReshapeFunc(resize_func);
  glutIdleFunc(idle_func);
  glutMouseFunc(mouse_func);
  glutMotionFunc(motion_func);

  int draw_mode_menu = glutCreateMenu(handle_draw_mode_menu);
  glutAddMenuEntry("Wireframe",App::WIREFRAME);
  glutAddMenuEntry("Solid",App::SOLID);

  int model_menu = glutCreateMenu(handle_model_menu);
  glutAddMenuEntry("Teapot",App::TEAPOT);
  glutAddMenuEntry("Box",App::BOX);
  glutAddMenuEntry("Table",App::TABLE);
 
  int separation_menu = glutCreateMenu(handle_separation_menu);
  glutAddMenuEntry("Red/Green",App::RED_GREEN);
  glutAddMenuEntry("Red/Cyan",App::RED_CYAN);
  glutAddMenuEntry("Full Color/No 3D",App::FULL_COLOR);

  int cameraOutput_menu = glutCreateMenu(handle_camera_output_menu);
  glutAddMenuEntry("None",0);
  glutAddMenuEntry("Panel",1);
  glutAddMenuEntry("Scene",2);
  glutAddMenuEntry("Panel+Scene",3);

  int cameraFilter_menu = glutCreateMenu(handle_camera_filter_menu);
  glutAddMenuEntry("Base image",RealCamera::BASE_IMAGE);
  glutAddMenuEntry("Tracked Cyan",RealCamera::TRACKED_CYAN);
  glutAddMenuEntry("Tracked Red",RealCamera::TRACKED_RED);
 
  glutCreateMenu(handle_main_menu);
  glutAddSubMenu("Model Type",model_menu);
  glutAddSubMenu("Draw Mode",draw_mode_menu);
  glutAddSubMenu("Eye Separation Filter",separation_menu);
  glutAddSubMenu("Camera Output To",cameraOutput_menu);
  glutAddSubMenu("Camera Filter",cameraFilter_menu);
  glutAddMenuEntry("Toggle Animation",MENU_ANIMATION);
  glutAddMenuEntry("Toggle Lighting",MENU_LIGHTING);
  glutAddMenuEntry("Toggle centered Projection",MENU_CENTERED_PROJECTION);
  glutAddMenuEntry("Kill Background",MENU_KILL_BACKGROUND);
  glutAddMenuEntry("Calibrate Position",MENU_CALIBRATE_POSITION);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
 
  // setup application
  main_app.setup();
 
  // print out some information about program control
  std::cout << "Usage\n"
            << "Right mouse button opens context menu for some settings.\n"
            << "Key \'q\' quits application\n";

  
  // enter the main loop
  glutMainLoop();

  return 0;
}

void handle_main_menu(int selection)
{
  switch(selection)
  {
    case MENU_ANIMATION:
      main_app.toggle_animation();
      break;
    case MENU_LIGHTING:
      main_app.toggle_lighting();
      break;
    case MENU_CENTERED_PROJECTION:
      main_app.toggle_centered_projection();
      break;
    case MENU_KILL_BACKGROUND:
      main_app.realCamera()->killBackground();
      break;
    case MENU_CALIBRATE_POSITION:
      main_app.calibrate_position();
      break;
    default:
      break;
  }
}

void handle_camera_filter_menu(int selection){
	main_app.set_camera_filter((RealCamera::ImageId) selection);
}

void handle_separation_menu(int selection)
{
  main_app.set_separation_mode(selection);
}

void handle_draw_mode_menu(int selection)
{
  main_app.set_draw_mode(selection);
}

void handle_model_menu(int selection)
{
  main_app.set_model(selection);
}
void handle_camera_output_menu(int selection){
  main_app.set_camera_output_mode(selection & 1, selection & 2);
}

void display_func()
{
  main_app.display();
}

void resize_func(int w, int h)
{
  main_app.resize(w,h);
}

void keyboard_func(unsigned char key, int x, int y)
{
  main_app.keyboard(key,x,y);
}

void mouse_func(int button, int state, int x, int y)
{
  main_app.mouse(button,state,x,y);
}

void motion_func(int x, int y)
{
  main_app.motion(x,y);
}

void idle_func()
{
  // trigger redraw
  glutPostRedisplay();
}


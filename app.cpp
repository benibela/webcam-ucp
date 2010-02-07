#include <iostream>
#include <GL/glut.h>

#include "app.h"

#include "glh_linear.h"

using glh::vec3f;
using glh::vec4f;
using glh::vec2i;

App::App()
{
  m_camera_trans = vec3f(0.0f,0.0f,-5.0f);
  m_camera_trans_lag = vec3f(0.0f,0.0f,-5.0f);
  inertia = 10;
  rot_inertia = 10;

  m_model = TEAPOT;
  m_draw_mode = GL_LINE;
  m_separation_mode = RED_GREEN;
  
  m_last_time = 0.0f;
  m_rotation  = 0.0f;
  m_next_camera_time=0;
  m_next_framecheck_time=0;
  m_animation = true;
  m_lighting = true;
  
  m_showCamAsPanel=true;
  m_cameraFilter=RealCamera::BASE_IMAGE;
  m_ZeroTrackedCyanX=0.5f;
  m_ZeroTrackedCyanY=0.5f;
  m_ZeroTrackedCyanW=0.1f;
  m_ZeroTrackedCyanH=0.1f,
  m_ZeroTrackedRedX=0.5f;
  m_ZeroTrackedRedY=0.5f;
  m_ZeroTrackedRedW=0.1f;
  m_ZeroTrackedRedH=0.1f;
  m_ZeroTrackedX=0.5f;
  m_ZeroTrackedY=0.5f;
  m_ZeroTrackedW=0.1f;
  m_ZeroTrackedH=0.1f;
  
  m_user_centered_projection=true;
}

App::~App()
{
}
  
void App::toggle_animation()
{
  m_animation = !m_animation;
}

void App::toggle_lighting()
{
  m_lighting= !m_lighting;
  if(m_lighting)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);
}

void App::toggle_centered_projection(){
	m_user_centered_projection=!m_user_centered_projection;
}

void App::setup()
{
  // setup OpenGL stuff
  setup_gl();

  m_realcamera.init(0,0);

}

void App::setup_gl()
{
   // define the color we use to clearscreen 
  glClearColor(0.0,0.0,0.0,0.0);

  // use z buffer
  glEnable(GL_DEPTH_TEST);
  
  // no smoothing of lines, points and polygons for better eye separation
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH); 
  // no dithering of colors for better eyes separation
  glDisable(GL_DITHER);

  // light setting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  const vec4f ambient(0.6f,0.6f,0.6f,1.0f);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
  
  // interior of objects should be visible
  glDisable(GL_CULL_FACE);
  
  glPolygonMode(GL_FRONT_AND_BACK,m_draw_mode);

  m_last_time = glutGet(GLUT_ELAPSED_TIME);

  // check for any OpenGL errors
  check_errors_gl("App::setup_gl");
 
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1,&m_camTexture);
  glBindTexture(GL_TEXTURE_2D, m_camTexture);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glBindTexture(GL_TEXTURE_2D,0);
  glDisable(GL_TEXTURE_2D);
}
  
void App::set_draw_mode(const unsigned int draw_mode)
{
  if(draw_mode == WIREFRAME)
    m_draw_mode = GL_LINE;
  else
    m_draw_mode = GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK,m_draw_mode);

}

void App::set_model(const unsigned int model)
{
  m_model = model;
}
  
void App::set_separation_mode(const unsigned int separation_mode)
{
  m_separation_mode = separation_mode;
}

void  App::set_camera_output_mode(const bool showPanel, const bool showScene){
  m_showCamInScene=showScene;
  m_showCamAsPanel=showPanel;
}

void App::set_camera_filter(const RealCamera::ImageId imageId){
	m_cameraFilter=imageId;
}

void App::calibrate_position(){
  m_ZeroTrackedCyanX=m_trackedCyanX;
  m_ZeroTrackedCyanY=m_trackedCyanY;
  m_ZeroTrackedCyanW=m_trackedCyanW;
  m_ZeroTrackedCyanH=m_trackedCyanH;
  m_ZeroTrackedRedX=m_trackedRedX;
  m_ZeroTrackedRedY=m_trackedRedY;
  m_ZeroTrackedRedW=m_trackedRedW;
  m_ZeroTrackedRedH=m_trackedRedH;
  m_ZeroTrackedX=m_trackedX;
  m_ZeroTrackedY=m_trackedY;
  m_ZeroTrackedW=m_trackedW;
  m_ZeroTrackedH=m_trackedH;
}

void App::draw_teapot()
{
  glutSolidTeapot(1.0f);
}

void App::draw_box()
{
	float size = 1.0f;
	//copied from glut source
  static GLfloat n[6][3] =
  {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

  for (i = 5; i >= 0; i--) {
    glBegin(GL_QUADS);
    glNormal3fv(&n[i][0]);
    glTexCoord2f(0,0);
    glVertex3fv(&v[faces[i][0]][0]);
    glTexCoord2f(0,1);
    glVertex3fv(&v[faces[i][1]][0]);
    glTexCoord2f(1,1);
    glVertex3fv(&v[faces[i][2]][0]);
    glTexCoord2f(1,0);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}

void App::draw_table()
{
  glPushMatrix();
  glScalef(1.0f/100.f,1.0f/100.f,1.0f/100.0f);

  // table top
  glPushMatrix();
    glTranslatef(0.0f,-1.5f,0.0f);
    glScalef(150.0f,3.0f,80.0f);
    draw_box();
  glPopMatrix();

  // table legs
  glPushMatrix();
    glTranslatef(0.0f,-38.0f,0.0f);
    // leg
    glPushMatrix();
      glTranslatef(-72.5f,0.0f,37.5f);
      glScalef(5.0f,70.0f,5.0f);
      draw_box();
    glPopMatrix();
    // leg
    glPushMatrix();
      glTranslatef(72.5f,0.0f,37.5f);
      glScalef(5.0f,70.0f,5.0f);
      draw_box();
    glPopMatrix();
    // leg
    glPushMatrix();
      glTranslatef(-72.5f,0.0f,-37.5f);
      glScalef(5.0f,70.0f,5.0f);
      draw_box();
    glPopMatrix();
    // leg
    glPushMatrix();
      glTranslatef(72.5f,0.0f,-37.5f);
      glScalef(5.0f,70.0f,5.0f);
      draw_box();
    glPopMatrix();
  glPopMatrix();

  glPopMatrix();
  
}


void App::draw_scene()
{
  // choose rendering according to selected model type
  glColor3f(1.0f,1.0f,1.0f);
  switch(m_model)
  {
    case BOX:
    {
      draw_box();
    } break;
    case TABLE:
    {
      draw_table();
    } break;
    case TEAPOT:
    default:
      {
        draw_teapot();
      }
    break;
  }
}

void App::display()
{
  // compute frame time
  const float time = glutGet(GLUT_ELAPSED_TIME)/1000.0f;	  
  const float frame_time = time - m_last_time;
  m_last_time = time;

  m_curFps++;
  if (m_next_framecheck_time<time){
		std::cout << "fps: "<<m_curFps / 5.0f << "\n";
		m_next_framecheck_time=time+5;
		m_curFps=0;
	}

  if (m_next_camera_time<time) {
    m_realcamera.nextFrame();
    m_realcamera.trackGlasses(m_trackedX, m_trackedY, m_trackedW, m_trackedH, 
                       m_trackedCyanX, m_trackedCyanY, m_trackedCyanW, m_trackedCyanH,
                       m_trackedRedX, m_trackedRedY, m_trackedRedW, m_trackedRedH);
    m_next_camera_time = time + 0.1;
    if (m_showCamInScene || m_showCamAsPanel){
	    glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, m_camTexture);	  
      IplImage* image=m_realcamera.image(m_cameraFilter);
//  std::cout << image->width << " " << image->height << " " << image->imageSize << "\n";
		  if (m_cameraFilter==RealCamera::BASE_IMAGE) 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image->width, image->height, 0, GL_BGR, GL_UNSIGNED_BYTE,  image->imageData);
      else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, image->width, image->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,  image->imageData);
      
      check_errors_gl("genteximage");
      if (!m_showCamInScene) {
        glBindTexture(GL_TEXTURE_2D, 0);	  
        glDisable(GL_TEXTURE_2D);
      }
    }
  } else {
      if (m_showCamInScene) {
        glBindTexture(GL_TEXTURE_2D, m_camTexture);	  
        glEnable(GL_TEXTURE_2D);
      }
  }
  

  const float rot_speed = 10.0f;
  if(m_animation)
    m_rotation += frame_time*rot_speed;

  // GL_COLOR_BUFFER_BIT stores GL_DRAW_BUFFER setting
  // GL_COLOR_BUFFER_BIT stores color mode write masks
  glPushAttrib(GL_COLOR_BUFFER_BIT);

  // clear color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
  // set transformations in modelview matrix to identiy
  glLoadIdentity();

  glPushMatrix();
    // apply viewing transformations
    m_camera_trans_lag += (m_camera_trans - m_camera_trans_lag) * inertia*frame_time;
    m_camera_rot_lag += (m_camera_rot - m_camera_rot_lag) * rot_inertia*frame_time;

    glRotatef(m_camera_rot_lag[0], 1.0, 0.0, 0.0);
    glRotatef(-m_camera_rot_lag[1], 0.0, 1.0, 0.0);
    glRotatef(-m_rotation, 0.0, 1.0, 0.0);
    glTranslatef(m_camera_trans_lag[0], -m_camera_trans_lag[1], m_camera_trans_lag[2]);

    // forward transformations to camera object
    GLfloat  mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv) ;
    m_camera.update(mv);
  glPopMatrix();
  
  // ----------------------------------------------
  // left eye 
  // ----------------------------------------------

  vec3f up=m_camera.view_up();
  vec3f right=m_camera.right_vector();
  if (m_separation_mode!=FULL_COLOR){	
		const float calibrationXColorCamImageToVR=0;
		const float calibrationYColorCamImageToVR=0;
		const float calibrationXBothCamImageToVR=-2;
		const float calibrationYBothCamImageToVR=-2;
		
    // red-green left
    if(m_separation_mode == RED_GREEN)
    {glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);}
    else // red-cyan left
    {glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);}

    glPushMatrix();
      // 2. left eye transformation 
      vec3f eye=m_camera.left_eye();
      vec3f focus=m_camera.focal_point();
      glLoadIdentity();
      float tempmat[16];
      
      m_camera.calc_centered_projection_matrix(m_trackedRedX-m_ZeroTrackedRedX,m_trackedRedY-m_ZeroTrackedRedY,tempmat);
      if (m_user_centered_projection){
        eye = eye + right*calibrationXColorCamImageToVR*(m_trackedRedX-m_ZeroTrackedRedX);
        eye = eye +  up*calibrationYColorCamImageToVR*(m_trackedRedY-m_ZeroTrackedRedY);
        eye = eye + right*calibrationXBothCamImageToVR*(m_trackedX-m_ZeroTrackedX);
        eye = eye +  up*calibrationYBothCamImageToVR*(m_trackedY-m_ZeroTrackedY);
        glMultMatrixf(tempmat);
      }
      gluLookAt(eye[0],eye[1],eye[2],focus[0],focus[1],focus[2],up[0],up[1],up[2]);
      draw_scene(); 
    glPopMatrix();


    // make sure all OpenGL draw calls are finished before proceeding to right eye
    glFlush();

    // ----------------------------------------------
    // right eye
    // ----------------------------------------------
  
    glClear(GL_DEPTH_BUFFER_BIT);
   
    // red-green right
    if(m_separation_mode == RED_GREEN)
    {glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);}
    // red-cyan right 
    else
    {glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);}

      m_camera.calc_centered_projection_matrix(m_trackedCyanX-m_ZeroTrackedCyanX,m_trackedCyanY-m_ZeroTrackedCyanY,tempmat);
 
    glPushMatrix();
      eye=m_camera.right_eye();
      //vec3f focus=camera.focal_point();
      glLoadIdentity();
      if (m_user_centered_projection){
        eye = eye + right*calibrationXColorCamImageToVR*(m_trackedCyanX-m_ZeroTrackedCyanX);
        eye = eye +  up*calibrationYColorCamImageToVR*(m_trackedCyanY-m_ZeroTrackedCyanY);
        eye = eye + right*calibrationXBothCamImageToVR*(m_trackedX-m_ZeroTrackedX);
        eye = eye +  up*calibrationYBothCamImageToVR*(m_trackedY-m_ZeroTrackedY);
        glMultMatrixf(tempmat);
      }
      gluLookAt(eye[0],eye[1],eye[2],focus[0],focus[1],focus[2],up[0],up[1],up[2]);
      // 2. right eye transformation 
      draw_scene();
    glPopMatrix();
  
  } else {
    glPushMatrix();
      vec3f eye=m_camera.view_position();
      vec3f focus=m_camera.focal_point();
      glLoadIdentity();
      gluLookAt(eye[0],eye[1],eye[2],focus[0],focus[1],focus[2],up[0],up[1],up[2]);
      // 2. right eye transformation 
      draw_scene();
    glPopMatrix();
	}
  glPopAttrib(); // GL_COLOR_BUFFER_BIT

  if (m_showCamAsPanel) {
	  glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	  glLoadIdentity();
	  gluOrtho2D(0, 2, 2, 0);

	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix();
	  glLoadIdentity();
	  
	  glDisable(GL_DEPTH_TEST);
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D,m_camTexture);
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_BLEND);
    glBlendColor(0.5f,0.5f,0.5f,0.5f);
    glBlendFunc(GL_CONSTANT_ALPHA,GL_ONE_MINUS_CONSTANT_ALPHA);

    glDisable(GL_LIGHTING);

	  //camera output
	  glBegin(GL_QUADS);
	  glTexCoord2f(0,1);
	  glVertex2f(0,1);
	  glTexCoord2f(0,0);
	  glVertex2f(0,0.0f);
	  glTexCoord2f(1,0);
	  glVertex2f(1.0f,0.0f);
	  glTexCoord2f(1,1);
	  glVertex2f(1.0f,1);
	  glEnd();

	  glBindTexture(GL_TEXTURE_2D,0);
	  glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //tracked cyan rect
	  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glColor4f(0,1,1,1);
	  glBegin(GL_QUADS);
	  glVertex2f(m_trackedCyanX,m_trackedCyanY);
	  glVertex2f(m_trackedCyanX,m_trackedCyanY+m_trackedCyanH);
	  glVertex2f(m_trackedCyanX+m_trackedCyanW,m_trackedCyanY+m_trackedCyanH);
	  glVertex2f(m_trackedCyanX+m_trackedCyanW,m_trackedCyanY);
	  glEnd();

    //tracked red rect
    glColor4f(1,0,0,1);
	  glBegin(GL_QUADS);
	  glVertex2f(m_trackedRedX,m_trackedRedY);
	  glVertex2f(m_trackedRedX,m_trackedRedY+m_trackedRedH);
	  glVertex2f(m_trackedRedX+m_trackedRedW,m_trackedRedY+m_trackedRedH);
	  glVertex2f(m_trackedRedX+m_trackedRedW,m_trackedRedY);
	  glEnd();

    //tracked glasses rect
    glColor4f(1,1,1,1);
	  glBegin(GL_QUADS);
	  glVertex2f(m_trackedX,m_trackedY);
	  glVertex2f(m_trackedX,m_trackedY+m_trackedH);
	  glVertex2f(m_trackedX+m_trackedW,m_trackedY+m_trackedH);
	  glVertex2f(m_trackedX+m_trackedW,m_trackedY);
	  glEnd();
    //std::cout << m_trackedX << ":"<<m_trackedY<<" " <<m_trackedW<< ":" <<m_trackedH <<"\n";

    glColor3f(1,1,1);
	  glEnable(GL_DEPTH_TEST);
    if(m_lighting)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

	  glPopMatrix();

	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();
	  glMatrixMode(GL_MODELVIEW);
  }



  glFlush();
  // swap front and back buffers
  glutSwapBuffers();

  // check for any OpenGL errors
  check_errors_gl("App::display");

  glPolygonMode(GL_FRONT_AND_BACK,m_draw_mode);
}

void App::resize(int w, int h)
{
  if (h == 0) h = 1; 
  if(w <= 0)
    w = 1;
  
  // propagate changes to camera
  m_camera.resize_screen(w,h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  const float near_plane = 0.1f;
  const float far_plane = 500.0f;
  gluPerspective(m_camera.aperture(),(float) w / (float) h, near_plane, far_plane);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);
  glLoadIdentity();

  check_errors_gl("App::resize");

}

void App::keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
  case 27: // ESC
  case 'q':   
    
      //exit(0);
    
    break;  
  default: break;
  }
}

void App::motion(int x, int y)
{
  const float dx = x - m_old_pos[0];
  const float dy = y - m_old_pos[1];

  if (m_button_state == 3) {
    // left+middle = zoom
    m_camera_trans[2] += (dy / 100.0) * 0.5 * fabs(m_camera_trans[2]);
  } 
  else if (m_button_state & 2) {
    // middle = translate
    m_camera_trans[0] += dx / 100.0;
    m_camera_trans[1] -= dy / 100.0;
  }
  else if (m_button_state & 1) {
    // left = rotate
    m_camera_rot[0] += dy / 5.0;
    m_camera_rot[1] += dx / 5.0;
  }

  m_old_pos = vec2i(x,y);
}

void App::mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN)
    m_button_state |= 1<<button;
  else if (state == GLUT_UP)
    m_button_state = 0;

  if ( button == 3)//GLUT_WHEEL_UP )
    m_camera_trans[2] -= 0.1f; 
    else if ( button == 4)// GLUT_WHEEL_DOWN )
    m_camera_trans[2] += 0.1f;
 
  m_old_pos[0] = x; m_old_pos[1] = y;
  
}

void check_errors_gl(const char* location , std::ostream& ostr ) 
{
  GLuint err_num;
  const char *err_str;
  while ( (err_num = glGetError()) ) 
  {
  ostr << "--OPENGL-ERROR-- ";
  err_str = reinterpret_cast<const char *>(gluErrorString(err_num));
  ostr << err_str; 

  if(location) 
    ostr << " at " << location;		
  ostr << std::endl;
  }
  return;
}

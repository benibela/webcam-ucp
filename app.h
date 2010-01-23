#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <iostream>

#include "camera.h"
#include "realcamera.h"

/// Check for OpenGL errors
/// \param location give info about location in code when calling this function
/// \param ostr print target for possible error messages (e.g. error stream std::cerr)
void check_errors_gl(const char* location=0 , std::ostream& ostr = std::cerr);

/// Main application 
class App
{
public:
  enum MODEL_TYPES { TEAPOT = 0, BOX, TABLE };
  enum DRAW_MODES { WIREFRAME = 0, SOLID};
  enum SEPARATION_MODES {RED_GREEN = 0, RED_CYAN, FULL_COLOR};

  /// constructor
  App();
  /// destructor
  virtual ~App();

  /// Choose which kind of model is used for scene rendering
  /// \param model identifier of one of the models from MODEL_TYPES
  void set_model(const unsigned int model);

  /// Change draw mode, choose between wireframe and solid rendering
  /// \param draw mode identifier is one of the types in DRAW_MODES
  void set_draw_mode(const unsigned int draw_mode);

  /// Change left/right eye separation color filters
  /// \param separation identifier is one of SEPARATION_MODES, like red/green and red/cyan
  void set_separation_mode(const unsigned int separation_mode);

  /// Change direct camera output
  void set_camera_output_mode(const bool showPanel, const bool showScene);

  void set_camera_filter(const RealCamera::ImageId imageId);
  
  void calibrate_position();
  /// Enable/Disable rotation animation
  void toggle_animation();

  /// Enable/Disable lighting of objects
  void toggle_lighting();

  void toggle_centered_projection();

  /// Implement keyboard events here
  /// \param key keycode of key event
  void keyboard(unsigned char key, int x, int y);
  
  /// This function is called once per frame, OpenGL draw calls belong here
  void display();
  
  /// This function is called whenever the corresponding window changes its size.
  /// \param w new width of window
  /// \param h new height of window
  void resize(int w, int h);

  /// Setup application
  void setup();

  /// This function called upon active mouse movement. 
  /// Active mouse movement is triggered by moving the mouse while pressing a mouse button.
  /// \param x x position of mouse pointer in screen coordinates
  /// \param y y position of mouse pointer in screen coordinates
  void motion(int x, int y);

  /// This function is called on mouse button state changes. See GLUT docs for more info.
  /// \param button button identifier (left, right, middle)
  /// \param state state identifier (up/down)
  /// \param x x position of mouse pointer in screen coordinates
  /// \param y y position of mouse pointer in screen coordinates
  void mouse(int button, int state, int x, int y);
  
  RealCamera* realCamera(){return &m_realcamera;}
protected:
  /// draws teapot
  void draw_teapot();
  /// draws a box
  void draw_box();
  /// draws a table with four legs
  void draw_table();

  /// Setup OpenGL stuff
  void setup_gl();

  /// Actual rendering of OpenGL scene
  void draw_scene();

  Camera m_camera;
  RealCamera m_realcamera;

  // mouse navigation stuff:
  int m_button_state;
  glh::vec2i m_old_pos;
  glh::vec3f m_camera_trans;
  glh::vec3f m_camera_rot;
  glh::vec3f m_camera_trans_lag;
  glh::vec3f m_camera_rot_lag;
  float inertia;
  float rot_inertia;

  unsigned int  m_model;
  GLenum        m_draw_mode;
  unsigned int  m_separation_mode;

  float m_last_time;
  float m_next_camera_time;
  float m_next_framecheck_time;
  float m_rotation;
  int m_curFps;
  bool m_animation;
  bool m_lighting;
  
  GLuint m_camTexture;
  bool m_showCamInScene, m_showCamAsPanel, m_user_centered_projection;
  RealCamera::ImageId m_cameraFilter;
  float m_trackedCyanX, m_trackedCyanY, m_trackedCyanW, m_trackedCyanH,
        m_trackedRedX, m_trackedRedY, m_trackedRedW, m_trackedRedH,
        m_trackedX, m_trackedY, m_trackedW, m_trackedH;
  float m_ZeroTrackedCyanX, m_ZeroTrackedCyanY, m_ZeroTrackedCyanW, m_ZeroTrackedCyanH,
        m_ZeroTrackedRedX, m_ZeroTrackedRedY, m_ZeroTrackedRedW, m_ZeroTrackedRedH,
        m_ZeroTrackedX, m_ZeroTrackedY, m_ZeroTrackedW, m_ZeroTrackedH;
};

// MAIN_APP_H
#endif

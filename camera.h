#ifndef CAMERA_H
#define CAMERA_H

// for glh::vec3f vector class
#include "glh_linear.h"

/// Basic Camera class
class Camera
{
public:
  Camera();
  virtual ~Camera();

  /// Call this function when the screen size changes
  /// \param width new screen width
  /// \param height new screen height
  void resize_screen(const int width, const int height);
  
  /// Updates viewing parameteres from modelview matrix. Call this function once per frame.
  /// \param modelview_matrix modelview matrix (column-major = OpenGL layout)
  void update(float modelview_matrix[16]);

  /// Returns view position
  const glh::vec3f view_position() const;
  /// Return view direction (normalized)
  const glh::vec3f view_direction() const;
  /// Returns focal length of camera
  const float focal_length() const;
  /// Returns eye separation distance. 
  const float eye_separation() const;
  /// Returns position of left eye
  const glh::vec3f left_eye() const;
  /// Returns position of right eye
  const glh::vec3f right_eye() const;
  /// Returns focal point
  const glh::vec3f focal_point() const;
  /// Returns up vector
  const glh::vec3f view_up() const;
  /// Returns camera opening angle
  const float aperture() const;

  const int screen_width() const {return m_screen_width;}
  const int screen_height() const {return m_screen_height;}
  
  void calc_centered_projection_matrix(const float xoff, const float yoff, float skew_matrix[16]);
  /// returns right vector
  const glh::vec3f right_vector() const;
protected:
  
  glh::vec3f m_view_position;
  glh::vec3f m_view_direction;
  glh::vec3f m_view_up;
  glh::vec3f m_view_right;
  float      m_focal_length;
  float      m_aperture;
  float      m_eye_separation;
  int        m_screen_width;
  int        m_screen_height;
};

// CAMERA_H
#endif

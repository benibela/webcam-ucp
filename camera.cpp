#include "camera.h"

// so we can type vec3f instead of glh::vec3f
using glh::vec3f;

Camera::Camera():
  m_view_position(0.0f,0.0f,0.0f)
  ,m_view_direction(0.0f,0.0f,1.0f)
  ,m_focal_length(10.0f)
  ,m_aperture(45.0f)
  ,m_eye_separation(0.33)
  ,m_screen_width(1024)
  ,m_screen_height(576)
{
}

Camera::~Camera()
{}
  
const float Camera::aperture() const
{
  return m_aperture;
}

const glh::vec3f Camera::view_position() const
{ 
  return m_view_position;
}

const glh::vec3f Camera::view_direction() const
{
  return m_view_direction;

}

const float Camera::focal_length() const
{
  return m_focal_length;
}

const float Camera::eye_separation() const
{
  return m_eye_separation;
}

const vec3f Camera::focal_point() const
{
  /** 
   * BEGIN CODE SNIPPET
   * 1) determine the focal point based on view position, direction and focal length of camera
   * */  
  const vec3f focus = m_view_position+m_view_direction*m_focal_length;
  return focus;

  /**
   * END CODE SNIPPET
   * */
}

const vec3f Camera::left_eye() const
{
  /** 
   * BEGIN CODE SNIPPET
   * 1) determine the position of the left eye based on view position and eye separation.
   * */ 
  const vec3f left_eye_position=m_view_position-m_view_right*(m_eye_separation*0.5f);
  return left_eye_position;
  /**
   * END CODE SNIPPET
   * */
}

const vec3f Camera::right_eye() const
{
  /** 
   * BEGIN CODE SNIPPET
   * 1) determine the position of the right eye based on view position and eye separation.
   * */
  const vec3f right_eye_position=m_view_position+m_view_right*(m_eye_separation*0.5f);;
  return right_eye_position;
  /**
   * END CODE SNIPPET
   * */
}

  
void Camera::update(float mv[])
{
  // retrieve view direction, up, right vector and view position from modelview matrix
  // those vectors form the columns of the modelview matrix
  // note: OpenGL matrices are stored column-major!
  m_view_right = vec3f(mv[0],mv[1],mv[2]);
  m_view_up = vec3f(mv[4],mv[5],mv[6]);
  m_view_direction = vec3f(mv[8],mv[9],mv[10]);
  
  m_view_direction.normalize();
  m_view_right.normalize();
  m_view_up = m_view_direction.cross(m_view_right);
  m_view_right = m_view_direction.cross(m_view_up);
  
  m_view_position = vec3f(mv[12],mv[13],mv[14]);
}
  
const glh::vec3f Camera::view_up() const
{
  return m_view_up;
}

const vec3f Camera::right_vector() const
{
  return  m_view_right;
}

void Camera::resize_screen(const int width, const int height)
{
  m_screen_width = width;
  m_screen_height = height;
}

void Camera::calc_centered_projection_matrix(const float xoff, const float yoff, float skew_matrix[16]){
	skew_matrix[0]=skew_matrix[5]=skew_matrix[10]=skew_matrix[15]=1;
	skew_matrix[1]=skew_matrix[2]=skew_matrix[3]=
	skew_matrix[4]=   skew_matrix[6]=skew_matrix[7]=
	skew_matrix[8]=skew_matrix[9]=  skew_matrix[11]=
	skew_matrix[12]=skew_matrix[13]=skew_matrix[14]=0;
	
	skew_matrix[2]=-2*xoff;
	skew_matrix[6]=2*yoff;
	std::cout << xoff <<":"<<yoff<<"\n";
}


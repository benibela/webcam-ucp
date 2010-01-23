
#include "cv.h"
#include "highgui.h"

class RealCamera{
public:
  enum ImageId {BASE_IMAGE, TRACKED_CYAN, TRACKED_RED};
  RealCamera();
  void init(int id, unsigned long int handle);
  void nextFrame();
  void trackSomething(IplImage* image,float &x, float &y, float &w, float &h, int constraintMinX);
  void trackGlasses(float &x, float &y, float &w, float &h,
                    float &cyanX, float &cyanY, float &cyanW, float &cyanH,
                    float &redX, float &redY, float &redW, float &redH);
  void killBackground();
  IplImage* image(ImageId id);
  ~RealCamera();
protected:
  CvCapture* m_capture;
  IplImage *m_image, *m_imageHLS, *m_background, *m_backgroundHLS, *m_imagePreMask, *m_imageMask;
  
  IplImage *m_trackRed, *m_trackCyan, *m_trackIntegral;
};

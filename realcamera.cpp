#include "realcamera.h"
#include <iostream>
RealCamera::RealCamera(){
    m_capture = 0;
    m_image = m_background = m_imagePreMask = m_imageMask = 0;
}
void RealCamera::init(int id, unsigned long int handle){
  m_capture = cvCaptureFromCAM( id );
  if( !m_capture )
  {
    fprintf(stderr,"Could not initialize capturing...\n");
    return;
  }
  cvSetCaptureProperty( m_capture, CV_CAP_PROP_FRAME_WIDTH, 320 );
  cvSetCaptureProperty( m_capture, CV_CAP_PROP_FRAME_HEIGHT, 240 );
}

void RealCamera::nextFrame(){
  if (!m_capture) {
    fprintf(stderr, "no capture in nextframe");
    return;
  }
  IplImage* frame = cvQueryFrame( m_capture );
  assert(frame->origin==0);
  if( !frame ){
    fprintf(stderr, "no frame available in nextframe");
    return;
  }

  if( !m_image )
  {
    /* allocate all the buffers */
    CvSize frameSize=cvGetSize(frame);
    m_image = cvCreateImage( frameSize, 8, 3 );
    m_imageHLS = cvCreateImage( frameSize, 8, 3 );
    m_trackRed = cvCreateImage( frameSize, 8, 1 );
    m_trackCyan = cvCreateImage( frameSize, 8, 1 );
    m_trackIntegral = cvCreateImage( cvSize(frame->width+1,frame->height+1), IPL_DEPTH_32S, 1 );
   // std::cout << frame->origin << "\n";
/*
            HLS = cvCreateImage( cvGetSize(frame), 8, 3 );
            hue = cvCreateImage( cvGetSize(frame), 8, 1 );
            mask = cvCreateImage( cvGetSize(frame), 8, 1 );
            backproject = cvCreateImage( cvGetSize(frame), 8, 1 );
            hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
            histimg = cvCreateImage( cvSize(320,200), 8, 3 );
            cvZero( histimg );*/
  }

//std::cout << m_image << "next frame\n";
  cvCopy( frame, m_image, 0 );
  cvCvtColor( m_image, m_imageHLS, CV_BGR2HLS );
    //cvAbsDiff(m_imageHLS, m_backgroundHLS, m_imagePreMask);
    //cvCmpS(m_image, m_background, m_imageMask);
    int stepPreMask = m_imagePreMask->widthStep/sizeof(uchar);
    int stepMask = m_imageMask->widthStep/sizeof(uchar);
    int stepImage = m_image->widthStep/sizeof(uchar);
    int stepImageHLS = m_imageHLS->widthStep/sizeof(uchar);
    int stepTrack = m_trackCyan->widthStep/sizeof(uchar);
    assert(stepTrack==m_trackRed->widthStep/sizeof(uchar));
    uchar* dataPreMask = (uchar *)m_imagePreMask->imageData;
    uchar* dataMask = (uchar *)m_imageMask->imageData;
    uchar* dataImage = (uchar *)m_image->imageData;
    uchar* dataImageHLS = (uchar *)m_imageHLS->imageData;
    uchar* dataTrackCyan = (uchar *)m_trackCyan->imageData;
    uchar* dataTrackRed = (uchar *)m_trackRed->imageData;
//    uchar* dataImage = (uchar *)m_image->imageData;
    
  if (m_background){
    int stepBackgroundHLS = m_backgroundHLS->widthStep/sizeof(uchar);
    uchar* dataBackgroundHLS = (uchar *)m_backgroundHLS->imageData;
    for (int i=0; i<m_imagePreMask->height;i++)
      for (int j=0; j<m_imagePreMask->width;j++){
				uchar* backgroundPixelHLS = &dataBackgroundHLS[i*stepBackgroundHLS+3*j];
				uchar* imagePixelHLS = &dataImageHLS[i*stepImageHLS+3*j];
				char diffH = abs(imagePixelHLS[0]-backgroundPixelHLS[0]);
				char diffL = abs(imagePixelHLS[1]-backgroundPixelHLS[1]);
				char diffS = abs(imagePixelHLS[2]-backgroundPixelHLS[2]);
				bool isBackground = false ;
				if (backgroundPixelHLS[2] <= 20) {
					//no reliable color (light/dark gray);
					isBackground = imagePixelHLS[2] <= 40+backgroundPixelHLS[2];
				} else if (backgroundPixelHLS[1] <= 20){
					isBackground = imagePixelHLS[1]<=50+backgroundPixelHLS[1];
				} else if (backgroundPixelHLS[1] >= 230){
					isBackground = imagePixelHLS[1]>=230;
				} else if (backgroundPixelHLS[1] <= 30 || backgroundPixelHLS[1] >= 220 || backgroundPixelHLS[2] <= 30) {
					//almost no reliable color (light/dark gray);
					isBackground = diffH  <50 && diffL<140 && diffS<50;
				} else {
          isBackground = diffH  <30 && diffL<170 && diffS<70;            
        }
        if (isBackground) {
          dataImage[i*stepImage+j*3] = dataImage[i*stepImage+j*3+1] = dataImage[i*stepImage+j*3+2] = 0;
          dataImageHLS[i*stepImageHLS+j*3] = dataImageHLS[i*stepImageHLS+j*3+1] = dataImageHLS[i*stepImageHLS+j*3+2] = 0;
        }
			}
  }
}

#define CVPIXEL(data, step, x, y) data[(y)*(step)+(x)]
#define CVREGIONSUM(data, step, x, y, w, h) (CVPIXEL(data, step, x+w, y+h) + CVPIXEL(data, step, x,y) - CVPIXEL(data, step, x+w,y) - CVPIXEL(data, step, x,y+h))

void RealCamera::trackSomething(IplImage* image, float &x, float &y, float &w, float &h, int constraintMinX=0){  
	cvIntegral(image, m_trackIntegral);
	
  int stepTrackCyanIntegral = m_trackIntegral->widthStep/sizeof(int32_t);
  int32_t* dataTrackCyanIntegral = (int32_t *)m_trackIntegral->imageData;
  int maxAvg=0; int bestX, bestY, bestSize;
  //global, low resolution scan
	for (int size=30;size<=image->width/3; size+=10){
		int inc=size/3;
	  for (int y=size; y<image->height;y+=inc)
  		for (int x=constraintMinX+size; x<image->width;x+=inc){
		    int avg=CVREGIONSUM(dataTrackCyanIntegral, stepTrackCyanIntegral, x-size,y-size,size,size)*1000/size;
		    if (avg>maxAvg) {
					maxAvg=avg;
					bestX=x-size;
					bestY=y-size;
					bestSize=size;
				}
			}
	}
	if (maxAvg==0) return; 
	//	std::cout << "ma:"<<maxAvg <<"->\t"<<bestX <<":"<<bestY<<"\n";
  int oldMaxAvg=maxAvg, oldBestX=bestX, oldBestY=bestY, oldBestSize=bestSize;
  //local, high resolution scan
  int minX = MAX(constraintMinX,oldBestX - oldBestSize/2);
  int maxX = MIN(m_trackCyan->height-oldBestSize-6, oldBestX + oldBestSize*3/2);
  int minY = MAX(0,oldBestY - oldBestSize/2);
  int maxY = MIN(m_trackCyan->height-oldBestSize-6, oldBestY + oldBestSize*3/2);
	for (int size=oldBestSize-6;size<=oldBestSize+6; size++){
	  for (int y=minY; y<maxY;y+=2)
  		for (int x=minX; x<maxX;x+=2){
		    int avg=CVREGIONSUM(dataTrackCyanIntegral, stepTrackCyanIntegral, x,y,size,size)*1000/size;
		    if (avg>maxAvg) {
					maxAvg=avg;
					bestX=x;
					bestY=y;
					bestSize=size;
				}
			}
	}
	x=bestX*1.0/image->width;
	y=bestY*1.0/image->height;
	w=bestSize*1.0/image->width;
	h=bestSize*1.0/image->height;
}

void RealCamera::trackGlasses(float &x, float &y, float &w, float &h,
                    float &cyanX, float &cyanY, float &cyanW, float &cyanH,
                    float &redX, float &redY, float &redW, float &redH){  
	assert(m_trackCyan!=0);
    int stepPreMask = m_imagePreMask->widthStep/sizeof(uchar);
    int stepMask = m_imageMask->widthStep/sizeof(uchar);
    int stepImage = m_image->widthStep/sizeof(uchar);
    int stepImageHLS = m_imageHLS->widthStep/sizeof(uchar);
    int stepTrack = m_trackCyan->widthStep/sizeof(uchar);
    assert(stepTrack==m_trackRed->widthStep/sizeof(uchar));
    uchar* dataPreMask = (uchar *)m_imagePreMask->imageData;
    uchar* dataMask = (uchar *)m_imageMask->imageData;
    uchar* dataImage = (uchar *)m_image->imageData;
    uchar* dataImageHLS = (uchar *)m_imageHLS->imageData;
    uchar* dataTrackCyan = (uchar *)m_trackCyan->imageData;
    uchar* dataTrackRed = (uchar *)m_trackRed->imageData;
    for (int i=0; i<m_image->height;i++)
      for (int j=0; j<m_image->width;j++){
				uchar* imagePixelHLS = &dataImageHLS[i*stepImageHLS+3*j];
			
			  bool isCyan=false;
				if (imagePixelHLS[1] >= 15 && imagePixelHLS[1] <= 230  && imagePixelHLS[2] >= 50) {
					isCyan = imagePixelHLS[0]>=75 && imagePixelHLS[0]<=115;
				}
				if (!isCyan) dataTrackCyan[i*stepTrack+j] = 0;
				else {
					int quality = 255-4*abs(imagePixelHLS[0]-90)-(255-imagePixelHLS[2])/2;
					if (imagePixelHLS[1]<=50) quality-=(50-imagePixelHLS[1]);
					else if (imagePixelHLS[1]>=205) quality-=(imagePixelHLS[1]-205);
					dataTrackCyan[i*stepTrack+j]=quality;
			  }

				bool isRed=false;
				if (imagePixelHLS[1] >= 15 && imagePixelHLS[1] <= 200 && imagePixelHLS[2] >= 70) {
					isRed = imagePixelHLS[0]<=10 || imagePixelHLS[0]>=170;
				}
				if (!isRed) dataTrackRed[i*stepTrack+j] = 0;
				else {
				  uchar quality=255-(255-imagePixelHLS[2]);
					if (imagePixelHLS[0]<=10) quality-=imagePixelHLS[0]*6;
					else quality-=(180-imagePixelHLS[0])*6;
					if (imagePixelHLS[1]<=70) quality-=70-imagePixelHLS[1];
					else if (imagePixelHLS[1]>=140) quality-=imagePixelHLS[1]-140;
					if (quality<imagePixelHLS[2]) dataTrackRed[i*stepTrack+j]=quality;
					else dataTrackRed[i*stepTrack+j]=0; //underflow
			  }
			}
	
	//cvSmooth(m_trackCyan, m_trackCyan);
	
  trackSomething(m_trackCyan, cyanX, cyanY, cyanW, cyanH);
  trackSomething(m_trackRed, redX, redY, redW, redH);
	w=redX+redW-cyanX;
	h=redY+redH-cyanY;
	x=(cyanX+cyanW+redX)/2-w/2;
	y=(cyanY+cyanH+redY)/2-h/2;
	
	
  //cvShowImage( "CamShiftDemo", m_image );
  //CvSize imageRect = cvGetSize(m_image);
//  CvConnectedComp comp;
//  CvBox2D box;
//  cvCamShift(m_trackCyan, cvRect(0,0,imageRect.width,imageRect.height), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 15, 1), &comp, &box);
//  std::cout << box.center.x<<":"<<box.center.y<<"\t" << box.size.width<<":"<<box.size.height<<"\t"
  //          <<comp.rect.x << ":" <<comp.rect.y<<"\t" << comp.rect.width <<":"<<comp.rect.height<<  "\n";
  /*x=box.center.x *1.0f/ imageRect.width;
  y=box.center.y *1.0f/ imageRect.height;
  w=box.size.width *1.0f/ imageRect.width;
  h=box.size.height *1.0f/ imageRect.height;*/
  //x=(bestX) *1.0f/ imageRect.width;
  //y=1-(bestY) *1.0f/ imageRect.height;
  //w=bestSize *1.0f/ imageRect.width;
  //h=-bestSize *1.0f/ imageRect.height;
 }

void RealCamera::killBackground(){
	if (!m_image) return;
	std::cout << "kill background\n";
	if (!m_background) {
		std::cout << "background created\n";
    m_background = cvCreateImage( cvGetSize(m_image), 8, 3 );
    m_background->origin = m_image->origin;
    m_backgroundHLS = cvCreateImage( cvGetSize(m_image), 8, 3 );
    m_backgroundHLS->origin = m_image->origin;
    m_imageMask = cvCreateImage( cvGetSize(m_image), 8, 1);
    m_imageMask->origin = m_image->origin;
    m_imagePreMask = cvCreateImage( cvGetSize(m_image), 8, 3 );
    m_imagePreMask->origin = m_image->origin;
	
	  cvCopy(m_image, m_background, 0);
    cvCvtColor( m_background, m_backgroundHLS, CV_BGR2HLS );
  } else {
		cvReleaseImage(&m_background);
		cvReleaseImage(&m_backgroundHLS);
		cvReleaseImage(&m_imageMask);
		cvReleaseImage(&m_imagePreMask);
		m_background=m_backgroundHLS=m_imageMask=m_imagePreMask=0;
  }
}

IplImage* RealCamera::image(ImageId id){
	switch (id){
		case BASE_IMAGE: return m_image;
		case TRACKED_CYAN: return m_trackCyan;
		case TRACKED_RED: return m_trackRed;
		default: std::cout << "rc:image: ups\n";
	}
}

RealCamera::~RealCamera(){
//	cvcamStop( );
//	cvcamExit( );
}
/*
#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>
#endif

IplImage *image = 0, *HLS = 0, *hue = 0, *mask = 0, *backproject = 0, *histimg = 0;
CvHistogram *hist = 0;

int backproject_mode = 0;
int select_object = 0;
int track_object = 0;
int show_hist = 1;
CvPoint origin;
CvRect selection;
CvRect track_window;
CvBox2D track_box;
CvConnectedComp track_comp;
int hdims = 16;
float hranges_arr[] = {0,180};
float* hranges = hranges_arr;
int vmin = 10, vmax = 256, smin = 30;

void on_mouse( int event, int x, int y, int flags, void* param )
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( select_object )
    {
        selection.x = MIN(x,origin.x);
        selection.y = MIN(y,origin.y);
        selection.width = selection.x + CV_IABS(x - origin.x);
        selection.height = selection.y + CV_IABS(y - origin.y);
        
        selection.x = MAX( selection.x, 0 );
        selection.y = MAX( selection.y, 0 );
        selection.width = MIN( selection.width, image->width );
        selection.height = MIN( selection.height, image->height );
        selection.width -= selection.x;
        selection.height -= selection.y;
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = cvPoint(x,y);
        selection = cvRect(x,y,0,0);
        select_object = 1;
        break;
    case CV_EVENT_LBUTTONUP:
        select_object = 0;
        if( selection.width > 0 && selection.height > 0 )
            track_object = -1;
        break;
    }
}


CvScalar HLS2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

int main( int argc, char** argv )
{
    CvCapture* capture = 0;
    
    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        capture = cvCaptureFromAVI( argv[1] ); 

    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }

    printf( "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tc - stop the tracking\n"
        "\tb - switch to/from backprojection view\n"
        "\th - show/hide object histogram\n"
        "To initialize tracking, select the object with mouse\n" );

    cvNamedWindow( "Histogram", 1 );
    cvNamedWindow( "CamShiftDemo", 1 );
    cvSetMouseCallback( "CamShiftDemo", on_mouse, 0 );
    cvCreateTrackbar( "Vmin", "CamShiftDemo", &vmin, 256, 0 );
    cvCreateTrackbar( "Vmax", "CamShiftDemo", &vmax, 256, 0 );
    cvCreateTrackbar( "Smin", "CamShiftDemo", &smin, 256, 0 );

    for(;;)
    {
        IplImage* frame = 0;
        int i, bin_w, c;

        frame = cvQueryFrame( capture );
        if( !frame )
            break;

        if( !image )
        {
            /* allocate all the buffers */
/*
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
            image->origin = frame->origin;
            hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
            hue = cvCreateImage( cvGetSize(frame), 8, 1 );
            mask = cvCreateImage( cvGetSize(frame), 8, 1 );
            backproject = cvCreateImage( cvGetSize(frame), 8, 1 );
            hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
            histimg = cvCreateImage( cvSize(320,200), 8, 3 );
            cvZero( histimg );
        }

        cvCopy( frame, image, 0 );
        cvCvtColor( image, hsv, CV_BGR2HSV );

        if( track_object )
        {
            int _vmin = vmin, _vmax = vmax;

            cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),
                        cvScalar(180,256,MAX(_vmin,_vmax),0), mask );
            cvSplit( hsv, hue, 0, 0, 0 );

            if( track_object < 0 )
            {
                float max_val = 0.f;
                cvSetImageROI( hue, selection );
                cvSetImageROI( mask, selection );
                cvCalcHist( &hue, hist, 0, mask );
                cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
                cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
                cvResetImageROI( hue );
                cvResetImageROI( mask );
                track_window = selection;
                track_object = 1;

                cvZero( histimg );
                bin_w = histimg->width / hdims;
                for( i = 0; i < hdims; i++ )
                {
                    int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
                    CvScalar color = hsv2rgb(i*180.f/hdims);
                    cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
                                 cvPoint((i+1)*bin_w,histimg->height - val),
                                 color, -1, 8, 0 );
                }
            }

            cvCalcBackProject( &hue, backproject, hist );
            cvAnd( backproject, mask, backproject, 0 );
            cvCamShift( backproject, track_window,
                        cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                        &track_comp, &track_box );
            track_window = track_comp.rect;
            
            if( backproject_mode )
                cvCvtColor( backproject, image, CV_GRAY2BGR );
            if( !image->origin )
                track_box.angle = -track_box.angle;
            cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );
        }
        
        if( select_object && selection.width > 0 && selection.height > 0 )
        {
            cvSetImageROI( image, selection );
            cvXorS( image, cvScalarAll(255), image, 0 );
            cvResetImageROI( image );
        }

        cvShowImage( "CamShiftDemo", image );
        cvShowImage( "Histogram", histimg );

        c = cvWaitKey(10);
        if( (char) c == 27 )
            break;
        switch( (char) c )
        {
        case 'b':
            backproject_mode ^= 1;
            break;
        case 'c':
            track_object = 0;
            cvZero( histimg );
            break;
        case 'h':
            show_hist ^= 1;
            if( !show_hist )
                cvDestroyWindow( "Histogram" );
            else
                cvNamedWindow( "Histogram", 1 );
            break;
        default:
            ;
        }
    }

    cvReleaseCapture( &capture );
    cvDestroyWindow("CamShiftDemo");

    return 0;
}

#ifdef _EiC
main(1,"camshiftdemo.c");
#endif*/

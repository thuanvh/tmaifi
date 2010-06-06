#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include "segmentAlgo.h"
#include "imageSegment.h"

string cvScalar2String(CvScalar value) {
  string a;
  a += (int) value.val[0];
  a += "-";
  a += (int) value.val[1];
  a += "-";
  a += (int) value.val[2];
  return a;
}

void meanshift(IplImage* image) {
  //  CvHistogram* a;
  //  int hist_size[] = {20, 20, 20};
  //  float r_ranges[] = {0, 255}; /* hue varies from 0 (~0°red) to 180 (~360°red again) */
  //  float g_ranges[] = {0, 255}; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
  //  float b_ranges[] = {0, 255}; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
  //  float* ranges[] = {r_ranges, g_ranges, b_ranges};
  //
  //  a = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);

  //IplImage *image = cvLoadImage("winter.jpg", 1);
  IplImage *image_hsv = cvCreateImage(cvGetSize(image), image->depth, 3);
  //Replacing the origin if necessary
  int flip = 0;
  if (image->origin != IPL_ORIGIN_TL) {
    flip = CV_CVTIMG_FLIP;
  }
  //creating the image in levels of gray
  cvConvertImage(image, image_hsv, CV_BGR2HSV);

  //creating the windows in wich the images will appear
  //  cvNamedWindow("Original image", CV_WINDOW_AUTOSIZE);
  //  cvNamedWindow("HSV image", CV_WINDOW_AUTOSIZE);

  CvHistogram *hist;
  int h_bins = 30, s_bins = 32, v_bins = 30;
  int hist_size[] = {h_bins, s_bins, v_bins};
  float h_ranges[] = {0, 180};
  float s_ranges[] = {0, 255};
  float v_ranges[] = {0, 255};
  float* ranges[] = {h_ranges, s_ranges, v_ranges};
  IplImage* h_plane = cvCreateImage(cvGetSize(image), 8, 1);
  IplImage* s_plane = cvCreateImage(cvGetSize(image), 8, 1);
  IplImage* v_plane = cvCreateImage(cvGetSize(image), 8, 1);
  //IplImage * planes[] = {h_plane, s_plane, v_plane};
  IplImage * planes[] = {s_plane};
  hist = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
  cvCvtPixToPlane(image_hsv, h_plane, s_plane, v_plane, 0);
  cvCalcHist(planes, hist);

  //Show the images
  //  cvShowImage("Original image", image);
  //  cvShowImage("HSV image", image_hsv);

  //cvCalcHist(&image, hist);
  IplImage* imageprob = cvCloneImage(image);
  IplImage* project = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
  cvCalcBackProject(planes, project, hist);
  int x = 0;
  int y = 0;
  int width = image->width;
  int height = image->height;
  int windowsize = 41;
  CvConnectedComp comp;
  CvRect window = cvRect(x, y, windowsize, windowsize);
  CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1000, 0.1);

  cvMeanShift(project, window, criteria, &comp);

  SegImage segimage;
  segimage.width = image->width;
  segimage.height = image->height;
  segimage.image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
  int segcount = 0;
  CvSeq* contours = comp.contour;



  //cvDrawContours(segimage.image, contours, cvScalarAll(segcount + 1), cvScalarAll(255), -1, -1, 8, cvPoint(0, 0));
  for (; contours != 0; contours = contours->h_next, segcount++)
  {
    cout<<"contour"<<segcount<<endl;
    cvDrawContours(segimage.image, contours, cvScalarAll(segcount + 1),
            cvScalarAll(255), -1, -1, 8, cvPoint(0, 0));
  }

  segimage.segments = segcount;
  cout << "Segment:" << segcount << endl;
  IplImage* imageshow = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
  segimage.colorerSegImage(image, imageshow);
  cvShowImage("image segment", imageshow);
  cvShowImage("image source", image);
  //cvShowImage("image source",image);

  cvWaitKey(0);
  cvReleaseImage(&imageshow);
  cvReleaseImage(&imageprob);
  cvReleaseImage(&project);

}

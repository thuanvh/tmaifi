#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>

#include "imageSegment.h"


IplImage* marker_mask = 0;
IplImage* markers = 0;
IplImage* imgSource, *img = 0, *img_gray = 0, *wshed = 0;
CvPoint prev_pt = {-1, -1};

void on_mouse(int event, int x, int y, int flags, void* param) {
  if (!img)
    return;

  if (event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))
    prev_pt = cvPoint(-1, -1);
  else if (event == CV_EVENT_LBUTTONDOWN)
    prev_pt = cvPoint(x, y);
  else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)) {
    CvPoint pt = cvPoint(x, y);
    if (prev_pt.x < 0)
      prev_pt = pt;
    cvLine(marker_mask, prev_pt, pt, cvScalarAll(255), 5, 8, 0);
    cvLine(img, prev_pt, pt, cvScalarAll(255), 5, 8, 0);
    prev_pt = pt;
    cvShowImage("image", img);
  }
}

void watershedManual(IplImage* imagesource) {
  imgSource = imagesource;

  //char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
  CvRNG rng = cvRNG(-1);

  //if ((img0 = cvLoadImage(filename, 1)) == 0)
  //  return 0;

  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tr - restore the original image\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\t\t(before running it, roughly mark the areas on the image)\n"
          "\t  (before that, roughly outline several markers on the image)\n");

  cvNamedWindow("image", 1);
  cvNamedWindow("watershed transform", 1);
  cvNamedWindow("imagecontour", 1);

  img = cvCloneImage(imgSource);
  img_gray = cvCloneImage(imgSource);
  wshed = cvCloneImage(imgSource);
  marker_mask = cvCreateImage(cvGetSize(img), 8, 1);
  markers = cvCreateImage(cvGetSize(img), IPL_DEPTH_32S, 1);
  cvCvtColor(img, marker_mask, CV_BGR2GRAY);
  cvCvtColor(marker_mask, img_gray, CV_GRAY2BGR);

  cvZero(marker_mask);
  cvZero(wshed);
  cvShowImage("image", img);
  cvShowImage("watershed transform", wshed);
  cvSetMouseCallback("image", on_mouse, 0);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 'r') {
      cvZero(marker_mask);
      cvCopy(imgSource, img);
      cvShowImage("image", img);
    }

    if ((char) c == 'w' || (char) c == ' ') {
      CvMemStorage* storage = cvCreateMemStorage(0);
      CvSeq* contours = 0;
      CvMat* color_tab;
      int i, j, comp_count = 0;
      //cvSaveImage( "wshed_mask.png", marker_mask );
      //marker_mask = cvLoadImage( "wshed_mask.png", 0 );
      cvFindContours(marker_mask, storage, &contours, sizeof (CvContour),
              CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
      cvZero(markers);
      for (; contours != 0; contours = contours->h_next, comp_count++) {
        cvDrawContours(markers, contours, cvScalarAll(comp_count + 1),
                cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
      }

      color_tab = cvCreateMat(1, comp_count, CV_8UC3);
      for (i = 0; i < comp_count; i++) {
        uchar* ptr = color_tab->data.ptr + i * 3;
        ptr[0] = (uchar) (cvRandInt(&rng) % 180 + 50);
        ptr[1] = (uchar) (cvRandInt(&rng) % 180 + 50);
        ptr[2] = (uchar) (cvRandInt(&rng) % 180 + 50);
      }

      {
        IplImage* markersShow = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
        cvScale(markers, markersShow);
        cvShowImage("imagecontour", markersShow);
        cvReleaseImage(&markersShow);

        double t = (double) cvGetTickCount();
        cvWatershed(imgSource, markers);
        t = (double) cvGetTickCount() - t;
        printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));
      }

      // paint the watershed image
      for (i = 0; i < markers->height; i++)
        for (j = 0; j < markers->width; j++) {
          int idx = CV_IMAGE_ELEM(markers, int, i, j);
          uchar* dst = &CV_IMAGE_ELEM(wshed, uchar, i, j * 3);
          if (idx == -1)
            dst[0] = dst[1] = dst[2] = (uchar) 255;
          else if (idx <= 0 || idx > comp_count)
            dst[0] = dst[1] = dst[2] = (uchar) 0; // should not get here
          else {
            uchar* ptr = color_tab->data.ptr + (idx - 1)*3;
            dst[0] = ptr[0];
            dst[1] = ptr[1];
            dst[2] = ptr[2];
          }
        }

      cvAddWeighted(wshed, 0.5, img_gray, 0.5, 0, wshed);
      cvShowImage("watershed transform", wshed);
      cvReleaseMemStorage(&storage);
      cvReleaseMat(&color_tab);
    }
  }
}

void watershedContour(IplImage* imagesource) {
  imgSource = imagesource;
  CvRNG rng = cvRNG(-1);
  int aperture_size = 3;
  cvNamedWindow("image", 1);
  cvNamedWindow("watershed transform", 1);
  cvNamedWindow("imagecontour", 1);

  img = cvCloneImage(imgSource);
  IplImage* img_gray2 = cvCloneImage(imgSource);
  img_gray = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
  cvSmooth(img_gray, img_gray, CV_GAUSSIAN, 11, 11);

  if (imgSource->nChannels > 1)
    cvCvtColor(imgSource, img_gray, CV_RGB2GRAY); //(imgSource);
  else
    cvCopyImage(imgSource, img_gray);
  wshed = cvCloneImage(imgSource);
  marker_mask = cvCreateImage(cvGetSize(img), 8, 1);
  //markers = cvCreateImage(cvGetSize(img), IPL_DEPTH_32S, 1);
  cvCvtColor(img, marker_mask, CV_BGR2GRAY);
  cvCvtColor(marker_mask, img_gray2, CV_GRAY2BGR);

  //  cvZero(marker_mask);
  //  cvZero(wshed);
  cvShowImage("image", img);
  cvShowImage("watershed transform", wshed);
  cvSetMouseCallback("image", on_mouse, 0);


  //CvMemStorage* storage = cvCreateMemStorage(0);
  //CvSeq* contours = 0;
  CvMat* color_tab;
  int i, j, comp_count = 100;
  //cvSaveImage( "wshed_mask.png", marker_mask );
  //marker_mask = cvLoadImage( "wshed_mask.png", 0 );

  //  cvFindContours(marker_mask, storage, &contours, sizeof (CvContour),
  //          CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  //  cvZero(markers);
  //  for (; contours != 0; contours = contours->h_next, comp_count++) {
  //    cvDrawContours(markers, contours, cvScalarAll(comp_count + 1),
  //            cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
  //  }


  IplImage* imgContour = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
  IplImage* imgContourDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);

  //cvSobel(img_gray, imgSobel, 1, 1, aperture_size);
  //cvLaplace(img_gray, imgContour, aperture_size);
  IplImage* imgCanny = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
  cvCanny(img_gray, imgCanny, 20, 50);
  cvScale(imgCanny, imgContour);
  //cvSubS(imgSobel, cvScalar(255), imgSobel);
  //cvInv(imgSobel,inv);

  //cvAbs(imgSobel, imgSobel);
  cvThreshold(imgContour, imgContour, 0, 255, CV_THRESH_BINARY_INV);
  cvConvert(imgContour, imgContourDisplay);
  //cvThreshold(imgSobelDisplay, imgSobelDisplay, threshold1, 255, CV_THRESH_BINARY);
  // Ouvrir des fenetes

  cvShowImage("Output Sobel", imgContourDisplay);

  color_tab = cvCreateMat(1, comp_count, CV_8UC3);
  for (i = 0; i < comp_count; i++) {
    uchar* ptr = color_tab->data.ptr + i * 3;
    ptr[0] = (uchar) (cvRandInt(&rng) % 180 + 50);
    ptr[1] = (uchar) (cvRandInt(&rng) % 180 + 50);
    ptr[2] = (uchar) (cvRandInt(&rng) % 180 + 50);
  }


  markers = cvCreateImage(cvGetSize(img), IPL_DEPTH_32S, 1);

  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* contours = 0;

  cvFindContours(imgContourDisplay, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  cvZero(markers);
  for (; contours != 0; contours = contours->h_next, comp_count++) {
    cvDrawContours(markers, contours, cvScalarAll(comp_count + 1), cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
  }

  cvReleaseImage(&imgContour);
  double t = (double) cvGetTickCount();
  cvWatershed(imgSource, markers);
  t = (double) cvGetTickCount() - t;
  printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));


  // paint the watershed image
  for (i = 0; i < markers->height; i++)
    for (j = 0; j < markers->width; j++) {
      int idx = CV_IMAGE_ELEM(markers, int, i, j);
      uchar* dst = &CV_IMAGE_ELEM(wshed, uchar, i, j * 3);
      if (idx == -1)
        dst[0] = dst[1] = dst[2] = (uchar) 255;
      else if (idx <= 0 || idx > comp_count)
        dst[0] = dst[1] = dst[2] = (uchar) 0; // should not get here
      else {
        uchar* ptr = color_tab->data.ptr + (idx - 1)*3;
        dst[0] = ptr[0];
        dst[1] = ptr[1];
        dst[2] = ptr[2];
      }
    }

  cvAddWeighted(wshed, 0.5, img_gray2, 0.5, 0, wshed);
  cvShowImage("watershed transform", wshed);
  cvWaitKey(0);
  cvReleaseMemStorage(&storage);
  cvReleaseMat(&color_tab);
}

void watershed(IplImage* imagesource) {
  IplImage* imageGaussian = cvCloneImage(imagesource);
  cvSmooth(imageGaussian, imageGaussian, CV_GAUSSIAN, 11, 11);
  IplImage* imgGray = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 1);
  if (imageGaussian->nChannels > 1)
    cvCvtColor(imageGaussian, imgGray, CV_RGB2GRAY); //(imgSource);
  else
    cvCopyImage(imageGaussian, imgGray);
  cvShowImage("smooth image", imageGaussian);
  IplImage* markers = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_32S, 1);

  int localsize = 100;
  //CvMat mask=cvCreateMat(localsize,localsize, IPL_DEPTH_8U);
  double min = 0;
  double max = 0;
  CvPoint minLoc, maxLoc;
  int width = imagesource->width;
  int height = imagesource->height;
  int segid = 1;
  int i = 0;

  cvZero(markers);
  for (int i = 0; i < height - localsize; i += localsize) {
    for (int j = 0; j < width - localsize; j += localsize) {
      cvSetImageROI(imgGray, cvRect(j, i, localsize, localsize));
      cvMinMaxLoc(imgGray, &min, &max, &minLoc, &maxLoc);
      cvSetAt(markers, cvScalar(segid++), i + minLoc.y, j + minLoc.x);
      cvResetImageROI(imgGray);
    }
  }

  IplImage* imgm = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 1);
  cvScale(markers, imgm);
  cout << "markers" << endl;
  uchar* p2 = (uchar*) imgm->imageData;
  i = 0;
  while (i++<imgm->imageSize) {
    if (*p2 > segid)
      cout << (int) * p2 << " ";
    p2++;
  }

  IplImage* markersShow = cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1);
  cvScale(markers, markersShow);
  cvShowImage("imagecontour", markersShow);
  cvReleaseImage(&markersShow);

  double t = (double) cvGetTickCount();
  cvWatershed(imageGaussian, markers);
  t = (double) cvGetTickCount() - t;
  printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));

  markersShow = cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1);
  cvScale(markers, markersShow, 255 / segid);
  cvShowImage("imagecontour2", markersShow);


  SegImage segmentImage;
  cvZero(markersShow);
  cvScale(markers, markersShow);
  segmentImage.image = markersShow;
  segmentImage.height = height;
  segmentImage.width = width;
  segmentImage.segments = segid;

  cout << "markers after" << endl;
  uchar* p = (uchar*) markersShow->imageData;
  i = 0;
  //  while(i++<markersShow->imageSize){
  //    if(*p>segid)
  //      cout<<(int)*p<<" ";
  //    p++;
  //  }
  cvShowImage("imagecontour3", markersShow);
  IplImage* segmentShow = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 3);
  segmentImage.colorerSegImage(imagesource, segmentShow);
  cvShowImage("imagesegmentsoource", imagesource);
  cvShowImage("imagesegment2", segmentShow);

  int idensiteSeuil = 255;
  cvWaitKey(0);
  cvReleaseImage(&markersShow);
  cvReleaseImage(&imageGaussian);
  cvReleaseImage(&markers);
  cvReleaseImage(&segmentShow);
}


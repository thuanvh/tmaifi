#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>

#include "imageSegment.h"
#include "watershedDemo.h"
WatershedDemo* WatershedDemo::currentDemo;

WatershedDemo::WatershedDemo() {
  marker_mask = markers = imgSource = img = img_gray = wshed = 0;
  prev_pt = cvPoint(-1, -1);
  isManual = false;
  WatershedDemo::currentDemo = this;
  segmentImageRef = NULL;
  localsize = 3;
  threshold = 30;
}

WatershedDemo::~WatershedDemo() {

}

void WatershedDemo::on_mouseDemo(int event, int x, int y, int flags, void* param) {
  WatershedDemo::currentDemo->on_mouse(event, x, y, flags, param);
}

void WatershedDemo::on_mouse(int event, int x, int y, int flags, void* param) {
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

void WatershedDemo::printInformation() {
  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tr - restore the original image - only for manual\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\ts - save segment image, fusion matrix.\n"
          "\t\tNotice: for manual "
          "\t  (before running it, roughly mark the areas on the image)\n"
          "\t  (before that, roughly outline several markers on the image)\n");

}

void WatershedDemo::watershedManual(IplImage* imagesource) {
  imgSource = imagesource;

  //char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
  CvRNG rng = cvRNG(-1);

  //if ((img0 = cvLoadImage(filename, 1)) == 0)
  //  return 0;
  printInformation();

  cvNamedWindow("image", 0);
  cvNamedWindow("watershed transform", 0);
  if (segmentImageRef != NULL) {
    cvNamedWindow("image reference", 0);
    imgref = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 3);
    segmentImageRef->colorerSegImage(imagesource, imgref);
    cvShowImage("image reference", imgref);

  }

  img = cvCloneImage(imgSource);
  img_gray = cvCloneImage(imgSource);
  wshed = cvCloneImage(imgSource);
  marker_mask = cvCreateImage(cvGetSize(img), 8, 1);
  imgSegmentOuput = cvCreateImage(cvGetSize(img), 8, 3);
  markers = cvCreateImage(cvGetSize(img), IPL_DEPTH_32S, 1);
  cvCvtColor(img, marker_mask, CV_BGR2GRAY);
  cvCvtColor(marker_mask, img_gray, CV_GRAY2BGR);

  cvZero(marker_mask);
  cvZero(wshed);
  cvShowImage("image", img);
  cvShowImage("watershed transform", wshed);
  cvSetMouseCallback("image", WatershedDemo::on_mouseDemo, 0);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 'r') {
      cvZero(marker_mask);
      cvCopy(imgSource, img);
      cvShowImage("image", img);
    }
    if ((char) c == 's') {
      saveFile();
    }

    if ((char) c == 'w' || (char) c == ' ') {
      CvMemStorage* storage = cvCreateMemStorage(0);
      CvSeq* contours = 0;
      CvMat* color_tab;
      int i, j, comp_count = 0;
      //cvSaveImage( "wshed_mask.png", marker_mask );
      //marker_mask = cvLoadImage( "wshed_mask.png", 0 );
      cvFindContours(marker_mask, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
      cvZero(markers);
      for (; contours != 0; contours = contours->h_next, comp_count++) {
        cvDrawContours(markers, contours, cvScalarAll(comp_count + 1), cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
      }

      double t = (double) cvGetTickCount();
      cvWatershed(imgSource, markers);
      t = (double) cvGetTickCount() - t;
      printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));

      IplImage* markersShow = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 1);

      cvZero(markersShow);
      cvScale(markers, markersShow);
      segmentImage.image = markersShow;
      segmentImage.height = imagesource->height;
      segmentImage.width = imagesource->width;
      segmentImage.segments = comp_count;
      segmentImage.filename = filepath;

      segmentImage.colorerSegImage(imagesource, imgSegmentOuput);

      cvAddWeighted(imgSegmentOuput, 0.9, img_gray, 0.1, 0, wshed);
      cvShowImage("watershed transform", wshed);
      cvReleaseMemStorage(&storage);

    }
  }
}

void WatershedDemo::watershed(IplImage* imagesource) {
  cvNamedWindow("image", 1);
  cvNamedWindow("watershed transform", 1);
  if (segmentImageRef != NULL) {
    cvNamedWindow("image reference", 0);
    imgref = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 3);
    segmentImageRef->colorerSegImage(imagesource, imgref);
    cvShowImage("image reference", imgref);

  }
  imgSegmentOuput = cvCreateImage(cvGetSize(imagesource), 8, 3);



  IplImage* imgGray = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 1);
  IplImage* markers = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_32S, 1);
  IplImage* markersShow = cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1);
  img_gray = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 3);
  wshed = cvCreateImage(cvGetSize(imagesource), IPL_DEPTH_8U, 3);
  cvCvtColor(imagesource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);

  IplImage* imageGaussian = cvCloneImage(imagesource);
  cvSmooth(imageGaussian, imageGaussian, CV_GAUSSIAN);

  if (imageGaussian->nChannels > 1)
    cvCvtColor(imageGaussian, imgGray, CV_RGB2GRAY); //(imgSource);
  else
    cvCopyImage(imageGaussian, imgGray);


  cvShowImage("image", imagesource);

  int width = imagesource->width;
  int height = imagesource->height;
  int segid = 1;
  int i, j = 0;

  cvCreateTrackbar("Threshold", "watershed transform", &threshold, 255, NULL);
  cvCreateTrackbar("Gaussian Size", "watershed transform", &localsize, 11, NULL);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 'r') {
      cvZero(marker_mask);
      cvCopy(imgSource, img);
      cvShowImage("image", img);
    }
    if ((char) c == 's') {
      saveFile();
    }
    if ((char) c == 'w' || (char) c == ' ') {

      if (localsize % 2 == 0)
        localsize += 1;
      cvSmooth(imagesource, imageGaussian, CV_GAUSSIAN, localsize, localsize);
      cvCvtColor(imageGaussian, imgGray, CV_RGB2GRAY);
      cvZero(markers);
      CvPoint minLoc, maxLoc;
      double min, max;

      IplImage* imgcontour = cvCloneImage(imgGray);
      //      for (i = 0; i < height - localsize; i ++) {
      //        for (j = 0; j < width - localsize; j ++) {
      //          cvSetImageROI(imgGray, cvRect(j, i, localsize, localsize));
      //          cvMinMaxLoc(imgGray, &min, &max, &minLoc, &maxLoc);
      //          cvSetAt(imgcontour, cvScalar(255), i + minLoc.y, j + minLoc.x);
      //          cvResetImageROI(imgGray);
      //        }
      //      }
      cvThreshold(imgGray, imgcontour, threshold, 255, CV_THRESH_BINARY_INV);

      CvMemStorage* storage = cvCreateMemStorage(0);
      CvSeq* contours = 0;

      cvFindContours(imgcontour, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

      for (segid = 0; contours != 0; contours = contours->h_next, segid++) {
        cvDrawContours(markers, contours, cvScalarAll(segid + 1), cvScalarAll(segid + 1), -1, -1, 8, cvPoint(0, 0));
      }

      cvReleaseImage(&imgcontour);
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


      double t = (double) cvGetTickCount();
      cvWatershed(imageGaussian, markers);
      t = (double) cvGetTickCount() - t;
      printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));


      //segmentImage;

      cvZero(markersShow);
      cvScale(markers, markersShow);
      segmentImage.image = markersShow;
      segmentImage.height = height;
      segmentImage.width = width;
      segmentImage.segments = segid;
      segmentImage.filename = filepath;
      //IplImage* a = cvCloneImage(imgGray);
      //segmentImage.graySegImage(a);
      //cvShowImage("watershed transform2", a);
      //cvReleaseImage(&a);

      cvZero(imgSegmentOuput);
      segmentImage.colorerSegImage(imageGaussian, imgSegmentOuput);
      //cvShowImage("watershed transform3", imgSegmentOuput);
      cvAddWeighted(imgSegmentOuput, 0.9, img_gray, 0.1, 0, wshed);
      cvShowImage("watershed transform", wshed);
      cvReleaseMemStorage(&storage);
    }

  }
  cvReleaseImage(&markersShow);
  cvReleaseImage(&markers);
  cvReleaseImage(&imageGaussian);
}

void WatershedDemo::saveFile() {
  char fileout [ FILENAME_MAX ];
  if (isManual) {
    sprintf(fileout, "%s.watershed.manual.marked.jpg", filepath);
    cvSaveImage(fileout, img);
    sprintf(fileout, "%s.watershed.manual.segment.jpg", filepath);
    cvSaveImage(fileout, imgSegmentOuput);
    sprintf(fileout, "%s.watershed.manual.hybrid.jpg", filepath);
    cvSaveImage(fileout, wshed);
  } else {
    sprintf(fileout, "%s.watershed.gauss.segment.%d.%d.jpg", filepath, threshold, localsize);
    cvSaveImage(fileout, imgSegmentOuput);
    sprintf(fileout, "%s.watershed.gauss.hybrid.%d.%d.jpg", filepath, threshold, localsize);
    cvSaveImage(fileout, wshed);
  }
  if (segmentImageRef != NULL) {
    sprintf(fileout, "%s.ref.jpg", filepath);
    cvSaveImage(fileout, imgref);

    sprintf(fileout, "%s.watershed.matrix.txt", filepath);
    MatrixConfusion* mx = segmentImage.compareSegImage(segmentImageRef);
    saveMatrixConfusion(fileout, mx);
    delete mx;
  }
}

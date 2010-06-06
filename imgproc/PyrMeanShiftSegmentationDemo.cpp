#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "PyrMeanShiftSegmentationDemo.h"
#include "segmentAlgo.h"
#define CV_NO_BACKWARD_COMPATIBILITY

using namespace std;


PyrMeanShiftSegmentationDemo::PyrMeanShiftSegmentationDemo() {
  imageSegmentInput = imageSegmentShow = NULL;
  level = 1;
  sp = 10;
  sr = 50;

}


void PyrMeanShiftSegmentationDemo::printInformation() {
  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\ts - save segment image, fusion matrix.\n"
          "\t\tNotice: "
          "\t  Choose parametres first, then press space or w\n"
          "\t  \n");

}

int PyrMeanShiftSegmentationDemo::pyrmeanshiftdemo(IplImage* imageSource) {
  //char* filename = argc == 2 ? argv[1] : (char*)"fruits.jpg";
  printInformation();
  //if( (image[0] = cvLoadImage( filename, 1)) == 0 )
  //    return -1;
  IplImage* imgGray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 1);
  IplImage* img_gray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  hybridImage = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  cvCvtColor(imageSource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);


  cvNamedWindow("Source", 0);
  cvShowImage("Source", imageSource);

  cvNamedWindow("Pyr Mean Shift Segmentation", 0);
  if (segmentImageRef != NULL) {
    cvNamedWindow("image reference", 0);
    imgref = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
    segmentImageRef->colorerSegImage(imageSource, imgref);
    cvShowImage("image reference", imgref);

  }
 
  IplImage* imageSrc = cvCloneImage(imageSource);
 
  imageSegmentInput = cvCloneImage(imageSrc);
  imageSegmentShow = cvCloneImage(imageSrc);
  // segmentation of the color image



  cvCreateTrackbar("Level", "Pyr Mean Shift Segmentation", &level, 10, NULL);
  cvCreateTrackbar("Spatial radius", "Pyr Mean Shift Segmentation", &sp, 255, NULL);
  cvCreateTrackbar("Color radius", "Pyr Mean Shift Segmentation", &sr, 255, NULL);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 's') {
      saveFile();
    }

    if ((char) c == 'w' || (char) c == ' ') {

      
      double t = (double) cvGetTickCount();
      cvPyrMeanShiftFiltering(imageSegmentInput, imageSegmentShow, sp, sr, level);
      t = (double) cvGetTickCount() - t;
      printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));


      getImageSegment();
      //segmentImage.colorerSegImage(imageSource, imageSegmentShow);
      //cvAddWeighted(imageSegmentShow, 0.9, img_gray, 0.1, 0, hybridImage);
      //cvShowImage("Pyr Mean Shift Segmentation", hybridImage);
      cvShowImage("Pyr Mean Shift Segmentation", imageSegmentShow);
    }
  }


  cvDestroyWindow("Pyr Mean Shift Segmentation");
  cvDestroyWindow("Source");

  //cvReleaseMemStorage(&storage);

  cvReleaseImage(&imageSource);
  cvReleaseImage(&imageSegmentInput);
  cvReleaseImage(&imageSegmentShow);

  return 0;
}

void PyrMeanShiftSegmentationDemo::getImageSegment() {
  segmentImage.filename = filepath;
  segmentImage.width = imageSegmentShow->width;
  segmentImage.height = imageSegmentShow->height;
  segmentImage.image = cvCreateImage(cvGetSize(imageSegmentShow), IPL_DEPTH_8U, 1);

  int segcount = 0;
  //min_comp.value = cvScalarAll(0);
  map<string, int> mapping;

  for (int i = 0; i < segmentImage.height; i++) {
    for (int j = 0; j < segmentImage.width; j++) {
      uchar* outptr = &CV_IMAGE_ELEM(imageSegmentShow, uchar, i, j * 3);
      CvScalar value = cvScalar(outptr[0], outptr[1], outptr[2]);
      cout << (int) outptr[0] << "-" << (int) outptr[1] << "-" << (int) outptr[2] << " ";
      int cluster_idx = -1;
      string strvalue = cvScalar2String(value);
      cluster_idx = mapping[strvalue]; //clusters->data.i[i * imageSource->width + j];
      if (cluster_idx == 0)
        mapping[strvalue] = segcount++;
      cluster_idx = segcount - 1;
      uchar* q = &CV_IMAGE_ELEM(segmentImage.image, uchar, i, j);
      *q = (uchar) cluster_idx;
      cout << (int) cluster_idx << " ";
    }
  }
  segmentImage.segments = segcount;
}

void PyrMeanShiftSegmentationDemo::saveFile() {
  char fileout [ FILENAME_MAX ];
  sprintf(fileout, "%s.pyrms.segment.%d.%d.jpg", filepath, sp, sr);
  cvSaveImage(fileout, imageSegmentShow);
  sprintf(fileout, "%s.pyrms.hybrid.%d.%d.jpg", filepath, sp, sr);
  cvSaveImage(fileout, hybridImage);

//  if (segmentImageRef != NULL) {
//    sprintf(fileout, "%s.ref.jpg", filepath);
//    cvSaveImage(fileout, imgref);
//
//    sprintf(fileout, "%s.pyrms.matrix.%d.%d.txt", filepath, sp, sr);
//    MatrixConfusion* mx = segmentImage.compareSegImage(segmentImageRef);
//    saveMatrixConfusion(fileout, mx);
//    delete mx;
//  }
}
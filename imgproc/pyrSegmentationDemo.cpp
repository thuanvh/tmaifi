#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "pyrSegmentationDemo.h"
#include "segmentAlgo.h"
using namespace std;
#define CV_NO_BACKWARD_COMPATIBILITY
//PyrSegmentationDemo* PyrSegmentationDemo::currentDemo;

PyrSegmentationDemo::PyrSegmentationDemo() {

  imageSegmentInput = imageSegmentShow = NULL;
  level = 4;
  block_size = 1000;
  filter = CV_GAUSSIAN_5x5;
  //  PyrSegmentationDemo::currentDemo = this;
}

void PyrSegmentationDemo::printInformation() {
  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\ts - save segment image, fusion matrix.\n"
          "\t\tNotice: "
          "\t  Choose parametres first, then press space or w\n"
          "\t  \n");

}

int PyrSegmentationDemo::pyrdemo(IplImage* imageSource) {
  //char* filename = argc == 2 ? argv[1] : (char*)"fruits.jpg";
  printInformation();
  //if( (image[0] = cvLoadImage( filename, 1)) == 0 )
  //    return -1;
  IplImage* imgGray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 1);
  IplImage* img_gray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  hybridImage = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  cvCvtColor(imageSource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);

  IplImage* imageSrc = cvCloneImage(imageSource);
  //image[0] = imageSource;
  cvNamedWindow("Source", 0);
  cvShowImage("Source", imageSrc);

  cvNamedWindow("Pyr Segmentation", 0);
  if (segmentImageRef != NULL) {
    cvNamedWindow("image reference", 0);
    imgref = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
    segmentImageRef->colorerSegImage(imageSource, imgref);
    cvShowImage("image reference", imgref);

  }

  storage = cvCreateMemStorage(block_size);

  imageSrc->width &= -(1 << level);
  imageSrc->height &= -(1 << level);

  imageSegmentInput = cvCloneImage(imageSrc);
  imageSegmentShow = cvCloneImage(imageSrc);
  // segmentation of the color image
  l = 1;
  threshold1 = 255;
  threshold2 = 30;

  //ON_SEGMENT(1);
  cvCreateTrackbar("Level", "Pyr Segmentation", &level, 10, NULL);
  cvCreateTrackbar("Error Link Threshold", "Pyr Segmentation", &threshold1, 255, NULL);
  cvCreateTrackbar("Error Cluster Threshold", "Pyr Segmentation", &threshold2, 255, NULL);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 's') {
      saveFile();
    }

    if ((char) c == 'w' || (char) c == ' ') {
      imageSegmentShow->width = imageSegmentInput->width = (imageSource->width & -(1 << level));
      imageSegmentShow->height = imageSegmentInput->height = (imageSource->height & -(1 << level));

      double t = (double) cvGetTickCount();
      cvPyrSegmentation(imageSegmentInput, imageSegmentShow, storage, &comp, level, threshold1 + 1, threshold2 + 1);
      t = (double) cvGetTickCount() - t;
      printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));


      imageSegmentShow->width = imageSource->width;
      imageSegmentShow->height = imageSource->height;
      getImageSegment();
      segmentImage.colorerSegImage(imageSource, imageSegmentShow);
      cvAddWeighted(imageSegmentShow, 0.9, img_gray, 0.1, 0, hybridImage);
      cvShowImage("Pyr Segmentation", hybridImage);
    }
  }
  cvShowImage("Pyr Segmentation", imageSegmentShow);
  cvWaitKey(0);

  cvDestroyWindow("Pyr Segmentation");
  cvDestroyWindow("Source");

  cvReleaseMemStorage(&storage);

  cvReleaseImage(&imageSource);
  cvReleaseImage(&imageSegmentInput);
  cvReleaseImage(&imageSegmentShow);

  return 0;
}

void PyrSegmentationDemo::getImageSegment() {
  segmentImage.filename = filepath;
  segmentImage.width = imageSegmentShow->width;
  segmentImage.height = imageSegmentShow->height;
  segmentImage.segments = l_comp = comp->total;
  segmentImage.image = cvCreateImage(cvGetSize(imageSegmentShow), IPL_DEPTH_8U, 1);


  int segid = 0;
  min_comp.value = cvScalarAll(0);
  map<string, int> mapping;
  while (segid < l_comp) {
    cur_comp = (CvConnectedComp*) cvGetSeqElem(comp, segid);


    string a = cvScalar2String(cur_comp->value);

    mapping.insert(pair<string, int>(a, segid));

    segid++;

  }
  for (int i = 0; i < segmentImage.height; i++) {
    for (int j = 0; j < segmentImage.width; j++) {
      uchar* outptr = &CV_IMAGE_ELEM(imageSegmentShow, uchar, i, j * 3);
      CvScalar value = cvScalar(outptr[0], outptr[1], outptr[2]);
      int cluster_idx = mapping[cvScalar2String(value)]; //clusters->data.i[i * imageSource->width + j];
      uchar* q = &CV_IMAGE_ELEM(segmentImage.image, uchar, i, j);
      *q = (uchar) cluster_idx;
    }
  }
  //return segmentImage;
}

void PyrSegmentationDemo::saveFile() {
  char fileout [ FILENAME_MAX ];
  sprintf(fileout, "%s.pyr.segment.%d.%d.%d.jpg", filepath, level, threshold1, threshold2);
  cvSaveImage(fileout, imageSegmentShow);
  sprintf(fileout, "%s.pyr.hybrid.%d.%d.%d.jpg", filepath, level, threshold1, threshold2);
  cvSaveImage(fileout, hybridImage);

  if (segmentImageRef != NULL) {
    sprintf(fileout, "%s.ref.jpg", filepath);
    cvSaveImage(fileout, imgref);

    sprintf(fileout, "%s.pyr.matrix.%d.%d.%d.txt", filepath, level, threshold1, threshold2);
    MatrixConfusion* mx = segmentImage.compareSegImage(segmentImageRef);
    saveMatrixConfusion(fileout, mx);
    delete mx;
  }
}

SegImage* pyrSegmentation(IplImage* imageSource, int threshold1 , int threshold2) {

  //char* filename = argc == 2 ? argv[1] : (char*)"fruits.jpg";
  //printInformation();
  //if( (image[0] = cvLoadImage( filename, 1)) == 0 )
  //    return -1;
  int level = 4;
  int block_size = 1000;
  int filter = CV_GAUSSIAN_5x5;
  CvConnectedComp *cur_comp, min_comp;
  int l_comp;
  CvSeq *comp;
  IplImage* imgGray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 1);
  IplImage* img_gray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  IplImage* hybridImage = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  cvCvtColor(imageSource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);

  IplImage* imageSrc = cvCloneImage(imageSource);
  //image[0] = imageSource;

  CvMemStorage* storage = cvCreateMemStorage(block_size);

  imageSrc->width &= -(1 << level);
  imageSrc->height &= -(1 << level);

  IplImage* imageSegmentInput = cvCloneImage(imageSrc);
  IplImage* imageSegmentShow = cvCloneImage(imageSrc);
  // segmentation of the color image



  //ON_SEGMENT(1);
  //  cvCreateTrackbar("Level", "Pyr Segmentation", &level, 10, NULL);
  //  cvCreateTrackbar("Error Link Threshold", "Pyr Segmentation", &threshold1, 255, NULL);
  //  cvCreateTrackbar("Error Cluster Threshold", "Pyr Segmentation", &threshold2, 255, NULL);


  imageSegmentShow->width = imageSegmentInput->width = (imageSource->width & -(1 << level));
  imageSegmentShow->height = imageSegmentInput->height = (imageSource->height & -(1 << level));


  cvPyrSegmentation(imageSegmentInput, imageSegmentShow, storage, &comp, level, threshold1 + 1, threshold2 + 1);



  imageSegmentShow->width = imageSource->width;
  imageSegmentShow->height = imageSource->height;
  SegImage* segmentImage = new SegImage();

  segmentImage->width = imageSegmentShow->width;
  segmentImage->height = imageSegmentShow->height;
  segmentImage->segments = l_comp = comp->total;
  segmentImage->image = cvCreateImage(cvGetSize(imageSegmentShow), IPL_DEPTH_8U, 1);


  int segid = 0;
  min_comp.value = cvScalarAll(0);
  map<string, int> mapping;
  while (segid < l_comp) {
    cur_comp = (CvConnectedComp*) cvGetSeqElem(comp, segid);


    string a = cvScalar2String(cur_comp->value);

    mapping.insert(pair<string, int>(a, segid));

    segid++;

  }
  for (int i = 0; i < segmentImage->height; i++) {
    for (int j = 0; j < segmentImage->width; j++) {
      uchar* outptr = &CV_IMAGE_ELEM(imageSegmentShow, uchar, i, j * 3);
      CvScalar value = cvScalar(outptr[0], outptr[1], outptr[2]);
      int cluster_idx = mapping[cvScalar2String(value)]; //clusters->data.i[i * imageSource->width + j];
      uchar* q = &CV_IMAGE_ELEM(segmentImage->image, uchar, i, j);
      *q = (uchar) cluster_idx;
    }
  }

  cvReleaseMemStorage(&storage);

  cvReleaseImage(&imageSegmentInput);
  cvReleaseImage(&imageSegmentShow);

  return segmentImage;
}
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
PyrMeanShiftSegmentationDemo* PyrMeanShiftSegmentationDemo::currentDemo;

PyrMeanShiftSegmentationDemo::PyrMeanShiftSegmentationDemo() {
  image[0] = image[1] = image0 = image1 = NULL;
  level = 1;
  block_size = 1000;
  filter = CV_GAUSSIAN_5x5;
  PyrMeanShiftSegmentationDemo::currentDemo = this;
}

void PyrMeanShiftSegmentationDemo::segment() {
  cout << sp << ":" << sr << endl;
  cvPyrMeanShiftFiltering(image0, image1, sp, sr, level);
  /*l_comp = comp->total;

  i = 0;
  min_comp.value = cvScalarAll(0);
  while(i<l_comp)
  {
      cur_comp = (CvConnectedComp*)cvGetSeqElem ( comp, i );
      if(fabs(255- min_comp.value.val[0])>
         fabs(255- cur_comp->value.val[0]) &&
         fabs(min_comp.value.val[1])>
         fabs(cur_comp->value.val[1]) &&
         fabs(min_comp.value.val[2])>
         fabs(cur_comp->value.val[2]) )
         min_comp = *cur_comp;
      i++;
  }*/
  cvShowImage("Segmentation", image1);
}

void PyrMeanShiftSegmentationDemo::ON_SEGMENT(int a) {
  //cvPyrSegmentation(image0, image1, storage, &comp,level, threshold1 + 1, threshold2 + 1);
  currentDemo->segment();
}

int PyrMeanShiftSegmentationDemo::pyrmeanshiftdemo(IplImage* imageSource) {
  //char* filename = argc == 2 ? argv[1] : (char*)"fruits.jpg";

  //if( (image[0] = cvLoadImage( filename, 1)) == 0 )
  //    return -1;

  image[0] = imageSource;
  cvNamedWindow("Source", 0);
  cvShowImage("Source", image[0]);

  cvNamedWindow("Segmentation", 0);

  storage = cvCreateMemStorage(block_size);

  image[0]->width &= -(1 << level);
  image[0]->height &= -(1 << level);

  image0 = cvCloneImage(image[0]);
  image1 = cvCloneImage(image[0]);
  // segmentation of the color image
  l = 1;
  sp = 25;
  sr = 10;

  ON_SEGMENT(1);

  ssp = cvCreateTrackbar("Threshold1", "Segmentation", &sp, 255, this->ON_SEGMENT);
  ssr = cvCreateTrackbar("Threshold2", "Segmentation", &sr, 255, this->ON_SEGMENT);


  cvShowImage("Segmentation", image1);
  cvWaitKey(0);

  cvDestroyWindow("Segmentation");
  cvDestroyWindow("Source");

  cvReleaseMemStorage(&storage);

  cvReleaseImage(&image[0]);
  cvReleaseImage(&image0);
  cvReleaseImage(&image1);

  return 0;
}

SegImage PyrMeanShiftSegmentationDemo::getImageSegment() {
  SegImage segimage;
  segimage.width = image1->width;
  segimage.height = image1->height;
  segimage.segments = l_comp = comp->total;
  segimage.image = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);


  int segcount = 0;
  min_comp.value = cvScalarAll(0);
  map<string, int> mapping;

  for (int i = 0; i < segimage.height; i++) {
    for (int j = 0; j < segimage.width; j++) {
      uchar* outptr = &CV_IMAGE_ELEM(image1, uchar, i, j * 3);
      CvScalar value = cvScalar(outptr[0], outptr[1], outptr[2]);
      int cluster_idx = -1;
      string strvalue = cvScalar2String(value);
      cluster_idx = mapping[strvalue]; //clusters->data.i[i * imageSource->width + j];
      if (cluster_idx == 0)
        mapping[strvalue] = segcount++;
      uchar* q = &CV_IMAGE_ELEM(segimage.image, uchar, i, j);
      *q = (uchar) cluster_idx;
    }
  }
  return segimage;
}

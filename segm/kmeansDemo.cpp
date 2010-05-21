#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include "imageSegment.h"
#include "kmeansDemo.h"

KmeansSegmentDemo::KmeansSegmentDemo() {
  criteriaType = CV_TERMCRIT_EPS + CV_TERMCRIT_ITER;
  epsilon = 1.0;
  maxIteration = 10;
  clusterCount = 15;
}

void KmeansSegmentDemo::segmentation(IplImage* imageSource) {
  //#define MAX_CLUSTERS 5
  //  static const CvScalar color_tab[MAX_CLUSTERS] = {
  //    CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(100, 100, 255),
  //    CV_RGB(255, 0, 255), CV_RGB(255, 255, 0)
  //  };
  //  IplImage* img = cvCreateImage(cvSize(500, 500), 8, 3);
  //  CvRandState rng;
  //  cvRandInit(&rng, 0, 1, -1, CV_RAND_NORMAL);

  //cvNamedWindow("clusters", 1);

  //  while (true) {
  //int k, cluster_count = cvRandNext(&rng) % MAX_CLUSTERS + 1;
  int i, sample_count = imageSource->width * imageSource->height;

  CvMat* points = cvCreateMat(sample_count, 1, CV_32FC3);
  CvMat* clusters = cvCreateMat(sample_count, 1, CV_32SC1);
  IplImage* imgpoints = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      uchar* p = &CV_IMAGE_ELEM(imageSource, uchar, i, j * 3);
      uchar* ip = &CV_IMAGE_ELEM(imgpoints, uchar, i, j * 3);
      //uchar* q = &CV_IMAGE_ELEM(si.image, uchar, i, j);
      CvPoint3D32f* pt = &((CvPoint3D32f*) points->data.fl)[i * imageSource->width + j];
      //points->ptr [3 * i * imageSource->width + j * 3] = p;
      ip[0] = pt->x = p[0];
      ip[1] = pt->y = p[1];
      ip[2] = pt->z = p[2];
      //*q = (uchar) p;
      //cout<<(int)*p<<"-"<<(int)*(p+1)<<"-"<<(int)*(p+2)<<" ";
    }
  }
  cvShowImage("points ", imgpoints);
  cvReleaseImage(&imgpoints);

  cvKMeans2(points, clusterCount, clusters, cvTermCriteria(criteriaType, maxIteration, epsilon));

  //cvZero(img);
  SegImage si;
  si.image = cvCreateImage(cvGetSize(imageSource), 8, 1);
  si.width = imageSource->width;
  si.height = imageSource->height;
  si.segments = clusterCount;
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      //int p = CV_IMAGE_ELEM(clusters, int, i, j);
      int cluster_idx = clusters->data.i[i * imageSource->width + j];
      uchar* q = &CV_IMAGE_ELEM(si.image, uchar, i, j);
      *q = (uchar) cluster_idx;
    }
  }

  IplImage* imageShow = cvCreateImage(cvGetSize(imageSource), 8, 3);
  IplImage* imageGris = cvCreateImage(cvGetSize(imageSource), 8, 1);
  si.colorerSegImage(imageSource, imageShow);
  si.graySegImage(imageGris);
  cvShowImage("image show", imageShow);
  cvShowImage("image source", imageSource);
  cvShowImage("image show gris", imageGris);
  cvWaitKey(0);

  cvReleaseImage(&imageShow);
  cvReleaseMat(&points);
  cvReleaseMat(&clusters);


  //    int key = cvWaitKey(0);
  //    if (key == 27) // 'ESC'
  //      break;
  //  }
}

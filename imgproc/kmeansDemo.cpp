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

void KmeansSegmentDemo::printInformation() {
  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\ts - save segment image, fusion matrix.\n"
          "\t\tNotice: "
          "\t  Choose parametres first, then press space or w\n");

}

void KmeansSegmentDemo::segmentation(IplImage* imageSource) {


  //cvNamedWindow("clusters", 1);
  printInformation();
  IplImage* imgGray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 1);
  IplImage* img_gray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  imageSegmentShow = cvCreateImage(cvGetSize(imageSource), 8, 3);
  hybridImage = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  cvCvtColor(imageSource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);
  int localsize;

  IplImage* imageGaussian = cvCloneImage(imageSource);
  cvSmooth(imageGaussian, imageGaussian, CV_GAUSSIAN);
  imageSource = imageGaussian;
  cvNamedWindow("image", 0);
  cvNamedWindow("Kmeans Segmentation", 0);
  if (segmentImageRef != NULL) {
    cvNamedWindow("image reference", 0);
    imgref = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
    segmentImageRef->colorerSegImage(imageSource, imgref);
    cvShowImage("image reference", imgref);
  }

  cvShowImage("image", imageSource);
  int i, sample_count = imageSource->width * imageSource->height;
  CvMat* points = cvCreateMat(sample_count, 1, CV_32FC3);
  CvMat* clusters = cvCreateMat(sample_count, 1, CV_32SC1);
  //IplImage* imgpoints = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      uchar* p = &CV_IMAGE_ELEM(imageSource, uchar, i, j * 3);

      CvPoint3D32f* pt = &((CvPoint3D32f*) points->data.fl)[i * imageSource->width + j];

      pt->x = p[0];
      pt->y = p[1];
      pt->z = p[2];

    }
  }


  cvCreateTrackbar("Cluster Count", "Kmeans Segmentation", &clusterCount, 100, NULL);
  cvCreateTrackbar("Gaussian Size", "watershed transform", &localsize, 11, NULL);

  while (true) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 's') {
      saveFile();
    }

    if ((char) c == 'w' || (char) c == ' ') {
      cvKMeans2(points, clusterCount, clusters, cvTermCriteria(criteriaType, maxIteration, epsilon));
      //cvZero(img);
      segmentImage.filename = filepath;
      segmentImage.image = cvCreateImage(cvGetSize(imageSource), 8, 1);
      segmentImage.width = imageSource->width;
      segmentImage.height = imageSource->height;
      segmentImage.segments = clusterCount;
      for (int i = 0; i < imageSource->height; i++) {
        for (int j = 0; j < imageSource->width; j++) {
          //int p = CV_IMAGE_ELEM(clusters, int, i, j);
          int cluster_idx = clusters->data.i[i * imageSource->width + j];
          uchar* q = &CV_IMAGE_ELEM(segmentImage.image, uchar, i, j);
          *q = (uchar) cluster_idx;
        }
      }

      IplImage* imageGris = cvCreateImage(cvGetSize(imageSource), 8, 1);
      segmentImage.colorerSegImage(imageSource, imageSegmentShow);
      cvAddWeighted(imageSegmentShow, 0.9, img_gray, 0.1, 0, hybridImage);

      cvShowImage("Kmeans Segmentation", hybridImage);

    }
  }


  cvWaitKey(0);
  cvReleaseMat(&points);
  cvReleaseMat(&clusters);
  cvReleaseImage(&imageSegmentShow);
  cvReleaseImage(&hybridImage);
}

void KmeansSegmentDemo::saveFile() {
  char fileout [ FILENAME_MAX ];
  sprintf(fileout, "%s.kmeans.segment.%d.jpg", filepath, clusterCount);
  cvSaveImage(fileout, imageSegmentShow);
  sprintf(fileout, "%s.kmeans.hybrid.%d.jpg", filepath, clusterCount);
  cvSaveImage(fileout, hybridImage);


  if (segmentImageRef != NULL) {
    sprintf(fileout, "%s.ref.jpg", filepath);
    cvSaveImage(fileout, imgref);

    sprintf(fileout, "%s.kmeans.matrix.%d..txt", filepath, clusterCount);
    MatrixConfusion* mx = segmentImage.compareSegImage(segmentImageRef);
    saveMatrixConfusion(fileout, mx);
    delete mx;
  }


}

SegImage* kmeansSegmentation(IplImage* imageSource, int clusterCount) {


  //cvNamedWindow("clusters", 1);
  //printInformation();
  IplImage* imgGray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 1);
  IplImage* img_gray = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  IplImage* imageSegmentShow = cvCreateImage(cvGetSize(imageSource), 8, 3);
  IplImage* hybridImage = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  cvCvtColor(imageSource, imgGray, CV_RGB2GRAY); //(imgSource);
  cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);
  int localsize;

  IplImage* imageGaussian = cvCloneImage(imageSource);
  cvSmooth(imageGaussian, imageGaussian, CV_GAUSSIAN);
  imageSource = imageGaussian;
//  cvNamedWindow("image", 0);
//  cvNamedWindow("Kmeans Segmentation", 0);
//
//
//  cvShowImage("image", imageSource);
  int i, sample_count = imageSource->width * imageSource->height;
  CvMat* points = cvCreateMat(sample_count, 1, CV_32FC3);
  CvMat* clusters = cvCreateMat(sample_count, 1, CV_32SC1);
  //IplImage* imgpoints = cvCreateImage(cvGetSize(imageSource), IPL_DEPTH_8U, 3);
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      uchar* p = &CV_IMAGE_ELEM(imageSource, uchar, i, j * 3);

      CvPoint3D32f* pt = &((CvPoint3D32f*) points->data.fl)[i * imageSource->width + j];

      pt->x = p[0];
      pt->y = p[1];
      pt->z = p[2];

    }
  }

  int criteriaType = CV_TERMCRIT_EPS + CV_TERMCRIT_ITER;
  double epsilon = 1.0;
  int maxIteration = 10;


  cvKMeans2(points, clusterCount, clusters, cvTermCriteria(criteriaType, maxIteration, epsilon));
  //cvZero(img);
  //IplImage* segmentImage = cvCreateImage(cvGetSize(imageSource), 8, 1);
  SegImage* segmentImage = new SegImage();
  segmentImage->image = cvCreateImage(cvGetSize(imageSource), 8, 1);
  segmentImage->width = imageSource->width;
  segmentImage->height = imageSource->height;
  segmentImage->segments = clusterCount;
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      //int p = CV_IMAGE_ELEM(clusters, int, i, j);
      int cluster_idx = clusters->data.i[i * imageSource->width + j];
      uchar* q = &CV_IMAGE_ELEM(segmentImage->image, uchar, i, j);
      *q = (uchar) cluster_idx;
    }
  }

  

  //cvWaitKey(0);
  cvReleaseMat(&points);
  cvReleaseMat(&clusters);
  cvReleaseImage(&imageSegmentShow);
  cvReleaseImage(&hybridImage);
  return segmentImage;
}

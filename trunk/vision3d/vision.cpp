
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>


#include "vision.h"

using namespace std;
using namespace cv;

void SurfVision3d::matching(char* strimage1, char* strimage2) {
  Mat image1 = imread(strimage1, 1);
  Mat image2 = imread(strimage2, 1);

  SURF surf;
  vector<KeyPoint> keypoints1;
  vector<float> descriptors1;
  vector<KeyPoint> keypoints2;
  vector<float> descriptors2;
  surf.hessianThreshold = 0.05;
  surf.nOctaveLayers = 4;
  surf.nOctaves = 4;
  surf.extended = 2;
  Mat imagegray1, imagegray2;
  cvtColor(image1, imagegray1, CV_RGB2GRAY);
  cvtColor(image2, imagegray2, CV_RGB2GRAY);
  surf(imagegray1, Mat(), keypoints1, descriptors1);
  surf(imagegray2, Mat(), keypoints2, descriptors2);
  Mat imagedraw1;
  Mat imagedraw2;
  drawPoint(keypoints1, image1, imagedraw1);
  drawPoint(keypoints2, image2, imagedraw2);
  imshow("image1", imagedraw1);
  imshow("image2", imagedraw2);
  int size = surf.descriptorSize();
  vector<int> ptpairs;
  //  cout<<"size:"<<size<<endl;
  findPairs(keypoints1, descriptors1, keypoints2, descriptors2, ptpairs, size);
  Mat imagedraw2i;
  Mat imagedraw1i;
  drawMatching2Image(ptpairs, keypoints1, keypoints2, image1, image2, imagedraw2i);
  drawMatchingDeltaImage(ptpairs, keypoints1, keypoints2, image1, imagedraw1i);
  imshow("image correspondance 2", imagedraw2i);
  imshow("image delta", imagedraw1i);
  waitKey();
}

void SurfVision3d::findPairs(const vector<KeyPoint>& objectKeypoints, const vector<float>& objectDescriptors,
  const vector<KeyPoint>& imageKeypoints, const vector<float>& imageDescriptors, vector<int>& ptpairs, int size) {
  int i;
  ptpairs.clear();
  for (i = 0; i < objectKeypoints.size(); i++) {
    const KeyPoint* kp = (const KeyPoint*) &objectKeypoints[i];

    int start1 = i*size;
    int nearest_neighbor = findMatchingOne(*kp, objectDescriptors, imageKeypoints, imageDescriptors, start1, size);
    if (nearest_neighbor != -1) {
      ptpairs.push_back(i);
      ptpairs.push_back(nearest_neighbor);
    }
  }
}

bool SurfVision3d::isNear(const KeyPoint& kp1, const KeyPoint& kp2) {
  //  int minX=kp1.pt.x<kp2.pt.x?kp1.pt.x:kp2.pt.x;
  //  int maxX=kp1.pt.x>kp2.pt.x?kp1.pt.x:kp2.pt.x;
  //  int minY=kp1.pt.y<kp2.pt.y?kp1.pt.y:kp2.pt.y;
  //  int maxY=kp1.pt.y>kp2.pt.y?kp1.pt.y:kp2.pt.y;
  int distance = sqrt(pow(kp1.pt.x - kp2.pt.x, 2) + pow(kp1.pt.y - kp2.pt.y, 2));
  return distance<range;
}

int SurfVision3d::findMatchingOne(const KeyPoint& keypoint, const vector<float>& vec,
  const vector<KeyPoint>& model_keypoints,
  const vector<float>& model_descriptors, int start, int length) {

  int i, neighbor = -1;
  double d, dist1 = 1e6, dist2 = 1e6;

  int start1 = start;
  for (i = 0; i < model_keypoints.size(); i++) {
    const KeyPoint* kp = (const KeyPoint*) &model_keypoints[i];
    if (!isNear(keypoint, *kp))
      continue;
    //    if (laplacian != kp->laplacian)
    //      continue;

    int start2 = i*length;
    //    cout<<start1<<"-"<<start2<<" ";
    d = compareSURFDescriptors(vec, model_descriptors, dist2, start1, start2, length);
    if (d < dist1) {
      dist2 = dist1;
      dist1 = d;
      neighbor = i;
    } else if (d < dist2)
      dist2 = d;
  }

  if (dist1 < 0.5 * dist2)
    return neighbor;
  return -1;
}

double SurfVision3d::compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length) {
  double total_cost = 0;
  assert(length % 4 == 0);
  int i, j;
  for (i = start1, j = start2; i < start1 + length; i += 4, j += 4) {
    double t0 = d1[i] - d2[j];
    double t1 = d1[i + 1] - d2[j + 1];
    double t2 = d1[i + 2] - d2[j + 2];
    double t3 = d1[i + 3] - d2[j + 3];
    total_cost += t0 * t0 + t1 * t1 + t2 * t2 + t3*t3;
    //    cout << total_cost << " ";
    if (total_cost > best)
      break;
  }
  return total_cost;
}

double SurfVision3d::drawPoint(const vector<KeyPoint>& objectKeypoints, Mat& image, Mat& imageDraw) {
  image.copyTo(imageDraw);
  for (int i = 0; i < objectKeypoints.size(); i++) {
    KeyPoint k = objectKeypoints[i];
    circle(imageDraw, k.pt, 2, Scalar(0, 0, 255), 1, 8, 0);
  }
}

double SurfVision3d::drawMatching2Image(const vector<int>& pointIndex,
  const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
  Mat& image1, Mat& image2, Mat& imageDraw) {
  int maxRow = image1.rows < image2.rows ? image2.rows : image1.rows;
  imageDraw = Mat::zeros(maxRow, image1.cols + image2.cols, CV_8UC3);
//  Mat imageLeft = imageDraw.adjustROI(0, maxRow - image1.rows, 0, image2.cols);
//  Mat imageRight = imageDraw.adjustROI(0, maxRow - image2.rows, image1.cols, 0);
  Mat imageLeft=Mat(imageDraw,Rect(0,0,image1.cols,image1.rows));
  Mat imageRight=Mat(imageDraw,Rect(image1.cols,0,image2.cols,image2.rows));
  image1.copyTo(imageLeft);
  image2.copyTo(imageRight);


  for (int i = 0; i < pointIndex.size(); i += 2) {
    int point1 = pointIndex[i];
    int point2 = pointIndex[i + 1];
    Point p1(objectKeypoints1[point1].pt.x, objectKeypoints1[point1].pt.y);
    Point p2(objectKeypoints2[point2].pt.x + image1.cols, objectKeypoints2[point2].pt.y);
    circle(imageDraw, p1, 2, Scalar(0, 0, 255), 1, 8, 0);
    circle(imageDraw, p2, 2, Scalar(0, 255, 0), 1, 8, 0);
    line(imageDraw, p1, p2, Scalar(0, 255, 255), 1, 8, 0);
  }
}

double SurfVision3d::drawMatchingDeltaImage(const vector<int>& pointIndex,
  const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
  Mat& image1, Mat& imageDraw) {
  image1.copyTo(imageDraw);
  for (int i = 0; i < pointIndex.size(); i += 2) {
    int point1 = pointIndex[i];
    int point2 = pointIndex[i + 1];
    Point p1(objectKeypoints1[point1].pt.x, objectKeypoints1[point1].pt.y);
    Point p2(objectKeypoints2[point2].pt.x, objectKeypoints2[point2].pt.y);
    circle(imageDraw, p1, 2, Scalar(0, 0, 255), 1, 8, 0);
//    circle(imageDraw, p2, 2, Scalar(255, 0, 0), 1, 8, 0);
    line(imageDraw, p1, p2, Scalar(0, 255, 255), 1, 8, 0);
  }
}
///*
// * A Demo to OpenCV Implementation of SURF
// * Further Information Refer to "SURF: Speed-Up Robust Feature"
// * Author: Liu Liu
// * liuliu.1987+opencv@gmail.com
// */
//
//
//
//
//// define whether to use approximate nearest-neighbor search
//#define USE_FLANN
//
//
//IplImage *image = 0;
//
//double compareSURFDescriptors(const float* d1, const float* d2, double best, int length) {
//  double total_cost = 0;
//  assert(length % 4 == 0);
//  for (int i = 0; i < length; i += 4) {
//    double t0 = d1[i] - d2[i];
//    double t1 = d1[i + 1] - d2[i + 1];
//    double t2 = d1[i + 2] - d2[i + 2];
//    double t3 = d1[i + 3] - d2[i + 3];
//    total_cost += t0 * t0 + t1 * t1 + t2 * t2 + t3*t3;
//    if (total_cost > best)
//      break;
//  }
//  return total_cost;
//}
//
//int naiveNearestNeighbor(const float* vec, int laplacian,
//  const CvSeq* model_keypoints,
//  const CvSeq* model_descriptors) {
//  int length = (int) (model_descriptors->elem_size / sizeof (float));
//  int i, neighbor = -1;
//  double d, dist1 = 1e6, dist2 = 1e6;
//  CvSeqReader reader, kreader;
//  cvStartReadSeq(model_keypoints, &kreader, 0);
//  cvStartReadSeq(model_descriptors, &reader, 0);
//
//  for (i = 0; i < model_descriptors->total; i++) {
//    const CvSURFPoint* kp = (const CvSURFPoint*) kreader.ptr;
//    const float* mvec = (const float*) reader.ptr;
//    CV_NEXT_SEQ_ELEM(kreader.seq->elem_size, kreader);
//    CV_NEXT_SEQ_ELEM(reader.seq->elem_size, reader);
//    if (laplacian != kp->laplacian)
//      continue;
//    d = compareSURFDescriptors(vec, mvec, dist2, length);
//    if (d < dist1) {
//      dist2 = dist1;
//      dist1 = d;
//      neighbor = i;
//    } else if (d < dist2)
//      dist2 = d;
//  }
//  if (dist1 < 0.6 * dist2)
//    return neighbor;
//  return -1;
//}
//
//void findPairs(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
//  const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs) {
//  int i;
//  CvSeqReader reader, kreader;
//  cvStartReadSeq(objectKeypoints, &kreader);
//  cvStartReadSeq(objectDescriptors, &reader);
//  ptpairs.clear();
//
//  for (i = 0; i < objectDescriptors->total; i++) {
//    const CvSURFPoint* kp = (const CvSURFPoint*) kreader.ptr;
//    const float* descriptor = (const float*) reader.ptr;
//    CV_NEXT_SEQ_ELEM(kreader.seq->elem_size, kreader);
//    CV_NEXT_SEQ_ELEM(reader.seq->elem_size, reader);
//    int nearest_neighbor = naiveNearestNeighbor(descriptor, kp->laplacian, imageKeypoints, imageDescriptors);
//    if (nearest_neighbor >= 0) {
//      ptpairs.push_back(i);
//      ptpairs.push_back(nearest_neighbor);
//    }
//  }
//}
//
//void
//flannFindPairs(const CvSeq*, const CvSeq* objectDescriptors,
//  const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs) {
//  int length = (int) (objectDescriptors->elem_size / sizeof (float));
//
//  cv::Mat m_object(objectDescriptors->total, length, CV_32F);
//  cv::Mat m_image(imageDescriptors->total, length, CV_32F);
//
//
//  // copy descriptors
//  CvSeqReader obj_reader;
//  float* obj_ptr = m_object.ptr<float>(0);
//  cvStartReadSeq(objectDescriptors, &obj_reader);
//  for (int i = 0; i < objectDescriptors->total; i++) {
//    const float* descriptor = (const float*) obj_reader.ptr;
//    CV_NEXT_SEQ_ELEM(obj_reader.seq->elem_size, obj_reader);
//    memcpy(obj_ptr, descriptor, length * sizeof (float));
//    obj_ptr += length;
//  }
//  CvSeqReader img_reader;
//  float* img_ptr = m_image.ptr<float>(0);
//  cvStartReadSeq(imageDescriptors, &img_reader);
//  for (int i = 0; i < imageDescriptors->total; i++) {
//    const float* descriptor = (const float*) img_reader.ptr;
//    CV_NEXT_SEQ_ELEM(img_reader.seq->elem_size, img_reader);
//    memcpy(img_ptr, descriptor, length * sizeof (float));
//    img_ptr += length;
//  }
//
//  // find nearest neighbors using FLANN
//  cv::Mat m_indices(objectDescriptors->total, 2, CV_32S);
//  cv::Mat m_dists(objectDescriptors->total, 2, CV_32F);
//  cv::flann::Index flann_index(m_image, cv::flann::KDTreeIndexParams(4)); // using 4 randomized kdtrees
//  flann_index.knnSearch(m_object, m_indices, m_dists, 2, cv::flann::SearchParams(64)); // maximum number of leafs checked
//
//  int* indices_ptr = m_indices.ptr<int>(0);
//  float* dists_ptr = m_dists.ptr<float>(0);
//  for (int i = 0; i < m_indices.rows; ++i) {
//    if (dists_ptr[2 * i] < 0.6 * dists_ptr[2 * i + 1]) {
//      ptpairs.push_back(i);
//      ptpairs.push_back(indices_ptr[2 * i]);
//    }
//  }
//}
//
///* a rough implementation for object location */
//int
//locatePlanarObject(const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
//  const CvSeq* imageKeypoints, const CvSeq* imageDescriptors,
//  const CvPoint src_corners[4], CvPoint dst_corners[4]) {
//  double h[9];
//  CvMat _h = cvMat(3, 3, CV_64F, h);
//  vector<int> ptpairs;
//  vector<CvPoint2D32f> pt1, pt2;
//  CvMat _pt1, _pt2;
//  int i, n;
//
//#ifdef USE_FLANN
//  flannFindPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs);
//#else
//  findPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs);
//#endif
//
//  n = ptpairs.size() / 2;
//  if (n < 4)
//    return 0;
//
//  pt1.resize(n);
//  pt2.resize(n);
//  for (i = 0; i < n; i++) {
//    pt1[i] = ((CvSURFPoint*) cvGetSeqElem(objectKeypoints, ptpairs[i * 2]))->pt;
//    pt2[i] = ((CvSURFPoint*) cvGetSeqElem(imageKeypoints, ptpairs[i * 2 + 1]))->pt;
//  }
//
//  _pt1 = cvMat(1, n, CV_32FC2, &pt1[0]);
//  _pt2 = cvMat(1, n, CV_32FC2, &pt2[0]);
//  if (!cvFindHomography(&_pt1, &_pt2, &_h, CV_RANSAC, 5))
//    return 0;
//
//  for (i = 0; i < 4; i++) {
//    double x = src_corners[i].x, y = src_corners[i].y;
//    double Z = 1. / (h[6] * x + h[7] * y + h[8]);
//    double X = (h[0] * x + h[1] * y + h[2]) * Z;
//    double Y = (h[3] * x + h[4] * y + h[5]) * Z;
//    dst_corners[i] = cvPoint(cvRound(X), cvRound(Y));
//  }
//
//  return 1;
//}
//
//int main(int argc, char** argv) {
//  const char* object_filename = argc == 3 ? argv[1] : "box.png";
//  const char* scene_filename = argc == 3 ? argv[2] : "box_in_scene.png";
//
//  CvMemStorage* storage = cvCreateMemStorage(0);
//
//  cvNamedWindow("Object", 1);
//  cvNamedWindow("Object Correspond", 1);
//
//  static CvScalar colors[] = {
//    {
//      {0, 0, 255}
//    },
//    {
//      {0, 128, 255}
//    },
//    {
//      {0, 255, 255}
//    },
//    {
//      {0, 255, 0}
//    },
//    {
//      {255, 128, 0}
//    },
//    {
//      {255, 255, 0}
//    },
//    {
//      {255, 0, 0}
//    },
//    {
//      {255, 0, 255}
//    },
//    {
//      {255, 255, 255}
//    }
//  };
//
//  IplImage* object = cvLoadImage(object_filename, CV_LOAD_IMAGE_GRAYSCALE);
//  IplImage* image = cvLoadImage(scene_filename, CV_LOAD_IMAGE_GRAYSCALE);
//  if (!object || !image) {
//    fprintf(stderr, "Can not load %s and/or %s\n"
//      "Usage: find_obj [<object_filename> <scene_filename>]\n",
//      object_filename, scene_filename);
//    exit(-1);
//  }
//  IplImage* object_color = cvCreateImage(cvGetSize(object), 8, 3);
//  cvCvtColor(object, object_color, CV_GRAY2BGR);
//
//  CvSeq *objectKeypoints = 0, *objectDescriptors = 0;
//  CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
//  int i;
//  CvSURFParams params = cvSURFParams(500, 1);
//
//  double tt = (double) cvGetTickCount();
//  cvExtractSURF(object, 0, &objectKeypoints, &objectDescriptors, storage, params);
//  printf("Object Descriptors: %d\n", objectDescriptors->total);
//  cvExtractSURF(image, 0, &imageKeypoints, &imageDescriptors, storage, params);
//  printf("Image Descriptors: %d\n", imageDescriptors->total);
//  tt = (double) cvGetTickCount() - tt;
//  printf("Extraction time = %gms\n", tt / (cvGetTickFrequency()*1000.));
//  CvPoint src_corners[4] = {
//    {0, 0},
//    {object->width, 0},
//    {object->width, object->height},
//    {0, object->height}
//  };
//  CvPoint dst_corners[4];
//  IplImage* correspond = cvCreateImage(cvSize(image->width, object->height + image->height), 8, 1);
//  cvSetImageROI(correspond, cvRect(0, 0, object->width, object->height));
//  cvCopy(object, correspond);
//  cvSetImageROI(correspond, cvRect(0, object->height, correspond->width, correspond->height));
//  cvCopy(image, correspond);
//  cvResetImageROI(correspond);
//
//#ifdef USE_FLANN
//  printf("Using approximate nearest neighbor search\n");
//#endif
//
//  if (locatePlanarObject(objectKeypoints, objectDescriptors, imageKeypoints,
//    imageDescriptors, src_corners, dst_corners)) {
//    for (i = 0; i < 4; i++) {
//      CvPoint r1 = dst_corners[i % 4];
//      CvPoint r2 = dst_corners[(i + 1) % 4];
//      cvLine(correspond, cvPoint(r1.x, r1.y + object->height),
//        cvPoint(r2.x, r2.y + object->height), colors[8]);
//    }
//  }
//  vector<int> ptpairs;
//#ifdef USE_FLANN
//  flannFindPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs);
//#else
//  findPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs);
//#endif
//  for (i = 0; i < (int) ptpairs.size(); i += 2) {
//    CvSURFPoint* r1 = (CvSURFPoint*) cvGetSeqElem(objectKeypoints, ptpairs[i]);
//    CvSURFPoint* r2 = (CvSURFPoint*) cvGetSeqElem(imageKeypoints, ptpairs[i + 1]);
//    cvLine(correspond, cvPointFrom32f(r1->pt),
//      cvPoint(cvRound(r2->pt.x), cvRound(r2->pt.y + object->height)), colors[8]);
//  }
//
//  cvShowImage("Object Correspond", correspond);
//  for (i = 0; i < objectKeypoints->total; i++) {
//    CvSURFPoint* r = (CvSURFPoint*) cvGetSeqElem(objectKeypoints, i);
//    CvPoint center;
//    int radius;
//    center.x = cvRound(r->pt.x);
//    center.y = cvRound(r->pt.y);
//    radius = cvRound(r->size * 1.2 / 9. * 2);
//    cvCircle(object_color, center, radius, colors[0], 1, 8, 0);
//  }
//  cvShowImage("Object", object_color);
//
//  cvWaitKey(0);
//
//  cvDestroyWindow("Object");
//  cvDestroyWindow("Object SURF");
//  cvDestroyWindow("Object Correspond");
//
//  return 0;
//}

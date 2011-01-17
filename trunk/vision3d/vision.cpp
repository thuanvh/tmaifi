
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
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

void fromMat2Array(const Mat& mat,vector<float>& array){
  for(int i=0; i<mat.rows; i++){
    for(int j=0; j<mat.cols; j++){
      array.push_back(mat.at<float>(i,j));
    }
  }
}

void FeatureVision3d::matching(char* strimage1, char* strimage2, char* outimage) {

  Mat image1 = imread(strimage1, 1);
  Mat image2 = imread(strimage2, 1);

  vector<KeyPoint> keypoints1;
  vector<KeyPoint> keypoints2;
  vector<float> descriptors1;
  vector<float> descriptors2;
  Mat imagegray1, imagegray2;
  cvtColor(image1, imagegray1, CV_RGB2GRAY);
  cvtColor(image2, imagegray2, CV_RGB2GRAY);
  int size;
  if (method == USE_SURF) {
    SURF surf;
    surf.hessianThreshold = 0.05;
    surf.nOctaveLayers = 4;
    surf.nOctaves = 4;
    surf.extended = 2;
    size = surf.descriptorSize();
    surf(imagegray1, Mat(), keypoints1, descriptors1);
    surf(imagegray2, Mat(), keypoints2, descriptors2);
  } else if (method == USE_SIFT) {
    SIFT sift;
    Mat matdescriptors1;
    Mat matdescriptors2;
    size = sift.descriptorSize();
    sift(imagegray1, Mat(), keypoints1, matdescriptors1);
    sift(imagegray2, Mat(), keypoints2, matdescriptors2);
//    cout << size << endl;
//    cout << keypoints1.size() << endl;
//    cout << matdescriptors1.rows << "*" << matdescriptors1.cols << endl;
//    cout << "chanel:" << matdescriptors1.channels() << endl;
//    cout << "type:" << matdescriptors1.type() << endl;
//    getchar();
    fromMat2Array(matdescriptors1,descriptors1);
    fromMat2Array(matdescriptors2,descriptors2);
  }
  if (!isQuiet) {
    Mat imagedraw1;
    Mat imagedraw2;
    drawPoint(keypoints1, image1, imagedraw1);
    drawPoint(keypoints2, image2, imagedraw2);
    imshow("image1", imagedraw1);
    imshow("image2", imagedraw2);
  }

  vector<int> ptpairs;
  //  cout<<"size:"<<size<<endl;
  findPairs(keypoints1, descriptors1, keypoints2, descriptors2, ptpairs, size);
  if (!isQuiet) {
    Mat imagedraw2i;
    drawMatching2Image(ptpairs, keypoints1, keypoints2, image1, image2, imagedraw2i);
    imshow("image correspondance 2", imagedraw2i);
  }
  Mat imagedraw1i;
  drawMatchingDeltaImage(ptpairs, keypoints1, keypoints2, image1, imagedraw1i);
  if (!isQuiet) {
    imshow("image delta", imagedraw1i);

  }
  if (outimage != NULL) {
    imwrite(outimage, imagedraw1i);
  }
  calculateFondamentalMatrix(ptpairs, keypoints1, keypoints2, image1, image2);
  if (!isQuiet) {
    waitKey();
  }
}

void insertIntoMap(vector<double>& distanceArray, vector<int>& ptpairs, double distance, int index1, int index2) {
  //  distanceArray.push_back(distance);
  //  ptpairs.push_back(index1);
  //  ptpairs.push_back(index2);
  //  return;

  vector<double>::iterator pos = lower_bound(distanceArray.begin(), distanceArray.end(), distance);

  if (pos < distanceArray.end()) {
    int delta = pos - distanceArray.begin();
    //    cout << delta << endl;
    distanceArray.insert(pos, distance);
    ptpairs.insert(ptpairs.begin() + delta * 2, index1);
    ptpairs.insert(ptpairs.begin() + delta * 2 + 1, index2);
  } else {
    distanceArray.push_back(distance);
    ptpairs.push_back(index1);
    ptpairs.push_back(index2);
  }
  //  for (int i = 0; i < distanceArray.size(); i++) {
  //    cout << distanceArray[i] << " ";
  //  }
  //  cout << endl;
  //  getchar();
}

void FeatureVision3d::findPairs(const vector<KeyPoint>& objectKeypoints, const vector<float>& objectDescriptors,
  const vector<KeyPoint>& imageKeypoints, const vector<float>& imageDescriptors, vector<int>& ptpairs, int size) {
  int i;
  ptpairs.clear();
  vector<double> distVec;
  for (i = 0; i < objectKeypoints.size(); i++) {
    const KeyPoint* kp = (const KeyPoint*) &objectKeypoints[i];
    double distance;
    int start1 = i*size;
    int nearest_neighbor = findMatchingOne(*kp, objectDescriptors, imageKeypoints, imageDescriptors, start1, size, distance);
    if (nearest_neighbor != -1) {
      insertIntoMap(distVec, ptpairs, distance, i, nearest_neighbor);
      //      ptpairs.push_back(i);
      //      ptpairs.push_back(nearest_neighbor);
    }
  }
}

bool FeatureVision3d::isNear(const KeyPoint& kp1, const KeyPoint& kp2) {
  //  int minX=kp1.pt.x<kp2.pt.x?kp1.pt.x:kp2.pt.x;
  //  int maxX=kp1.pt.x>kp2.pt.x?kp1.pt.x:kp2.pt.x;
  //  int minY=kp1.pt.y<kp2.pt.y?kp1.pt.y:kp2.pt.y;
  //  int maxY=kp1.pt.y>kp2.pt.y?kp1.pt.y:kp2.pt.y;
  int distance = sqrt(pow(kp1.pt.x - kp2.pt.x, 2) + pow(kp1.pt.y - kp2.pt.y, 2));
  return distance<range;
}

int FeatureVision3d::findMatchingOne(const KeyPoint& keypoint, const vector<float>& vec,
  const vector<KeyPoint>& model_keypoints,
  const vector<float>& model_descriptors, int start, int length, double& distance) {

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

  if (dist1 < 0.5 * dist2) {
    distance = dist1;
    return neighbor;
  }
  return -1;
}

double FeatureVision3d::compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length) {
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

double FeatureVision3d::drawPoint(const vector<KeyPoint>& objectKeypoints, Mat& image, Mat& imageDraw) {
  image.copyTo(imageDraw);
  for (int i = 0; i < objectKeypoints.size(); i++) {
    KeyPoint k = objectKeypoints[i];
    
    circle(imageDraw, k.pt, (int)k.size, Scalar(0, 0, 255), 1, 8, 0);
  }
}

double FeatureVision3d::drawMatching2Image(const vector<int>& pointIndex,
  const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
  Mat& image1, Mat& image2, Mat& imageDraw) {
  int maxRow = image1.rows < image2.rows ? image2.rows : image1.rows;
  imageDraw = Mat(maxRow, image1.cols + image2.cols, CV_8UC3);
  //  Mat imageLeft = imageDraw.adjustROI(0, maxRow - image1.rows, 0, image2.cols);
  //  Mat imageRight = imageDraw.adjustROI(0, maxRow - image2.rows, image1.cols, 0);
  Mat imageLeft = Mat(imageDraw, Rect(0, 0, image1.cols, image1.rows));
  Mat imageRight = Mat(imageDraw, Rect(image1.cols, 0, image2.cols, image2.rows));
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

double FeatureVision3d::drawMatchingDeltaImage(const vector<int>& pointIndex,
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

void FeatureVision3d::calculateFondamentalMatrix(const vector<int>& pointIndex,
  const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
  const Mat& image1, const Mat& image2) {

  //create the output fundamental matrix
  int numPoints = point_count < pointIndex.size() ? point_count : pointIndex.size();
  Mat fundMatr = cvCreateMat(3, 3, CV_32F);
  Mat points1 = Mat(1, numPoints, CV_32FC2);
  Mat points2 = Mat(1, numPoints, CV_32FC2);
  //  Mat status = Mat::zeros(1,point_count,CV_8UC1);

  for (int i = 0; i < numPoints * 2; i += 2) {
    int p1 = pointIndex[i];
    int p2 = pointIndex[i + 1];

    ((float*) points1.data)[i] = objectKeypoints1[p1].pt.x;
    ((float*) points1.data)[i + 1] = objectKeypoints1[p1].pt.y;

    ((float*) points2.data)[i] = objectKeypoints2[p2].pt.x;
    ((float*) points2.data)[i + 1] = objectKeypoints2[p2].pt.y;

    //    cout << p1 << " " << objectKeypoints1[p1].pt.x << "," << objectKeypoints1[p1].pt.y << " ";
    //    cout << p2 << " " << objectKeypoints2[p2].pt.x << "," << objectKeypoints2[p2].pt.y << endl;
  }
  fundMatr = findFundamentalMat(points1, points2, FM_RANSAC, 1.0, 0.99);
  cout << "Fundamental Matrix:" << endl;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      cout << fundMatr.at<float>(i, j) << "\t";
    }
    cout << endl;
  }

  displayEpipoleLine(fundMatr, image1, points2, 2, numPoints, points1, "image epipole 1");
  displayEpipoleLine(fundMatr, image2, points1, 1, numPoints, points2, "image epipole 2");
}

void FeatureVision3d::displayEpipoleLine(const Mat& fundMatr, const Mat& image, const Mat& points,
  int startImage, int numPoints, const Mat& pointsThisImage, const char* windowName) {
  //now visualize the fundamental matrix

  vector<Vec3f> corrLines;

  //specify which direction to compute epipolar lines
  //  int startImage = 2;
  computeCorrespondEpilines(points,
    startImage, //means points are in image 1
    fundMatr,
    corrLines);

  //create output window
  //  char windowName[100];
  //  strcpy_s(windowName, ”Output Window”);
  namedWindow(windowName, CV_WINDOW_AUTOSIZE);

  //for all the points set the point and corresponding epipolar line
  //and determine where the epipolar line intersects the image plane
  //then display all this info

  int width = image.cols;
  int height = image.rows;
  Mat imgcpy = image.clone();
  for (int i1 = 0; i1 < numPoints; i1++) {
    float a = corrLines[i1][0];
    float b = corrLines[i1][1];
    float c = corrLines[i1][2];
    //    cout << a << "," << b << "," << c << endl;

    Point epipolarLinePoint[2]; //, epipolarLinePoint2;
    Point points[4];
    points[0] = b == 0 ? Point(0, -1) : Point(0, round(-c / b));
    points[1] = a == 0 ? Point(-1, 0) : Point(round(-c / a), 0);
    points[2] = b == 0 ? Point(width - 1, -1) : Point(width - 1, round((-c - a * width) / b));
    points[3] = a == 0 ? Point(-1, height - 1) : Point(round((-c - b * height) / a), height - 1);

    int j = 0;
    for (int i = 0; i < 4; i++) {
      if (j >= 2) break;
      //      cout<<"Points:"<<points[i].x<<"*"<<points[i].y<<endl;
      if (points[i].x < width && points[i].x >= 0 && points[i].y < height && points[i].y >= 0)
        epipolarLinePoint[j++] = points[i];
    }
    //    cout<<"="<<j<<endl;
    if (j < 2)
      continue;
    circle(imgcpy, Point(((float*) pointsThisImage.data)[i1 * 2], ((float*) pointsThisImage.data)[i1 * 2 + 1]), 2, Scalar(0, 0, 255), 1, 8, 0);
    line(imgcpy, epipolarLinePoint[0], epipolarLinePoint[1], CV_RGB(0, 255, 0));

  }
  imshow(windowName, imgcpy);
}

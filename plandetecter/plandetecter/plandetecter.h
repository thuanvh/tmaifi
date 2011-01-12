/*
 * File:   motiondetector.h
 * Author: thuanvh
 *
 * Created on November 6, 2010, 10:53 PM
 */

#ifndef PLANDECTECTER_H
#define	PLANDECTECTER_H

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>


using namespace cv;
#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

#define STR_TRACKING_POSITION "pos"
#define STR_TRACKING_POSITION_VELOCITY "posveloc"

#define MAXINT 1e6
#define MININT -1e6

#define STR_PIXEL 	"pixel"
#define STR_LIKELIHOOD 	"likelihood"
#define STR_HISTOGRAM	"histogram"
#define STR_CONTOUR	"contour"

enum AlgoType{PixelToPixel,LikelihoodRatio,Histogram,Contour};
class detectMethod{
public:
  virtual bool isDifferentPlan(Mat& image1,Mat& image2)=0;
  virtual double calculateDistance(Mat& image1,Mat& image2)=0;
  virtual void setDecisionThreshold(float thres)=0;
  virtual float getDecisionThreshold()=0;
};
class plandetecter{
public:
  detectMethod* algo;
  AlgoType method;
  vector<string> file;
  vector<int> plan;
  void detectPlan(char* folder);
};

class pixelToPixelMethod:public detectMethod{
public:
  int seuil_delta;
  float seuil_pixel;  
  bool isDifferentPlan(Mat& image1,Mat& image2);
  double calculateDistance(Mat& image1,Mat& image2);
  void setDecisionThreshold(float thres);
  float getDecisionThreshold();
};
class likelihoodRatioMethod:public detectMethod{
public:
  double seuil_h;
  double seuil_p;
  bool isDifferentPlan(Mat& image1,Mat& image2);  
  double calculateDistance(Mat& image1,Mat& image2);
  void setDecisionThreshold(float thres);
  float getDecisionThreshold();
};
class histogramMethod:public detectMethod{
public:
  double seuil_histo;
  bool isDifferentPlan(Mat& image1,Mat& image2);
  double calculateDistance(Mat& image1,Mat& image2);
  void setDecisionThreshold(float thres);
  float getDecisionThreshold();
};
class contourMethod:public detectMethod{
public:
  int contourDistance;
  //int gradientThreshold;
  int contourThreshold;  
  int canny_threshold1;
  int canny_threshold2;
  int apertureSize;
  
  contourMethod(){
    apertureSize=3;
  }
  bool isDifferentPlan(Mat& image1,Mat& image2);
  double calculateDistance(Mat& image1,Mat& image2);
  void setDecisionThreshold(float thres);
  float getDecisionThreshold();
};
class thresholdAdaptifAlgo{
private:
  void deviceMat2(const Mat& mat,Mat& left,Mat& right,Mat& matN);
public:
  detectMethod* algo;
  AlgoType method;
  vector<string> file;
  vector<int> plan;
  vector<double> thresh;
  vector<double> normaledDiffNoPlan;
  void detectPlan(char* folder);
};
class matrixConfusion{
private:
  int matrix[2][2];  
  int* ref;
  int reflength;
  double recall[3];
  double precision[3];
public:  
  matrixConfusion();
  void setReference(int* arrayRef,int length);
  void setValue(int frameInputId,bool isPlan);
  void calculate();
  void print();
  
};
#endif	/* PLANDECTECTER_H */
  
  
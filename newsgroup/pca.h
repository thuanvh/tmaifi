#ifndef PCA_H
#define PCA_H
#include "tf-idf.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;
class PCAConverter{
  Mat eigenVectors;
  Mat mean;  
  int maxComponent;
public:
  void import(vector<category>& categorylist,vector<string>& dict);
  void save();
  void load();
  void getProjection();
};

#endif
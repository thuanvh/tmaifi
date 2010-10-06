/* 
 * File:   digitdetection.h
 * Author: thuanvh
 *
 * Created on October 3, 2010, 10:19 AM
 */

#ifndef DIGITDETECTION_POINT_H
#define	DIGITDETECTION_POINT_H

#include <opencv/cvaux.h>
#include <opencv/highgui.h>
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
#include <fstream>
#include <iomanip>

using namespace std;
using namespace cv;

#define MODALITE 10
struct descriptor_prob{
    double nb_endingpoint[MODALITE];
    double nb_intersection[MODALITE];
    double nb_endingpoint_quartier[4][MODALITE];
    //double nb_endingpoint[MODALITE];
    double prob;
};
struct descriptor{
  int nb_endingpoint;
  int nb_intersection;
  int nb_endingpoint_quartier[4];
  int label;
  descriptor(){
  nb_endingpoint=0;
  nb_intersection=0;
  for(int i=0; i<4; i++)
  nb_endingpoint_quartier[i]=0;
  }
};
void calculDesPointDeBout(const Mat& img, descriptor& des);
void learningPoint(const char* trainingimage,const char* traininglabel,const char* outputDirPath);
void showPoint(float** hist,const char* histoname,const char* outputDirPath);
void TestingPoint(char* histoDirPath,char* testImagePath, const char* testLabelPath);
void LoadPoint(float** hist, const char* name);
void SavePoint(float** hist,char* name);
float calculPointProb(int** img,float** hist);


#endif	/* DIGITDETECTION_POINT_H */


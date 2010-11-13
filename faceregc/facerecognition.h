/* 
 * File:   facerecognition.h
 * Author: thuanvh
 *
 * Created on October 23, 2010, 3:30 PM
 */

#ifndef FACERECOGNITION_H
#define	FACERECOGNITION_H


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

#define FUNC_LEARNING 1
#define FUNC_TEST 2

#define CMD_MAHALANOBIS 1
#define CMD_BAYES_PIXEL 2
#define CMD_BAYES_POINT 3

#define STR_CMD_MAHALANOBIS "mahalanobis"
#define STR_CMD_BAYES_PIXEL "bayes-pixel"
#define STR_CMD_BAYES_POINT "bayes-point"

#define HISTO_SCALE 5
#define HISTO_DELTA 5

#define STR_LEARNING "learn"
#define STR_TEST "test"

#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

void learning(const char* dirPath, const char* name);
void showHistogram(float** hist,const char* histoname,const char* outputDirPath);
void testing(const char* dirPath, const char* name);
void LoadHistograme(float** hist, const char* name);
void SaveHistograme(float** hist,char* name);
float calculProb(int** img,float** hist);
float calculProb(const Mat& img, float** hist);
void generateHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath);
float calculProb(float** img, float** hist,int height,int width);


void findPairs(const vector<KeyPoint>& objectKeypoints, const vector<float>& objectDescriptors,
  const vector<KeyPoint>& imageKeypoints, const vector<float>& imageDescriptors, vector<int>& ptpairs, int size);
int naiveNearestNeighbor(const vector<float>& vec, int laplacian,
  const vector<KeyPoint>& model_keypoints,
  const vector<float>& model_descriptors, int start, int length) ;
double compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length);
void extractSUFT(const Mat& img, vector<KeyPoint>& allKeyPoints, vector<float>& alldescriptors, ostream& featurefile, bool createDict, ostream& dictFile);
void saveToDict(const KeyPoint& kp, const vector<float>& descriptors, ostream& dictFile, int start, int size);
void loadFromDict(vector<KeyPoint>& kplist, vector<float>& descriptors, istream& dictFile);
void loadDictLookup(vector<bool*>& dictlookup, const char* filename);
void saveDictLookup(const vector<bool*>& dictlookup, const char* filename);

#endif	/* FACERECOGNITION_H */


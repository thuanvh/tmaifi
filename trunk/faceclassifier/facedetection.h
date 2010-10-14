/* 
 * File:   facedetection.h
 * Author: thuanvh
 *
 * Created on October 13, 2010, 11:24 PM
 */

#ifndef FACEDETECTION_H
#define	FACEDETECTION_H

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

void learningHisto(const char* trainingimage,const char* traininglabel,const char* outputDirPath, const char* command);
void showHistogram(float** hist,const char* histoname,const char* outputDirPath);
void testingHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath, const char* command);
void LoadHistograme(float** hist, const char* name);
void SaveHistograme(float** hist,char* name);
float calculProb(int** img,float** hist);
float calculProb(const Mat& img, float** hist);
void generateHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath);
float calculProb(float** img, float** hist,int height,int width);
#endif	/* FACEDETECTION_H */


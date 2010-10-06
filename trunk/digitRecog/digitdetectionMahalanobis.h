/* 
 * File:   digitdetection.h
 * Author: thuanvh
 *
 * Created on October 3, 2010, 10:19 AM
 */

#ifndef DIGITDETECTIONMahalanobis_H
#define	DIGITDETECTIONMahalanobis_H

#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <stdio.h>

using namespace std;
using namespace cv;

void learningMahalanobis(const char* trainingimage,const char* traininglabel,const char* outputDirPath);
//void showHistogram(float** hist,const char* histoname,const char* outputDirPath);
void TestingMahalanobis(const char* histoDirPath,const char* testImagePath, const char* testLabelPath);

void SaveMatrix(Mat mat, const char* matname);
void LoadMatrix(Mat& mat, const char* name);
void showMatrixMean(Mat& mean, const char* histoname, const char* outputDirPath);
#endif	/* DIGITDETECTION_H */


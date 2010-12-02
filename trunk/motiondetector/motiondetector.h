/* 
 * File:   motiondetector.h
 * Author: thuanvh
 *
 * Created on November 6, 2010, 10:53 PM
 */

#ifndef MOTIONDETECTOR_H
#define	MOTIONDETECTOR_H

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
#include "patchItem.h"

using namespace cv;
#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

#define MAXINT 1e6
#define MININT -1e6



void MotionDetection(char* videoFile, int fps, int queuesize);


// Normalize histogramme

void normalizeHistogram(MatND& hist, int totalpixel, int degree);

void resizeImageColor(const Mat& src, Mat& dst, int num_color);
// calculer le nombre total de valeur dans histogramme

int histNo(const MatND& hist, int degree);
// Sauver la matrice

void SaveMatrix(const MatND& hist, vector<double>& caracter, int degree);

void extractHistoColorRGB(const Mat & src, int colorSize, vector<double>& caracter);

void GetCaracteristic(const Mat& image, vector<double>& caracteristic);
// Calculer la distance entre Couleur

double getColorHistoDistance(const vector<double>& learningVector, const vector<double>& testingVector);

double GetImageDistance(const PatchItem& image1, const PatchItem& image2);

void FrameMotionMatching(vector<PatchItem*>& listImage, vector<PatchItem*>& mapImage, int& totalLabel, int defaultVisited);

void FrameMotionExtraire(const Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY, vector<PatchItem*>& listImage);
void FrameMotionFilter(int maxLabel, int* maxX, int* maxY, int* minX, int* minY);
void markItem(Mat& markedImage, vector<PatchItem*> trackingList);
void FrameMotionMarking(Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY);
void FrameMotionMarkingLabeled(Mat& image, vector<PatchItem*> trackingList);
void FrameMotionDetect(const Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY);

int KalmanMotionDetection(int argc, char** argv);

void printImageLabel(const Mat& img);

void printImage(const Mat& img);

void LabelingConnectedComponent(const Mat& img, Mat& dst, int& label);

void DotMotionMesureMarking(Mat& image, vector<PatchItem*> trackingList);
void DotMotionPredictMarking(Mat& image, vector<PatchItem*> trackingList);
void DotMotionCorrectMarking(Mat& image, vector<PatchItem*> trackingList);

#endif	/* MOTIONDETECTOR_H */


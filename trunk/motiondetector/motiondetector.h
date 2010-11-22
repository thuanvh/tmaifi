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
using namespace cv;
#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

#define MAXINT 1e6
#define MININT -1e6

void FrameMotionMarking(Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY);
void FrameMotionDetect(const Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY);
void MotionDetection(char* videoFile, int fps, int queuesize);
void LabelingConnectedComponent(const Mat& img, Mat& dst, int& label);
void printImage(const Mat& img);

#endif	/* MOTIONDETECTOR_H */


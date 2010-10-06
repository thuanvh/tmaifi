/* 
 * File:   skindetector.h
 * Author: thuanvh
 *
 * Created on September 30, 2010, 9:12 PM
 */

#ifndef SKINDETECTOR_H
#define	SKINDETECTOR_H

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "skindetector.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#define FUNC_LEARNING 1
#define FUNC_TEST 2

#define STR_LEARNING "learning"
#define STR_TEST "test"

#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

using namespace std;
using namespace cv;

void Learning(const char* dirPath,const char* histoName);
void Testing(const char* testPath,const char* histoName);
void normalizeHistogram(MatND& hist);
void displayHistogram(const MatND& hist, const char* name);
void SaveHistograme(const MatND& hist, string name);
void LoadHistograme(MatND& hist, string name);
void SaveHistograme(const MatND& hist, const char* name);
void LoadHistograme(MatND& hist, const char* name);
#endif	/* SKINDETECTOR_H */


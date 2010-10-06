/* 
 * File:   digitdetection.h
 * Author: thuanvh
 *
 * Created on October 3, 2010, 10:19 AM
 */

#ifndef DIGITDETECTION_H
#define	DIGITDETECTION_H

#define FUNC_LEARNING 1
#define FUNC_TEST 2

#define CMD_MAHALANOBIS 1
#define CMD_BAYES_PIXEL 2
#define CMD_BAYES_POINT 3

#define STR_CMD_MAHALANOBIS "mahalanobis"
#define STR_CMD_BAYES_PIXEL "bayes-pixel"
#define STR_CMD_BAYES_POINT "bayes-point"

#define STR_LEARNING "learning"
#define STR_TEST "test"

#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

void learningHisto(const char* trainingimage,const char* traininglabel,const char* outputDirPath);
void showHistogram(float** hist,const char* histoname,const char* outputDirPath);
void TestingHisto(char* histoDirPath,char* testImagePath, const char* testLabelPath);
void LoadHistograme(float** hist, const char* name);
void SaveHistograme(float** hist,char* name);
float calculProb(int** img,float** hist);


#endif	/* DIGITDETECTION_H */


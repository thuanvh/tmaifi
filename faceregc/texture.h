/*
 * File:   texture.h
 * Author: thuanvh
 *
 * Created on October 17, 2010, 10:42 PM
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

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

#define FUNC_LEARNING 1
#define FUNC_TEST 2
#define FUNC_CROSS_TEST 3
#define FUNC_SEGMENT 4
#define FUNC_SEGMENT_CAH 5

#define STR_LEARNING "learn"
#define STR_TEST "test"
#define STR_CROSS_TEST "crosstest"
#define STR_SEGMENT "segment"
#define STR_SEGMENT_CAH "segmentcah"

#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

#define NUM_MATRIX 8

using namespace std;
using namespace cv;

void extraitFeatureVector(const char* dirPath, const char* name,int graySize);
void reduireNiveauDeGris(Mat& image, int size);
void freeMatrix(double*** concurrenceArray, int graySize);
void calculerMatrixCooccurence(const Mat& image, double*** concurrenceArray,int graySize);
void extraitCaracteristicVector(double** mat, int size, ostream* ofs, vector<double>& vvalue) ;

double para_angular_second_moment(double** mat, int size);
double para_constrast(double** mat, int size);
double para_entropy(double** mat, int size);
double para_correlation(double** mat, int size);
double para_correlation(double** mat, int size, double meani, double meanj, double vari, double varj);
double para_meani(double** mat, int size);
double para_meanj(double** mat, int size);
double para_vari(double** mat, int size, double meani);
double para_varj(double** mat, int size, double meanj);
double para_dissimilarity(double** mat, int size);
double para_energy(double** mat, int size);
double para_homogenety(double** mat, int size);
double para_shade(double** mat, int size, int meani, int meanj);
double para_prominence(double** mat, int size, int meani, int meanj);
double para_sum_average(double** mat, int size);
double para_sum_variance(double** mat, int size, double sum_e);
double para_sum_entropy(double** mat, int size);
double para_difference_variance(double** mat, int size);
double para_difference_entropy(double** mat, int size);
double para_sum_of_squares__variance(double** mat, int size, double mean);
double para_mean(double** mat,int size);

void learning(const char* dirPath, const char* name,int graySize);
//void testing(const char* fileLearn, const char* fileTest, int k);
//void crossTesting(const char* filename, int percent,int k);
void printMatrix(double** dirPath, int size);
//double getTextureVectorDistance(const vector<double>& learningVector,const vector<double>& testingVector);
//void segmenter(const char* fileImage, const char* name, int graySize, int numberNeighbor, int segmblocksize);
//void segmenterCAH(const char* fileImage, const char* name, int graySize, int numberGroup, int segmblocksize);
void setZero(double*** concurrenceArray, int graySize);
void freeVector(vector<int>** vector, int size) ;
void freeMatrix(double** matrix, int size);
//double test_hung();
#endif	/* TEXTURE_H */


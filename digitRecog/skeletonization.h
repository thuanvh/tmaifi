/* 
 * File:   skeletonization.h
 * Author: thuanvh
 *
 * Created on October 4, 2010, 4:39 PM
 */

#ifndef SKELETONIZATION_H
#define	SKELETONIZATION_H


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

using namespace cv;

void Skeleton8bits(const Mat&  BitMap, Mat& outBitMap);

#define CV_SKEL_PAVLIDIS  1

//void cvRasterSkeleton( IplImage* image,int method CV_DEFAULT (CV_SKEL_PAVLIDIS));

void cvRasterSkeleton( Mat& image,int method =CV_SKEL_PAVLIDIS);
CvStatus icvPavlidis(uchar* img, int step, Size size);
void ThinImage(Mat& image);
#endif	/* SKELETONIZATION_H */


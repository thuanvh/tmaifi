/* 
 * File:   watershed.h
 * Author: thuan
 *
 * Created on May 13, 2010, 6:28 PM
 */

#ifndef _WATERSHED_H
#define	_WATERSHED_H

#include <iostream>
using namespace std;

void kmeans2(int argc, char** argv);
void on_mouse(int event, int x, int y, int flags, void* param);
void pyrmeansegmentation(IplImage* src, IplImage* dst);
void pyrsegmentation(IplImage* src, IplImage* dst);
void watershed(IplImage* filename);

int pyrdemo(IplImage* imageSource);
int pyrmeanshiftdemo(IplImage* imageSource);
string cvScalar2String(CvScalar value);
void meanshift(IplImage* image) ;
#endif	/* _WATERSHED_H */


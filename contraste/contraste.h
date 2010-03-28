/* 
 * File:   contraste.h
 * Author: thuan
 *
 * Created on 26 mars 2010, 17:32
 */
#include <opencv/highgui.h>

#ifndef _CONTRASTE_H
#define	_CONTRASTE_H
int* linealise(int* lut, CvPoint point1, CvPoint point2);
IplImage* linealise(IplImage* image, const CvPoint* pointArray, int length);
CvPoint* sortPoint(CvPoint* pointArray, int length);
IplImage* drawlut(int* lut, const CvPoint* pointArray, int length, CvScalar color=CV_RGB(250,123,123));
#endif	/* _CONTRASTE_H */


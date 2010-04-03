/* 
 * File:   histogramme.h
 * Author: thuan
 *
 * Created on 26 mars 2010, 17:31
 */
#include <opencv/highgui.h>

#ifndef _HISTOGRAMME_H
#define	_HISTOGRAMME_H
IplImage* imageHistogramme(IplImage* imageSource,CvScalar color=CV_RGB(123,123,123));
void drawCoordinate(IplImage* img, int deltaX, int deltaY, int width, int height, int maxY, CvScalar coordinateColor);
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY, int height);
const char* getFilePathName(const char* filepath, const char* newExtension);
const char* getFilePathName(char** filepath,int length);
#endif	/* _HISTOGRAMME_H */


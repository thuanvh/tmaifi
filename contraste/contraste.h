/* 
 * File:   contraste.h
 * Author: thuan
 *
 * Created on 26 mars 2010, 17:32
 */
#include <opencv/highgui.h>

#ifndef _CONTRASTE_H
#define	_CONTRASTE_H
/*
 * Linealiser des valeurs entre deux points
 */
uchar* linealise(uchar* lut, const CvPoint point1, const CvPoint point2);
/*
 * Faire linealisation avec une image par une serie de points
 */
IplImage* linealise(IplImage* image, CvPoint* pointArray, int length);
/*
 * ranger des points par l'ordre de x
 */
CvPoint* sortPoint(CvPoint* pointArray, int length);
/*
 * dessiner le diagramme de variable lut
 */
IplImage* drawlut(uchar* lut, const CvPoint* pointArray, int length, CvScalar color=CV_RGB(250,123,123));

#endif	/* _CONTRASTE_H */


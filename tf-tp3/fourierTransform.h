/* 
 * File:   fourierTransform.h
 * Author: thuan
 *
 * Created on April 17, 2010, 1:30 PM
 */

#ifndef _FOURIERTRANSFORM_H
#define	_FOURIERTRANSFORM_H
#include <opencv/cv.h>
#include <opencv/highgui.h>
IplImage** fourierTransform(IplImage* imgSource);

IplImage** fourierInverseTransform(IplImage* imgFourierTransform);
void absImageFourier(IplImage* realImage, IplImage* imgImage,IplImage* absImage);
void fourierVisibiliser(IplImage *reImg, IplImage* imImg, IplImage *dst);
IplImage *myDFT(IplImage *srcImg);
#endif	/* _FOURIERTRANSFORM_H */


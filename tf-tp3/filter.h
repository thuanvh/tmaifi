/* 
 * File:   filter.h
 * Author: thuan
 *
 * Created on April 17, 2010, 4:38 PM
 */

#ifndef _FILTER_H
#define	_FILTER_H

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#define COMMAND_FOURIER 0
#define COMMAND_FILTER 1
#define COMMAND_NOISE_FILTER 2

#define COMMAND_FILTER_LOW true
#define COMMAND_FILTER_HIGH false

IplImage* circleFourierImage(IplImage* image, float ratio, bool low = COMMAND_FILTER_LOW);
IplImage** filterImage(IplImage** imgSource, float ratio, bool low = COMMAND_FILTER_LOW);

#endif	/* _FILTER_H */


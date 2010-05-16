/* 
 * File:   segfile.h
 * Author: thuan
 *
 * Created on May 15, 2010, 12:15 PM
 */

#ifndef _SEGFILE_H
#define	_SEGFILE_H
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
using namespace std;

class SegImage {
public:
    int width;
    int height;
    int segments;
    bool gray;
    bool invert;
    bool flipflop;
    IplImage* image;
    SegImage();
};

SegImage* readSegFile(char* fileSeg);
string trim(string str);
#endif	/* _SEGFILE_H */


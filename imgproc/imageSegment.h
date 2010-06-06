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
class MatrixConfusion{
public:
    int height;
    int width;
    int** matrix;
    char* imageRef;
    char* imageSeg;
};
class SegImage {
public:
    char* filename;
    // information commun
    int width;
    int height;
    int segments;
    // information pour segfile
    bool gray;
    bool invert;
    bool flipflop;
    IplImage* image;
    SegImage();
    ~SegImage();
    MatrixConfusion* compareSegImage(SegImage* segImage);
    void colorerSegImage(IplImage* imageSource, IplImage* imageDest);
    void graySegImage(IplImage* imageDest);
};

void saveMatrixConfusion(char* file, MatrixConfusion* mc);
SegImage* readSegFile(char* fileSeg);
string trim(string str);
#endif	/* _SEGFILE_H */


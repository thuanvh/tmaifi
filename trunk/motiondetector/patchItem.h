/* 
 * File:   patchItem.h
 * Author: thuanvh
 *
 * Created on December 2, 2010, 3:03 PM
 */

#ifndef PATCHITEM_H
#define	PATCHITEM_H
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
using namespace cv;
class PatchItem {
public:
    int label;
//    Mat* image;
    int visited;
//    vector<double> caracteristic;
    int minX;
    int minY;
    int maxX;
    int maxY;
    //centre of mesure
    int centerX;
    int centerY;
    int vx;
    int vy;
    Scalar color;
    KalmanFilter* kalman;
    PatchItem();
    PatchItem(int x, int y, int vx, int vy);
    ~PatchItem();
    // predict
    int predictX;
    int predictY;
    int predictVx;
    int predictVy;
    
    // correct
    int correctX;
    int correctY;
    int correctVx;
    int correctVy;

    void predict();
    void correct();
    void initKalman();
};


#endif	/* PATCHITEM_H */


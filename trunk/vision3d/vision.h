#ifndef VISION_H
#define VISION_H

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

class SurfVision3d {
private:
    void findPairs(const vector<KeyPoint>& objectKeypoints, const vector<float>& objectDescriptors,
            const vector<KeyPoint>& imageKeypoints, const vector<float>& imageDescriptors, vector<int>& ptpairs, int size);
    int findMatchingOne(const KeyPoint& keypoint, const vector<float>& vec,
            const vector<KeyPoint>& model_keypoints,
            const vector<float>& model_descriptors, int start, int length,double& distance);
    double compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length);
    bool isNear(const KeyPoint& kp1, const KeyPoint& kp2);
    double drawPoint(const vector<KeyPoint>& objectKeypoints, Mat& image, Mat& imageDraw);
    double drawMatching2Image(const vector<int>& pointIndex,
            const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
            Mat& image1, Mat& image2, Mat& imageDraw);
    double drawMatchingDeltaImage(const vector<int>& pointIndex,
            const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
            Mat& image1, Mat& imageDraw);
    void calculateFondamentalMatrix(const vector<int>& pointIndex,
            const vector<KeyPoint>& objectKeypoints1, const vector<KeyPoint>& objectKeypoints2,
            const Mat& image1, const Mat& image2);
    void displayEpipoleLine(const Mat& fundMatr, const Mat& image, const Mat& points, int startImage, int numPoints, const Mat& pointsThisImage,const char* windowName);
public:
    int range;
    bool isQuiet;
    int point_count;
    void matching(char* image1, char* image2, char* outimage = NULL);
};

#endif
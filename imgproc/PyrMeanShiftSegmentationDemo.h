/* 
 * File:   PyrMeanShiftSegmentationDemo.h
 * Author: thuan
 *
 * Created on May 16, 2010, 10:15 PM
 */
#include "imageSegment.h"
#ifndef _PYRMEANSHIFTSEGMENTATIONDEMO_H
#define	_PYRMEANSHIFTSEGMENTATIONDEMO_H

class PyrMeanShiftSegmentationDemo {
private:
    IplImage *imageSegmentInput, *imageSegmentShow, *hybridImage,*imgref;
    CvSize size;

    int sp, sr;
    int level;


public:
    char* filepath;
    SegImage* segmentImageRef;
    SegImage segmentImage;
    int pyrmeanshiftdemo(IplImage* imageSource);
    PyrMeanShiftSegmentationDemo();
    void segment();
    static void ON_SEGMENT(int a);
    static PyrMeanShiftSegmentationDemo* currentDemo;
    void getImageSegment();
    void saveFile();
    void printInformation();
};

#endif	/* _PYRMEANSHIFTSEGMENTATIONDEMO_H */

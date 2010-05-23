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
    IplImage* image[2], *image0, *image1;
    CvSize size;

    int w0, h0, i;
    int sp, sr;
    int l, level;
    int ssp, ssr;
    int l_comp;
    int block_size;
    float parameter;
    double threshold;
    double rezult, min_rezult;
    CvFilter filter;
    CvConnectedComp *cur_comp, min_comp;
    CvSeq *comp;
    CvMemStorage *storage;

    CvPoint pt1, pt2;



public:
    int pyrmeanshiftdemo(IplImage* imageSource);
    PyrMeanShiftSegmentationDemo();
    void segment();
    static void ON_SEGMENT(int a);
    static PyrMeanShiftSegmentationDemo* currentDemo;
    SegImage getImageSegment();
};

#endif	/* _PYRMEANSHIFTSEGMENTATIONDEMO_H */

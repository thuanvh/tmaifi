/* 
 * File:   pyrSegmentationDemo.h
 * Author: thuan
 *
 * Created on May 17, 2010, 12:05 AM
 */

#ifndef _PYRSEGMENTATIONDEMO_H
#define	_PYRSEGMENTATIONDEMO_H

#include "imageSegment.h"

class PyrSegmentationDemo {
private:
    IplImage *imageSegmentInput, *imageSegmentShow, *hybridImage,*imgref;
    CvSize size;

    int w0, h0, i;
    int threshold1, threshold2;
    int l, level;
    int sthreshold1, sthreshold2;
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
    PyrSegmentationDemo();
    char* filepath;
    SegImage* segmentImageRef;
    SegImage segmentImage;

    void segment();
    int pyrdemo(IplImage* imageSource);
    void getImageSegment();
    void saveFile();
    void printInformation();
};
SegImage* pyrSegmentation(IplImage* imageSource, int threshold1 = 255, int threshold2 = 30);
#endif	/* _PYRSEGMENTATIONDEMO_H */


/* 
 * File:   pyrSegmentationDemo.h
 * Author: thuan
 *
 * Created on May 17, 2010, 12:05 AM
 */

#ifndef _PYRSEGMENTATIONDEMO_H
#define	_PYRSEGMENTATIONDEMO_H

class PyrSegmentationDemo {
private:
    IplImage* image[2], *image0, *image1;
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
    static PyrSegmentationDemo* currentDemo;
    static void ON_SEGMENT(int a);
    void segment();
    int pyrdemo(IplImage* imageSource);
};
#endif	/* _PYRSEGMENTATIONDEMO_H */


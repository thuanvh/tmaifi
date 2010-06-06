/* 
 * File:   watershedDemo.h
 * Author: thuan
 *
 * Created on May 20, 2010, 5:59 PM
 */

#ifndef _WATERSHEDDEMO_H
#define	_WATERSHEDDEMO_H

#include "imageSegment.h"


class WatershedDemo {
private:
    IplImage* marker_mask;
    IplImage* markers; // waters image
    IplImage* imgSource;//image source
    IplImage* img;
    IplImage* img_gray;
    IplImage* imgref;
    IplImage* wshed;//image output display
    IplImage* imgSegmentOuput;//image output display
    SegImage segmentImage;//segment image
    CvPoint prev_pt;
    int threshold;
    int localsize;
public:
    bool isManual;
    char* filepath;
    SegImage* segmentImageRef;
    static WatershedDemo* currentDemo;
    WatershedDemo();
    ~WatershedDemo();
    void on_mouse(int event, int x, int y, int flags, void* param);
    static void on_mouseDemo(int event, int x, int y, int flags, void* param);
    void demo(IplImage* imagesource);
    void printInformation();

    void watershedManual(IplImage* imagesource);
    void saveFile();
    void watershed(IplImage* imagesource);
    void manual(IplImage* imagesource);
    void autogaussian(IplImage* imagesource) ;
};

#endif	/* _WATERSHEDDEMO_H */


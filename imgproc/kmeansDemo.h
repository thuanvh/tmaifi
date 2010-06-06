/* 
 * File:   kmeans.h
 * Author: thuan
 *
 * Created on May 20, 2010, 12:09 AM
 */

#ifndef _KMEANS_H
#define	_KMEANS_H

class KmeansSegmentDemo {
private:
    int criteriaType;
    int maxIteration;
    int epsilon;
    int clusterCount;
    IplImage* imageSegmentShow;
    IplImage* hybridImage;
    IplImage* imgref;
public:
    KmeansSegmentDemo();
    void segmentation(IplImage* imageSource);
    void saveFile();
    void printInformation();
    char* filepath;
    SegImage* segmentImageRef;
    SegImage segmentImage;
};
SegImage* kmeansSegmentation(IplImage* imageSource, int clusterCount);
#endif	/* _KMEANS_H */


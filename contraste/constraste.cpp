
#include "contraste.h"
#include "histogramme.h"

int* linealise(int* lut, CvPoint point1, CvPoint point2){
    int x1,x2=0;
    if(point1.x < point2.x){
        x1=point1.x;
        x2=point2.x;
    }else{
        x1=point2.x;
        x2=point1.x;
    }
    for(int x=x1; x<x2; x++){
        int y=(point2.y - point1.y)(x-point1.x)/(point2.x-point1.x)+point1.y;
        lut[x]=y;
    }
    return lut;
}

IplImage* drawlut(int* lut, const CvPoint* pointArray, int length, CvScalar color){
    int imageWidth = 256;
    int imageHeight = 256;
    int deltaX = 50;
    int deltaY = 10;
    int maxNumberOfPoint=256;
    int i=0;
    // Creer une image de histogramme
    IplImage* histogrammeImg = cvCreateImage(cvSize(imageWidth + 2 * deltaX, imageHeight + 2 * deltaY), IPL_DEPTH_8U, 3);

    drawCoordinate(histogrammeImg, deltaX, deltaY, imageWidth, imageHeight, maxNumberOfPoint, CV_RGB(123, 0, 123));
    // Dessiner histogramme sur image
    double ratio = imageHeight / (double) maxNumberOfPoint;
    for (i = 0; i < 256; i++) {
        int y = (int) (ratio * lut[i]);
        cvLine(histogrammeImg, getCoordinate(i, 0, deltaX, deltaY, imageHeight),
                getCoordinate(i, y, deltaX, deltaY, imageHeight), color);
    }
    for (i=0 ; i<length-1; i++){
        cvLine(histogrammeImg, getCoordinate(pointArray[i].x, pointArray[i].y, deltaX, deltaY, imageHeight),
                getCoordinate(pointArray[i+1].x, pointArray[i+1].y, deltaX, deltaY, imageHeight), color);
    }
    return histogrammeImg;
}

IplImage* linealise(IplImage* image, const CvPoint* pointArray, int length){
    // trier les points
    pointArray = sortPoint(pointArray);
    // calculer lut
    uchar* lut=new uchar[256];
    for(int i=0; i<length-1; i++){
        lut=linealise(lut,pointArray[i],pointArray[i+1]);
    }
    // mettre a jour
    uchar* ptr=(uchar*)image->imageData;
    int size = image->imageSize;
    int i=0;
    while (i < size) {
        //std::cout<<*ptr<<"-"<<grayArray[*ptr]<<std::endl;
        //printf("%u - %u\n",*ptr,grayArray[*ptr]);
        *ptr=(uchar)lut[*ptr];
        i++;
        ptr++;
    }
    return image;
}
CvPoint* sortPoint(CvPoint* pointArray, int length){
    for(int i=0; i<length; i++){
        for(int j=i; j<length; j++){
            if(pointArray[i].x > pointArray[j].x){
                CvPoint temp=pointArray[i];
                pointArray[i]=pointArray[j];
                pointArray[j]=temp;
            }
        }
    }
    return pointArray;
}

#include "histogramme.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
/*
 * Generer une image histogramme a partir d'une image fournie
 */
IplImage* imageHistogramme(IplImage* imageSource,CvScalar color){
    uchar* ptr=NULL;
    ptr=(uchar*)imageSource->imageData;
    int size=imageSource->imageSize;
    int i=0;

    int* grayArray=new int[256];
    for(i=0; i<256; i++)
        grayArray[i]=0;

    while(i<size){
        //std::cout<<*ptr<<"-"<<grayArray[*ptr]<<std::endl;
        //printf("%u - %u\n",*ptr,grayArray[*ptr]);
        grayArray[*ptr]++;
        i++;
        ptr++;
    }

    int maxNumberOfPoint=0;// le nombre maximal de point dans histogramme
    for(i=0; i<256; i++){
        //printf("%u - %u : ",i,grayArray[i]);
        if(maxNumberOfPoint<grayArray[i])
            maxNumberOfPoint=grayArray[i];
    }

    int imageWidth=256;
    int imageHeight=200;
    int deltaX=50;
    int deltaY=10;
    // Creer une image de histogramme
    IplImage* histogrammeImg = cvCreateImage(cvSize(imageWidth+2*deltaX,imageHeight+2*deltaY),IPL_DEPTH_8U,3);

    drawCoordinate(histogrammeImg,deltaX,deltaY,imageWidth,imageHeight,maxNumberOfPoint,CV_RGB(123,0,123));
    // Dessiner histogramme sur image
    double ratio=imageHeight/(double)maxNumberOfPoint;
    for(i=0; i<256; i++){
        int y=(int)(ratio*grayArray[i]);
        cvLine(histogrammeImg,getCoordinate(i,0,deltaX,deltaY,imageHeight),
                getCoordinate(i, y,deltaX,deltaY,imageHeight),color);
    }
    return histogrammeImg;
}

/*
 * dessiner des coordinates de graphe
 */
void drawCoordinate(IplImage* img, int deltaX, int deltaY, int width, int height, int maxY, CvScalar coordinateColor) {
    // dessiner la ligne x
    cvLine(img, cvPoint(deltaX, deltaY + height), cvPoint(img->width - deltaX, deltaY + height), coordinateColor);
    // dessiner la ligne y
    cvLine(img, cvPoint(deltaX, deltaY + height), cvPoint(deltaX, deltaY), coordinateColor);
    // dessiner des repere sur la ligne y
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 0.5);
    char buffer [50];
    double ratio=height/(double)maxY;
    for (int i = 0; i <= 4; i ++) {
        int y=i*maxY/4;
        sprintf(buffer, "%d", y);
        cvPutText(img, buffer, cvPoint(0, deltaY + height - y*ratio), &font, coordinateColor);
    }

}

/*
 * calculer la valeur de point x,y sur la coordinate
 */
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY, int height) {
    return cvPoint(x + deltaX, deltaY + height - y);
}

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char* filepath, const char* newExtension) {
    std::string a(filepath);
    std::string b(newExtension);
    return (a + b).c_str();
}

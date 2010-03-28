/* 
 * File:   main.cpp
 * Author: thuan
 *
 * Created on 26 mars 2010, 17:28
 */

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "histogramme.h"
using namespace std;
/*
 * 
 */
int main(int argc, char** argv) {
    //char* filePath = "/home/thuan/sandbox/opencv/tp2/test/Fig0112(0)(small-gray-map-americas).tif";
    //char* filePath = "/home/thuan/sandbox/opencv/tp2/test/Fig0122(a)(fractal-iris).tif";
    //char* filePath = "/home/thuan/sandbox/opencv/tp2/test/Fig0102(1922 digital image).tif";
    //char* filePath = "/home/thuan/sandbox/opencv/tp2/test/Fig0121(a)(tungsten_flmt).tif";//Fig0121(a)(tungsten_flmt)
    try {
        char* filePath = NULL;
        bool isModeWindows = false;
        char* pointStrArray = NULL;
        // Analyser des parametres entres
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                i++;
                if (i < argc) {
                    pointStrArray = atoi(argv[i]);
                } else {
                    throw "Point is missing";
                }
                continue;
            }
            if (strcmp(argv[i], "-w") == 0) {
                isModeWindows = true;
            }
            if (i == argc - 1) {
                filePath = argv[i];
            }

        }


        if (filePath == NULL) {
            throw "File error";
        }
        if (pointStrArray == NULL) {
            throw "Point is missing";
        } else {

        }

        IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
        IplImage* histoImg = imageHistogramme(imgSource);

        // Ouvrir des fenetes
        if (isModeWindows) {
            cvShowImage("Output Source", imgSource);
            cvShowImage("Output Red", histoImg);

            cvWaitKey(0);
        }

        cvSaveImage(getFilePathName(filePath, ".histogramme.jpg"), histoImg);

        // vider la memoire
        cvReleaseImage(&imgSource);
        cvReleaseImage(&histoImg);

    } catch (const char* e) {
        // error
        std::cout << "Error:" << e << std::endl;
        std::cout << std::endl << "Using:" << std::endl;
        std::cout << argv[0] << " [-p <list of point>] [-w] imageFile" << std::endl;
    }
    return (EXIT_SUCCESS);
}

CvPoint* splitPointArray(const char* pointStrArray) {
    string strPoint(pointStrArray);
    int searchPoint=0;
    int pos=0;
    int pointIndex=0;
    int numberPoint = 0;
    for (int i = 0; i < strPoint.length; i++) {
        if (pointStrArray[i] == ';' && i < strPoint.length - 1)
            numberPoint++;
    }
    numberPoint++;
    CvPoint* pointArray = new CvPoint[numberPoint];
    while((pos=strPoint.find(';',searchPoint))>=0){
        string pointstr=strPoint.substr(searchPoint,pos-searchPoint);
        int posInPoint=pointstr.find(',',searchPoint);
        if(posInPoint>=0){
            string number1=strPoint.substr(searchPoint,posInPoint-searchPoint);
            string number2=strPoint.substr(posInPoint+1,pos-posInPoint-1);

            pointArray[pointIndex].x=atoi(number1.c_str());
            pointArray[pointIndex].y=atoi(number2.c_str());

        }else{
            throw "Error in point array format.";
        }
        searchPoint=pos+1;
        pointIndex++;
    }
    return pointArray;
//    int length = strlen(pointStrArray);
//
//    // nombre de point
//    int numberPoint = 0;
//    for (int i = 0; i < length; i++) {
//        if (pointStrArray[i] == ';' && i < length - 1)
//            numberPoint++;
//    }
//    numberPoint++;
//
//    int start = 0;
//    int end=0;
//    bool newPoint = true;
//    int pointIndex=0;
//    // split into array
//    CvPoint* pointArray = new CvPoint[numberPoint];
//    for (int i = 0; i < length; i++) {
//        if(newPoint)
//            pointArray[pointIndex]=cvPoint(0,0);
//        if (pointStrArray[i] == ';' && i < length - 1) {
//            strstr()
//            newPoint = true;
//            end=i;
//            start=i+1;
//            pointIndex++;
//        } else {
//            newPoint = false;
//        }
//    }
    return pointArray;
}

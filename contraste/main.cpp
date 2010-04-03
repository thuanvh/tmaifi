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
#include "contraste.h"
using namespace std;
CvPoint* splitPointArray(const char* pointStrArray, int& arrayLength);

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
        int arrlength = 0;

        // Analyser des parametres entres
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                i++;
                if (i < argc) {
                    pointStrArray = argv[i];
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
        // preparer des points trie 
        CvPoint* array = NULL;
        if (pointStrArray != NULL) {
            array = splitPointArray(pointStrArray, arrlength);
            array = sortPoint(array, arrlength);
//            for (int i = 0; i < arrlength; i++) {
//                std::cout << array[i].x << "-" << array[i].y << std::endl;
//            }
        }
        // ajouter deux point 0,0 et 255,255 a la liste de points
        CvPoint* pointArray = new CvPoint[arrlength + 2];
        pointArray[0] = cvPoint(0, 0);
        pointArray[arrlength + 1] = cvPoint(255, 255);
        for (int i = 1; i <= arrlength; i++) {
            pointArray[i] = array[i - 1];
        }
//        for (int i = 0; i < arrlength + 2; i++) {
//            std::cout << pointArray[i].x << "-" << pointArray[i].y << std::endl;
//        }

        IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
        IplImage* histoImg = imageHistogramme(imgSource);
        IplImage* imgModifie = cvCloneImage(imgSource);
        imgModifie = linealise(imgModifie, pointArray, arrlength + 2);
        IplImage* histoImgModifie = imageHistogramme(imgModifie);

        

        cvSaveImage(getFilePathName(filePath, ".histogramme.jpg"), histoImg);
        char* a[4]={filePath, ".modi.",pointStrArray,".jpg"};
        cvSaveImage(getFilePathName(a,4), imgModifie);
        char* b[4]={filePath, ".modihisto.",pointStrArray,".jpg"};
        cvSaveImage(getFilePathName(b,4), histoImgModifie);


        // Ouvrir des fenetes
        if (isModeWindows) {
            cvShowImage("Output Source", imgSource);
            cvShowImage("Output Histogramme Source", histoImg);
            cvShowImage("Output Modifie", imgModifie);
            cvShowImage("Output Histogramme Modifie", histoImgModifie);
            cvWaitKey(0);
        }

        // vider la memoire
        cvReleaseImage(&imgSource);
        cvReleaseImage(&histoImg);
        cvReleaseImage(&imgModifie);
        cvReleaseImage(&histoImgModifie);


    } catch (const char* e) {
        // error
        std::cout << "Error:" << e << std::endl;
        std::cout << std::endl << "Using:" << std::endl;
        std::cout << argv[0] << " [-p <list of point>] [-w] imageFile" << std::endl;
    }
    return (EXIT_SUCCESS);
}

CvPoint* splitPointArray(const char* pointStrArray, int& arrayLength) {
    string strPoint(pointStrArray);
    int searchPoint = 0;
    int pos = 0;
    int pointIndex = 0;
    int numberPoint = 0;
    for (int i = 0; i < strPoint.length(); i++) {
        if (pointStrArray[i] == ':' && i < strPoint.length() - 1)
            numberPoint++;
    }
    numberPoint++;
    arrayLength = numberPoint;
    CvPoint* pointArray = new CvPoint[numberPoint];
    while (pointIndex < numberPoint) {
        pos = strPoint.find(':', searchPoint);
        if (pos < 0)
            pos = strPoint.length();
        //std::cout<<strPoint<<"="<<pos<<":"<<searchPoint<<std::endl;
        string pointstr = strPoint.substr(searchPoint, pos - searchPoint);
        //std::cout<<pointstr<<str::end;
        int posInPoint = pointstr.find(',', 0);
        if (posInPoint >= 0) {
            string number1 = pointstr.substr(0, posInPoint);
            string number2 = pointstr.substr(posInPoint + 1);

            pointArray[pointIndex].x = atoi(number1.c_str());
            pointArray[pointIndex].y = atoi(number2.c_str());
            //std::cout<<searchPoint<<":"<<pos<<":"<<posInPoint<<" "<<pointArray[pointIndex].x<<","<<pointArray[pointIndex].y<<std::endl;
            if (pointArray[pointIndex].x > 255 || pointArray[pointIndex].x < 0 || pointArray[pointIndex].y > 255 || pointArray[pointIndex].y < 0)
                throw "Error: point input is invalid ";
        } else {
            throw "Error in point array format.";
        }
        searchPoint = pos + 1;
        pointIndex++;
    }
    return pointArray;

}

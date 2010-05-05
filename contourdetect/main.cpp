/* 
 * File:   main.cpp
 * Author: thuan
 *
 * Created on April 26, 2010, 5:06 PM
 */

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#define CONTOUR_SOBEL 1
#define CONTOUR_CANNY 2
#define CONTOUR_LAPLACE 4

#define STR_SOBEL "sobel"
#define STR_CANNY "canny"
#define STR_LAPLACE "laplace"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
  try {
    char* filePath = NULL;
    bool isModeWindows = false;
    char* pointStrArray = NULL;
    float ratio = 0.5;
    int threshold1 = 200;
    int threshold2 = 200;
    int typeOfAlgo = 0;

    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-r") == 0) {
        i++;
        if (i < argc) {
          ratio = atof(argv[i]);
        } else {
          throw "Ratio is missing";
        }
        continue;
      }
      if (strcmp(argv[i], "-t") == 0) {
        i++;
        if (i < argc) {
          threshold1 = atof(argv[i]);
        } else {
          throw "Threshold is missing";
        }
        i++;
        if (i < argc){
          if (argv[i][0] != '-') {
            threshold2 = atof(argv[i]);
          }else{
            i--;
          }
        }else
          i--;
        continue;
      }
      if (strcmp(argv[i], "-f") == 0) {
        //i++;
        while (++i < argc) {
          if (strcmp(argv[i], STR_SOBEL) == 0) {
            typeOfAlgo |= CONTOUR_SOBEL;
            continue;
          }
          if (strcmp(argv[i], STR_CANNY) == 0) {
            typeOfAlgo |= CONTOUR_CANNY;
            continue;
          }
          if (strcmp(argv[i], STR_LAPLACE) == 0) {
            typeOfAlgo |= CONTOUR_LAPLACE;
            continue;
          }
          i--;
          break;
        }
        continue;
      }
      if (strcmp(argv[i], "-w") == 0) {
        isModeWindows = true;
      }
      if (i >= argc - 1) {
        filePath = argv[i];
      }

    }


    if (filePath == NULL) {
      throw "File error";
    }
    //char* filePath = "Fig0516-c-BW_banreject_order4.tif"; //Fig0121(a)(tungsten_flmt)
    //bool isModeWindows = true;
    IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
    if (imgSource == NULL) {
      throw "Image File Error";
    }

    if (typeOfAlgo & CONTOUR_SOBEL) {
      IplImage* imgSobelX = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobelY = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobel = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobelDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      cvSobel(imgSource, imgSobelX, 0, 1);
      cvSobel(imgSource, imgSobelY, 1, 0);
      cvAbs(imgSobelX, imgSobelX);
      cvAbs(imgSobelY, imgSobelY);
      cvAdd(imgSobelX, imgSobelY, imgSobel);
      cvThreshold(imgSobel, imgSobel, threshold1, 255, CV_THRESH_BINARY);
      cvScale(imgSobel, imgSobelDisplay);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Sobel", imgSobelDisplay);
      char fileout [ FILENAME_MAX ];
      sprintf(fileout, "%s.sobel.%d.jpg", filePath, threshold1);
      cvSaveImage(fileout, imgSobelDisplay);
      cvReleaseImage(&imgSobel);
      cvReleaseImage(&imgSobelX);
      cvReleaseImage(&imgSobelY);
      cvReleaseImage(&imgSobelDisplay);

    }
    if (typeOfAlgo & CONTOUR_CANNY) {
      IplImage* imgCanny = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      cvCanny(imgSource, imgCanny, threshold1, threshold2);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Canny", imgCanny);
      char fileout [ FILENAME_MAX ];
      sprintf(fileout, "%s.canny.%d.%d.jpg", filePath, threshold1, threshold2);
      cvSaveImage(fileout, imgCanny);
      cvReleaseImage(&imgCanny);
    }
    if (typeOfAlgo & CONTOUR_LAPLACE) {
      IplImage* imgLaplace = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgLaplaceDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      cvLaplace(imgSource, imgLaplace);
      cvAbs(imgLaplace, imgLaplace);
      cvThreshold(imgLaplace, imgLaplace, threshold1, 255, CV_THRESH_BINARY);
      cvScale(imgLaplace, imgLaplaceDisplay);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Laplace", imgLaplaceDisplay);
      char fileout [ FILENAME_MAX ];
      sprintf(fileout, "%s.laplace.jpg", filePath);
      cvSaveImage(fileout, imgLaplaceDisplay);
      cvReleaseImage(&imgLaplace);
      cvReleaseImage(&imgLaplaceDisplay);
    }
    // Ouvrir des fenetes
    if (isModeWindows) {
      cvShowImage("Source", imgSource);
      cvWaitKey(0);
      cvReleaseImage(&imgSource);
    }
  } catch (const char* e) {
    // error
    std::cout << "Error:" << e << std::endl;
    std::cout << std::endl << "Using:" << std::endl;
    std::cout << argv[0] << "contourdetect [-w] [-f sobel canny laplace] [-t 200 250] image" << std::endl;
  }
  return (EXIT_SUCCESS);
}


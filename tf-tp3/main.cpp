/* 
 * File:   main.cpp
 * Author: thuan
 *
 * Created on April 12, 2010, 4:42 PM
 */

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "fourierTransform.h"
#include "utils.h"
#include "filter.h"

void performFourierTransform(IplImage* imgSource, char* filePath, bool isModeWindows);
void performFilter(IplImage* imgSource, char* filePath, bool isModeWindows, float ratio, bool low);
//void performFilter(IplImage* imgSource);
void performFilterSinusoidNoise(IplImage* imgSource,char* filePath, bool isModeWindows);
/*
 * 
 */
int main(int argc, char** argv) {
  try {
    char* filePath = NULL;
    bool isModeWindows = false;
    char* pointStrArray = NULL;
    float ratio = 0.5;
    int arrlength = 0;
    int typeOfTransform = COMMAND_FOURIER;
    bool filterLow = COMMAND_FILTER_LOW;
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
      if (strcmp(argv[i], "-f") == 0) {
        i++;
        if (i < argc) {
          typeOfTransform = COMMAND_FILTER;
          filterLow = (strcmp(argv[i], "low") == 0);
          if(strcmp(argv[i],"sin")==0){
            typeOfTransform = COMMAND_NOISE_FILTER;
          }
        } else {
          throw "command is missing";
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
    //char* filePath = "Fig0516-c-BW_banreject_order4.tif"; //Fig0121(a)(tungsten_flmt)
    //bool isModeWindows = true;
    IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
    if (imgSource == NULL) {
      throw "Image File Error";
    }
    switch (typeOfTransform) {
      case COMMAND_FOURIER:
        performFourierTransform(imgSource, filePath, isModeWindows);
        //performFilter(imgSource);
        break;
      case COMMAND_FILTER:

        performFilter(imgSource, filePath, isModeWindows, ratio, filterLow);
        break;
      case COMMAND_NOISE_FILTER:
        performFilterSinusoidNoise(imgSource, filePath, isModeWindows);
        break;

    }
    //IplImage** dft = fourierTransform(imgSource);
    //circleFourierImage(dft[0],0.5,true);
    //cvReleaseImage(&imgTemplateHigh);
    //cvReleaseImage(&imgTemplate);

    // return imgCircle;
    //cvWaitKey(0);
  } catch (const char* e) {
    // error
    std::cout << "Error:" << e << std::endl;
    std::cout << std::endl << "Using:" << std::endl;
    std::cout << argv[0] << " [-p <list of point>] [-w] imageFile" << std::endl;
  }
  return (EXIT_SUCCESS);
}

//void performFilter(IplImage* imgSource) {
//  IplImage* abc = myDFT(imgSource);
//  cvShowImage("Output Filter Source", abc);
//  cvWaitKey(0);
//  cvReleaseImage(&abc);
//}

void performFilter(IplImage* imgSource, char* filePath, bool isModeWindows, float ratio, bool low) {
  IplImage** dft = fourierTransform(imgSource);
  IplImage** idft = filterImage(dft, ratio, low);
//  cvSaveImage(getFilePathName(filePath, ".rl.jpg"), dft[0]);
//  cvSaveImage(getFilePathName(filePath, ".im.jpg"), dft[1]);
  cvSaveImage(getFilePathName(filePath, ".Fourier.jpg"), dft[3]);
//  cvSaveImage(getFilePathName(filePath, ".FitInvrl.jpg"), idft[0]);
//  cvSaveImage(getFilePathName(filePath, ".FitInvim.jpg"), idft[1]);
  cvSaveImage(getFilePathName(filePath, ".FitInvft.jpg"), idft[3]);
  cvSaveImage(getFilePathName(filePath, ".FitInvImage.jpg"), idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Filter Source", imgSource);
//    cvShowImage("Output Filter real image", dft[0]);
//    cvShowImage("Output Filter imagine image", dft[1]);
    cvShowImage("Output Filter fourier image", dft[3]);
//    cvShowImage("Output Filter invert real image", idft[0]);
//    cvShowImage("Output Filter invert imagine image", idft[1]);
    cvShowImage("Output Filter invert fourier image", idft[3]);
    cvShowImage("Output Filter invert fourier image source", idft[4]);
    cvWaitKey(0);
  }

  // vider la memoire
  cvReleaseImage(&imgSource);
  cvReleaseImage(&dft[0]);
  cvReleaseImage(&dft[1]);
  cvReleaseImage(&dft[2]);
  cvReleaseImage(&dft[3]);
  cvReleaseImage(&idft[0]);
  cvReleaseImage(&idft[1]);
  cvReleaseImage(&idft[2]);
  cvReleaseImage(&idft[3]);
  cvReleaseImage(&idft[4]);
  delete dft;
  delete idft;
}

void performFourierTransform(IplImage* imgSource, char* filePath, bool isModeWindows) {
  IplImage** dft = fourierTransform(imgSource);

  IplImage** idft = fourierInverseTransform(dft[2]);

//  cvSaveImage(getFilePathName(filePath, ".FtReal.jpg"), dft[0]);
//  cvSaveImage(getFilePathName(filePath, ".FtImag.jpg"), dft[1]);
  cvSaveImage(getFilePathName(filePath, ".Fourier.jpg"), dft[3]);
//  cvSaveImage(getFilePathName(filePath, ".InvReal.jpg"), idft[0]);
//  cvSaveImage(getFilePathName(filePath, ".InvImag.jpg"), idft[1]);
  cvSaveImage(getFilePathName(filePath, ".InvFourier.jpg"), idft[3]);
  cvSaveImage(getFilePathName(filePath, ".InvSrc.jpg"), idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Source", imgSource);
//    cvShowImage("Output real image", dft[0]);
//    cvShowImage("Output imagine image", dft[1]);
    cvShowImage("Output fourier image", dft[3]);
//    cvShowImage("Output invert real image", idft[0]);
//    cvShowImage("Output invert imagine image", idft[1]);
    cvShowImage("Output invert fourier image", idft[3]);
    cvShowImage("Output invert fourier image Ivert", idft[4]);
    cvWaitKey(0);
  }

  // vider la memoire
  cvReleaseImage(&imgSource);
  cvReleaseImage(&dft[0]);
  cvReleaseImage(&dft[1]);
  cvReleaseImage(&dft[2]);
  cvReleaseImage(&dft[3]);
  cvReleaseImage(&idft[0]);
  cvReleaseImage(&idft[1]);
  cvReleaseImage(&idft[2]);
  cvReleaseImage(&idft[3]);
  cvReleaseImage(&idft[4]);

  delete dft;
    delete idft;
}

void performFilterSinusoidNoise(IplImage* imgSource,char* filePath, bool isModeWindows){
  IplImage** dft = fourierTransform(imgSource);
  IplImage** idft = filterSinusoidNoise(dft);//filterImage(dft, ratio, low);
//  cvSaveImage(getFilePathName(filePath, ".rl.jpg"), dft[0]);
//  cvSaveImage(getFilePathName(filePath, ".im.jpg"), dft[1]);
  cvSaveImage(getFilePathName(filePath, ".Fourier.jpg"), dft[3]);
//  cvSaveImage(getFilePathName(filePath, ".FitInvrl.jpg"), idft[0]);
//  cvSaveImage(getFilePathName(filePath, ".FitInvim.jpg"), idft[1]);
  cvSaveImage(getFilePathName(filePath, ".FitInvft.jpg"), idft[3]);
  cvSaveImage(getFilePathName(filePath, ".FitInvImage.jpg"), idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Filter Source", imgSource);
//    cvShowImage("Output Filter real image", dft[0]);
//    cvShowImage("Output Filter imagine image", dft[1]);
    cvShowImage("Output Filter fourier image", dft[3]);
//    cvShowImage("Output Filter invert real image", idft[0]);
//    cvShowImage("Output Filter invert imagine image", idft[1]);
    cvShowImage("Output Filter invert fourier image", idft[3]);
    cvShowImage("Output Filter invert fourier image source", idft[4]);
    cvWaitKey(0);
  }

  // vider la memoire
  cvReleaseImage(&imgSource);
  cvReleaseImage(&dft[0]);
  cvReleaseImage(&dft[1]);
  cvReleaseImage(&dft[2]);
  cvReleaseImage(&dft[3]);
  cvReleaseImage(&idft[0]);
  cvReleaseImage(&idft[1]);
  cvReleaseImage(&idft[2]);
  cvReleaseImage(&idft[3]);
  cvReleaseImage(&idft[4]);
  delete dft;
  delete idft;
}
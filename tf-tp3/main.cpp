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
void performFilterSinusoidNoise(IplImage* imgSource, char* filePath, bool isModeWindows, int threshold = 200);

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
    int threshold = 200;
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
      if (strcmp(argv[i], "-t") == 0) {
        i++;
        if (i < argc) {
          threshold = atof(argv[i]);
        } else {
          throw "Threshold is missing";
        }
        continue;
      }
      if (strcmp(argv[i], "-f") == 0) {
        i++;
        if (i < argc) {
          typeOfTransform = COMMAND_FILTER;
          filterLow = (strcmp(argv[i], "low") == 0);
          if (strcmp(argv[i], "sin") == 0) {
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
        performFilterSinusoidNoise(imgSource, filePath, isModeWindows, threshold);
        break;

    }
  } catch (const char* e) {
    // error
    std::cout << "Error:" << e << std::endl;
    std::cout << std::endl << "Using:" << std::endl;
    std::cout << argv[0] << "tf [-f low | high | sin] [-r 'ratio'] [-w] 'fichier d'image'" << std::endl;
  }
  return (EXIT_SUCCESS);
}

void performFilter(IplImage* imgSource, char* filePath, bool isModeWindows, float ratio, bool low) {
  IplImage** dft = fourierTransform(imgSource);
  IplImage** idft = filterImage(dft, ratio, low);

  IplImage* img8u = convertToImage8U(dft[3]);

  string ratiostr;
  ratiostr += ratio;
  //  cout<<ratiostr<<ratio<<endl;
  char filef [ FILENAME_MAX ];
  char filename [ FILENAME_MAX ];
  char filename2 [ FILENAME_MAX ];
  sprintf(filef, "%s.Fourier.jpg", filePath);
  sprintf(filename, "%s.%s.%.3f.Fourier.jpg", filePath, low ? "low" : "high", ratio);
  sprintf(filename2, "%s.%s.%.3f.jpg", filePath, low ? "low" : "high", ratio);

  //  cout<<filename<<endl;
  cvSaveImage(filef, img8u);
  cvSaveImage(filename, idft[3]);
  cvSaveImage(filename2, idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Filter Source", imgSource);
    IplImage* a = convertToImage8U(dft[3]);
    cvShowImage("Output Filter fourier image", a);
    IplImage* b = convertToImage8U(idft[3]);
    cvShowImage("Output Filter invert fourier image", b);
    cvShowImage("Output Filter invert fourier image source", idft[4]);
    cvWaitKey(0);
        cvReleaseImage(&a);
    cvReleaseImage(&b);
  }

  cvReleaseImage(&img8u);
  cvReleaseImage(&imgSource);
  releaseMemory(dft, 4);
  releaseMemory(idft, 5);
}

void performFourierTransform(IplImage* imgSource, char* filePath, bool isModeWindows) {
  IplImage** dft = fourierTransform(imgSource);

  IplImage** idft = fourierInverseTransform(dft[2]);

  cvSaveImage(getFilePathName(filePath, ".Fourier.jpg"), dft[3]);
//  cvSaveImage(getFilePathName(filePath, ".InvFourier.jpg"), idft[3]);
  cvSaveImage(getFilePathName(filePath, ".Inverse.jpg"), idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Source", imgSource);
    IplImage* a = convertToImage8U(dft[3]);
    cvShowImage("Output fourier image", a);    
    cvShowImage("Output invert fourier image Ivert", idft[4]);
    cvWaitKey(0);
    cvReleaseImage(&a);
    
  }

  // vider la memoire
  cvReleaseImage(&imgSource);
  releaseMemory(dft, 4);
  releaseMemory(idft, 5);
}

void performFilterSinusoidNoise(IplImage* imgSource, char* filePath, bool isModeWindows, int threshold) {
  IplImage** dft = fourierTransform(imgSource);
  IplImage** idft = filterSinusoidNoise(dft, threshold);
  //  cout<<"hear"<<endl;
  char filef [ FILENAME_MAX ];
  char filename [ FILENAME_MAX ];
  char filename2 [ FILENAME_MAX ];
  sprintf(filef, "%s.Fourier.jpg", filePath);
  sprintf(filename, "%s.sin.%d.Fourier.jpg", filePath, threshold);
  sprintf(filename2, "%s.sin.%d.jpg", filePath, threshold);
  cvSaveImage(filef, dft[3]);
  cvSaveImage(filename, idft[3]);
  cvSaveImage(filename2, idft[4]);

  // Ouvrir des fenetes
  if (isModeWindows) {
    cvShowImage("Output Filter Source", imgSource);
    IplImage* a = convertToImage8U(dft[3]);
    cvShowImage("Output Filter fourier image", a);
    IplImage* b = convertToImage8U(idft[3]);
    cvShowImage("Output Filter invert fourier image", b);
    cvShowImage("Output Filter invert fourier image source", idft[4]);
    cvWaitKey(0);
    cvReleaseImage(&a);
    cvReleaseImage(&b);
  }

  // vider la memoire
  cvReleaseImage(&imgSource);
  releaseMemory(dft, 4);
  releaseMemory(idft, 5);

}

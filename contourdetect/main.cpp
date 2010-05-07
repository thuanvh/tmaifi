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

#include "contourFacteur.h"
#include "rothwell.h"

#define CONTOUR_SOBEL 1
#define CONTOUR_CANNY 2
#define CONTOUR_LAPLACE 4
#define CONTOUR_ROTHWELL 8

#define STR_SOBEL "sobel"
#define STR_CANNY "canny"
#define STR_LAPLACE "laplace"
#define STR_ROTHWELL "rothwell"
using namespace std;

void printFacteur(Facteur f) {
  printf("%d,%d,%d,%d,%d,%d,%f,%f,%f\n", f.countEdgeDetected, f.countEdgeRef, f.countCorrect,
          f.countFauxPositif, f.countFauxNegatif, f.countTotal, f.P, f.TFP, f.TFN);
}

/*
 * 
 */
int main(int argc, char** argv) {
  try {
    char* filePath = NULL;
    bool isModeWindows = false;
    char* pointStrArray = NULL;

    int threshold1 = 50;
    int threshold2 = 200;
    int typeOfAlgo = 0;
    int aperture_size = 3;
    float rw_sigma;
    float rw_alpha;
    char* refFilePath = NULL;
    bool saveFile = false;
    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {
      // sauver fichier ou non
      if (strcmp(argv[i], "-s") == 0) {
        saveFile = true;
        continue;
      }
      //aperture size
      if (strcmp(argv[i], "-apt-size") == 0) {
        i++;
        if (i < argc) {
          aperture_size = atoi(argv[i]);
        } else {
          throw "aperture_size is missing";
        }
        continue;
      }
      // file reference
      if (strcmp(argv[i], "-ref") == 0) {
        i++;
        if (i < argc) {
          refFilePath = argv[i];
        } else {
          throw "File ref is missing";
        }
        continue;
      }
      // seuil
      if (strcmp(argv[i], "-t") == 0) {
        // 1er seuil
        i++;
        if (i < argc) {
          threshold1 = atoi(argv[i]);
        } else {
          throw "Threshold is missing";
        }
        // 2e seuil
        i++;
        if (i < argc) {
          if (argv[i][0] != '-') {
            threshold2 = atoi(argv[i]);
          } else {
            i--;
          }
        } else
          i--;
        continue;
      }
      // type d'algo
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
          if (strcmp(argv[i], STR_ROTHWELL) == 0) {
            typeOfAlgo |= CONTOUR_ROTHWELL;
            continue;
          }
          i--;
          break;
        }
        continue;
      }
      // ouvrir les fenetres ou non
      if (strcmp(argv[i], "-w") == 0) {
        isModeWindows = true;
      }
      //RothWell Sigma
      if (strcmp(argv[i], "-rw-sigma") == 0) {
        i++;
        if (i < argc) {
          rw_sigma = atof(argv[i]);
        } else {
          throw "RothWell Sigma is missing";
        }
        continue;
      }
      //RothWell Alpha
      if (strcmp(argv[i], "-rw-alpha") == 0) {
        i++;
        if (i < argc) {
          rw_alpha = atof(argv[i]);
        } else {
          throw "RothWell Alpha is missing";
        }
        continue;
      }
      // fichier image
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
    } else if (imgSource->nChannels > 1) {
      // convert to gray
      IplImage* a = cvCreateImage(cvGetSize(imgSource), IPL_DEPTH_8U, 1);
      cvCvtColor(imgSource, a, CV_RGB2GRAY);
      cvReleaseImage(&imgSource);
      imgSource = a;
    }
    
    // load file reference
    IplImage* imgFileRef = NULL;
    if (refFilePath != NULL) {
      imgFileRef = cvLoadImage(refFilePath, CV_LOAD_IMAGE_UNCHANGED);
      cvXorS(imgFileRef, cvScalar(255), imgFileRef);
      if (imgFileRef == NULL) {
        throw "Image File Ref Error";
      }
    }

    if (typeOfAlgo & CONTOUR_SOBEL) {
      IplImage* imgSobelX = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobelY = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobel = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgSobelDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      //      cvSobel(imgSource, imgSobelX, 0, 1);
      //      cvSobel(imgSource, imgSobelY, 1, 0);
      //      cvAbs(imgSobelX, imgSobelX);
      //      cvAbs(imgSobelY, imgSobelY);
      //      cvAdd(imgSobelX, imgSobelY, imgSobel);
      //      cvThreshold(imgSobel, imgSobel, threshold1, 255, CV_THRESH_BINARY);
      //      cvScale(imgSobel, imgSobelDisplay);
      cvSobel(imgSource, imgSobel, 1, 1, aperture_size);
      cvAbs(imgSobel, imgSobel);
      cvThreshold(imgSobel, imgSobel, threshold1, 255, CV_THRESH_BINARY);
      cvConvert(imgSobel, imgSobelDisplay);
      //cvThreshold(imgSobelDisplay, imgSobelDisplay, threshold1, 255, CV_THRESH_BINARY);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Sobel", imgSobelDisplay);
      if (saveFile) {
        char fileout [ FILENAME_MAX ];
        sprintf(fileout, "%s.sobel.%d.jpg", filePath, threshold1);
        cvSaveImage(fileout, imgSobelDisplay);
      }

      if(imgFileRef!=NULL){
        Facteur f=getFacteur(imgSobelDisplay,imgFileRef);
        printFacteur(f);
      }
      cvReleaseImage(&imgSobel);
      cvReleaseImage(&imgSobelX);
      cvReleaseImage(&imgSobelY);
      cvReleaseImage(&imgSobelDisplay);

    }
    if (typeOfAlgo & CONTOUR_CANNY) {
      IplImage* imgCanny = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      cvCanny(imgSource, imgCanny, threshold1, threshold2, aperture_size);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Canny", imgCanny);
      if (saveFile) {
        char fileout [ FILENAME_MAX ];
        sprintf(fileout, "%s.canny.%d.%d.jpg", filePath, threshold1, threshold2);

        cvSaveImage(fileout, imgCanny);
      }
      if(imgFileRef!=NULL){
        Facteur f=getFacteur(imgCanny,imgFileRef);
        printFacteur(f);
      }
      cvReleaseImage(&imgCanny);
    }
    if (typeOfAlgo & CONTOUR_LAPLACE) {
      IplImage* imgLaplace = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgLaplaceDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      cvLaplace(imgSource, imgLaplace, aperture_size);
      //cvAbs(imgLaplace, imgLaplace);
      cvThreshold(imgLaplace, imgLaplace, threshold1, 255, CV_THRESH_BINARY);
      cvConvert(imgLaplace, imgLaplaceDisplay);
      //cvScale(imgLaplace, imgLaplaceDisplay);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Laplace", imgLaplaceDisplay);
      if (saveFile) {
        char fileout [ FILENAME_MAX ];
        sprintf(fileout, "%s.laplace.%d.jpg", filePath, threshold1);
        cvSaveImage(fileout, imgLaplaceDisplay);
      }
      if(imgFileRef!=NULL){
        Facteur f=getFacteur(imgLaplaceDisplay,imgFileRef);
        printFacteur(f);
      }
      cvReleaseImage(&imgLaplace);
      cvReleaseImage(&imgLaplaceDisplay);
    }
    if (typeOfAlgo & CONTOUR_ROTHWELL) {
      //IplImage* imgRothwell = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
      IplImage* imgRothwellDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
      rothwell(imgSource,imgRothwellDisplay,rw_sigma, threshold1, rw_alpha);
      //cvAbs(imgLaplace, imgLaplace);
      //cvThreshold(imgLaplace, imgLaplace, threshold1, 255, CV_THRESH_BINARY);
      //cvConvert(imgLaplace, imgLaplaceDisplay);
      //cvScale(imgLaplace, imgLaplaceDisplay);
      // Ouvrir des fenetes
      if (isModeWindows)
        cvShowImage("Output Rothwell", imgRothwellDisplay);
      if (saveFile) {
        char fileout [ FILENAME_MAX ];
        sprintf(fileout, "%s.rothwell.%f-%f.%d.jpg", filePath, rw_sigma, rw_alpha, threshold1);
        cvSaveImage(fileout, imgRothwellDisplay);
      }
      if(imgFileRef!=NULL){
        Facteur f=getFacteur(imgRothwellDisplay,imgFileRef);
        printFacteur(f);
      }
      //cvReleaseImage(&imgLaplace);
      cvReleaseImage(&imgRothwellDisplay);
    }
    // Ouvrir des fenetes
    if (isModeWindows) {
      cvShowImage("Source", imgSource);
      if(imgFileRef!=NULL)
        cvShowImage("Image Ref", imgFileRef);
      cvWaitKey(0);      
    }
    
    cvReleaseImage(&imgSource);
    if(imgFileRef!=NULL)
      cvReleaseImage(&imgFileRef);

  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << argv[0] << "contourdetect [-w] [-f sobel canny laplace] [-t 200 250] image" << std::endl;
  }
  return (EXIT_SUCCESS);
}


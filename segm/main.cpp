/* 
 * File:   main.cpp
 * Author: thuan
 *
 * Created on May 13, 2010, 1:24 AM
 */
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
#include "segmentAlgo.h"
#include "imageSegment.h"
#include "PyrMeanShiftSegmentationDemo.h"
#include "pyrSegmentationDemo.h"
#include "kmeansDemo.h"

#define CONTOUR_WATERSHED 1
#define CONTOUR_MEANSHIFT 2
#define CONTOUR_PYRSEGMENTATION 4
#define CONTOUR_PYRMEANSHIFTFILTERING 8
#define CONTOUR_KMEANS2 16

#define STR_WATERSHED "watershed"
#define STR_MEANSHIFT "meanshift"
#define STR_PYRSEGMENTATION "pyr"
#define STR_PYRMEANSHIFTFILTERING "pyrmeanshift"
#define STR_KMEANS2 "kmeans2"
using namespace std;

/*
void printFacteur(Facteur f) {
  printf("%d,%d,%d,%d,%d,%d,%f,%f,%f\n", f.countEdgeDetected, f.countEdgeRef, f.countCorrect,
          f.countFauxPositif, f.countFauxNegatif, f.countTotal, f.P, f.TFP, f.TFN);
}
*/

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
    float rw_sigma=0.3;
    float rw_alpha=0.5;
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
          throw "File seg ref is missing";
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
        if (i < argc-1) {
          cout<<i<<argv[i]<<endl;
          if (argv[i][0] != '-') {
            threshold2 = atoi(argv[i]);
          } else {
            //cout<<"seuil2";
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

          if (strcmp(argv[i], STR_WATERSHED) == 0) {
            typeOfAlgo |= CONTOUR_WATERSHED;
            continue;
          }
          if (strcmp(argv[i], STR_MEANSHIFT) == 0) {
            typeOfAlgo |= CONTOUR_MEANSHIFT;
            continue;
          }
          if (strcmp(argv[i], STR_PYRSEGMENTATION) == 0) {
            typeOfAlgo |= CONTOUR_PYRSEGMENTATION;
            continue;
          }
          if (strcmp(argv[i], STR_PYRMEANSHIFTFILTERING) == 0) {
            typeOfAlgo |= CONTOUR_PYRMEANSHIFTFILTERING;
            continue;
          }
          if (strcmp(argv[i], STR_KMEANS2) == 0) {
            typeOfAlgo |= CONTOUR_KMEANS2;
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
        //cout<<filePath;
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
//    else if (imgSource->nChannels > 1) {
//      // convert to gray
//      IplImage* a = cvCreateImage(cvGetSize(imgSource), IPL_DEPTH_8U, 1);
//      cvCvtColor(imgSource, a, CV_RGB2GRAY);
//      cvReleaseImage(&imgSource);
//      imgSource = a;
//    }

    // load file reference
    IplImage* imgFileRef = NULL;
    SegImage* segimage;
    if (refFilePath != NULL) {

      segimage=readSegFile(refFilePath);
      imgFileRef = segimage->image; //cvLoadImage(refFilePath, CV_LOAD_IMAGE_UNCHANGED);
      //cvXorS(imgFileRef, cvScalar(255), imgFileRef);
      if (imgFileRef == NULL) {
        throw "Image File Ref Error";
      }
    }


    if (typeOfAlgo & CONTOUR_WATERSHED) {
      watershed(imgSource);
    }
    if (typeOfAlgo & CONTOUR_MEANSHIFT) {
      //cvMeanShift()
      meanshift(imgSource);
    }
    if (typeOfAlgo & CONTOUR_PYRSEGMENTATION) {
      //cvPyrSegmentation()
      /*IplImage* pyrimageout = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, imgSource->nChannels);
      pyrsegmentation(imgSource,pyrimageout);
      if (isModeWindows)
        cvShowImage("Output Rothwell", pyrimageout);
      if (saveFile) {
        char fileout [ FILENAME_MAX ];
        sprintf(fileout, "%s.pyrmean.jpg", filePath);
        cvSaveImage(fileout, pyrimageout);
      }

      cvReleaseImage(&pyrimageout);
       */
      //pyrdemo(imgSource);
      PyrSegmentationDemo psd;
      psd.pyrdemo(imgSource);
    }
    if (typeOfAlgo & CONTOUR_PYRMEANSHIFTFILTERING) {
      //cvPyrMeanShiftFiltering()
//      IplImage* pyrimageout = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, imgSource->nChannels);
//      pyrmeansegmentation(imgSource,pyrimageout);
//      if (isModeWindows)
//        cvShowImage("Output Rothwell", pyrimageout);
//      if (saveFile) {
//        char fileout [ FILENAME_MAX ];
//        sprintf(fileout, "%s.pyrmean.jpg", filePath);
//        cvSaveImage(fileout, pyrimageout);
//      }
//
//      cvReleaseImage(&pyrimageout);
      PyrMeanShiftSegmentationDemo pmss;
      pmss.pyrmeanshiftdemo(imgSource);
    }
    if (typeOfAlgo & CONTOUR_KMEANS2) {
      //cvKMeans2()
      //kmeans2(argc,argv);
      KmeansSegmentDemo kmean;
      kmean.segmentation(imgSource);
    }
    /*
    if (typeOfAlgo & CONTOUR_) {
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
      
      //cvReleaseImage(&imgLaplace);
      
    }
     */
    // Ouvrir des fenetes
    if (isModeWindows) {
      cvShowImage("Source", imgSource);
      if(imgFileRef!=NULL){
        cvScale(imgFileRef,imgFileRef,255/segimage->segments);
        cvShowImage("Image Ref", imgFileRef);
      }
      cvWaitKey(0);
    }

    cvReleaseImage(&imgSource);
    if(imgFileRef!=NULL)
      cvReleaseImage(&imgFileRef);

  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << argv[0] << "seg [-w] [-s] [-f sobel canny laplace rothwell] [-t 200 250] image" << std::endl;
  }
  return (EXIT_SUCCESS);
}


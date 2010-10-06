#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "digitdetection.h"
#include "digitdetectionMahalanobis.h"
#include "digitdetectionpoint.h"
/*
 * File:   main.cpp
 * Author: thuan
 *
 * Created on May 13, 2010, 1:24 AM
 */

using namespace std;
using namespace cv;

/*
 *
 */
int main(int argc, char** argv) {
  try {
    char* testImagePath= NULL;
    char* histoDirPath=NULL;
    char* testLabelPath=NULL;
    char* trainingImagePath=NULL;
    char* trainingLabelPath=NULL;

    bool isModeWindows = false;

    int typeOfFunction = 0;
    int typeOfMethod = 0;


    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // directory reference
      if (strcmp(argv[i], "-learn-image") == 0) {
        i++;
        if (i < argc) {
          trainingImagePath = argv[i];
        } else {
          throw ERR_DIR_MISSING;
        }
        continue;
      }
      if (strcmp(argv[i], "-learn-label") == 0) {
        i++;
        if (i < argc) {
          trainingLabelPath = argv[i];
        } else {
          throw ERR_DIR_MISSING;
        }
        continue;
      }
      // load histogram
      if (strcmp(argv[i], "-histodir") == 0) {
        i++;
        if (i < argc) {
          histoDirPath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-test-image") == 0) {
        i++;
        if (i < argc) {
          testImagePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-test-label") == 0) {
        i++;
        if (i < argc) {
          testLabelPath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // type d'algo
      if (strcmp(argv[i], "-cmd") == 0) {

        if (++i < argc) {

          if (strcmp(argv[i], STR_LEARNING) == 0) {
            typeOfFunction = FUNC_LEARNING;
            continue;
          }
          if (strcmp(argv[i], STR_TEST) == 0) {
            typeOfFunction = FUNC_TEST;
            continue;
          }
        } else {
          throw ERR_FUNC_MISSING;
        }
        continue;
      }

      // type d'algo
      if (strcmp(argv[i], "-method") == 0) {

        if (++i < argc) {

          if (strcmp(argv[i], STR_CMD_MAHALANOBIS) == 0) {
            typeOfMethod = CMD_MAHALANOBIS;
            continue;
          }
          if (strcmp(argv[i], STR_CMD_BAYES_PIXEL) == 0) {
            typeOfMethod = CMD_BAYES_PIXEL;
            continue;
          }
          if (strcmp(argv[i], STR_CMD_BAYES_POINT) == 0) {
            typeOfMethod = CMD_BAYES_POINT;
            continue;
          }
        } else {
          throw ERR_FUNC_MISSING;
        }
        continue;
      }

    }

    if (typeOfFunction == FUNC_LEARNING) {
      if (trainingImagePath == NULL || trainingLabelPath==NULL) {
        throw ERR_DIR_MISSING;
      } else {
        if(typeOfMethod==CMD_BAYES_PIXEL)
          learningHisto(trainingImagePath,trainingLabelPath, histoDirPath);
        else if(typeOfMethod==CMD_MAHALANOBIS)
          learningMahalanobis(trainingImagePath,trainingLabelPath, histoDirPath);
        else if(typeOfMethod==CMD_BAYES_POINT)
          learningPoint(trainingImagePath,trainingLabelPath, histoDirPath);
      }
    } else if (typeOfFunction == FUNC_TEST) {
      if (testImagePath == NULL || testLabelPath==NULL) {
        throw ERR_FILE_MISSING;
      } else {
        if(typeOfMethod==CMD_BAYES_PIXEL)
          TestingHisto(histoDirPath,testImagePath, testLabelPath);
        else if(typeOfMethod==CMD_MAHALANOBIS)
          TestingMahalanobis(histoDirPath,testImagePath, testLabelPath);
        else if(typeOfMethod==CMD_BAYES_POINT)
          TestingPoint(histoDirPath,testImagePath, testLabelPath);
      }
    }


  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << argv[0] << " -learn-image 'fichier image d'apprentissage' -learn-label 'fichier label d'apprentissage' -cmd [learning | test] -histodir 'repertoire de sortie' -method bayes-point" << std::endl;
  }
  return (EXIT_SUCCESS);


//    QApplication a(argc, argv);
//    DigitDetectorWindow w;
//    w.show();
//
//    return a.exec();
}

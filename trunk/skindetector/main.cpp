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

#include "skindetector.h"

using namespace std;
using namespace cv;

/*
 *
 */
int main(int argc, char** argv) {
  try {
    char* filePath = NULL;
    bool isModeWindows = false;

    int typeOfFunction = 0;
    int aperture_size = 3;
    float rw_sigma = 0.3;
    float rw_alpha = 0.5;
    char* refFileDir = NULL;
    char* refFilePath = NULL;
    char* histoName = NULL;
    bool isManual = false;
    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // directory reference
      if (strcmp(argv[i], "-learn-dir") == 0) {
        i++;
        if (i < argc) {
          refFileDir = argv[i];
        } else {
          throw ERR_DIR_MISSING;
        }
        continue;
      }
      // load histogram
      if (strcmp(argv[i], "-histoname") == 0) {
        i++;
        if (i < argc) {
          histoName = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-testing-file") == 0) {
        i++;
        if (i < argc) {
          refFilePath = argv[i];
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

    }

    if (typeOfFunction == FUNC_LEARNING) {
      if (refFileDir == NULL) {
        throw ERR_DIR_MISSING;
      } else {
        Learning(refFileDir, histoName);
      }
    } else if (typeOfFunction == FUNC_TEST) {
      if (refFilePath == NULL) {
        throw ERR_FILE_MISSING;
      } else {
        Testing(refFilePath, histoName);
      }
    }


  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << argv[0] << " -f [kmeans | watershed | pyr | pyrmeanshift] [-ref file.seg] [–-manual] image" << std::endl;
  }
  return (EXIT_SUCCESS);
}

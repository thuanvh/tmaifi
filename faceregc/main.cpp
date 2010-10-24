
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "facerecognition.h"

using namespace std;
using namespace cv;

bool is_win_mode;
int num_color = 32;

/*
 *
 */
int main1(int argc, char** argv) {
  try {
    int typeOfFunction = 0;
    char* refFileDir = NULL;
    char* refFilePath = NULL;
    char* refOutFilePath = NULL;
    char* refRefFilePath = NULL;
    char* histoName = NULL;
    float threshold = 1;
    int crossTestPercent = 60;
    int numberNeighbor = 10;

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
      if (strcmp(argv[i], "-name") == 0) {
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
      // directory reference
      if (strcmp(argv[i], "-testing-out-file") == 0) {
        i++;
        if (i < argc) {
          refOutFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-testing-ref-file") == 0) {
        i++;
        if (i < argc) {
          refRefFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-threshold") == 0) {
        i++;
        if (i < argc) {
          threshold = atof(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-num-color") == 0) {
        i++;
        if (i < argc) {
          num_color = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-cross-test-percent") == 0) {
        i++;
        if (i < argc) {
          crossTestPercent = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-num-neighbor") == 0) {
        i++;
        if (i < argc) {
          numberNeighbor = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-win") == 0) {
        is_win_mode = true;
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
//      if (refFileDir == NULL) {
//        throw ERR_DIR_MISSING;
//      } else {
        learning(refFileDir, histoName);
//      }
    } else if (typeOfFunction == FUNC_TEST) {
//      if (refFilePath == NULL) {
//        throw ERR_FILE_MISSING;
//      } else {
        testing(refFileDir, histoName);
//      }
    }
  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << "texture -learn-dir « répertoire de l'apprentissage » -histoname « nom de histogramme » -testing-file « fichier de test » -testing-out-file « fichier sorti de test » -threshold « seuil » [–win] -cmd « learning | test » -num-color « nombre de couleur »" << std::endl;
  }
  return (EXIT_SUCCESS);
}

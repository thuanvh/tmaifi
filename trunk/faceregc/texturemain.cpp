
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "texture.h"

using namespace std;
using namespace cv;


int num_color = 32;
void printHelp();

/*
 *
 */
int main_texture(int argc, char** argv) {
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
    int segmblocksize = 32;
    int numberGroup=2;

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
      if (strcmp(argv[i], "-learning-file") == 0) {
        i++;
        if (i < argc) {
          refOutFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-segment-file") == 0) {
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
      if (strcmp(argv[i], "-num-group") == 0) {
        i++;
        if (i < argc) {
          numberGroup = atoi(argv[i]);
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
      if (strcmp(argv[i], "-segment-block-size") == 0) {
        i++;
        if (i < argc) {
          segmblocksize = atoi(argv[i]);
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
          if (strcmp(argv[i], STR_CROSS_TEST) == 0) {
            typeOfFunction = FUNC_CROSS_TEST;
            continue;
          }
          if (strcmp(argv[i], STR_SEGMENT) == 0) {
            typeOfFunction = FUNC_SEGMENT;
            continue;
          }
          if (strcmp(argv[i], STR_SEGMENT_CAH) == 0) {
            typeOfFunction = FUNC_SEGMENT_CAH;
            continue;
          }
        } else {
          throw ERR_FUNC_MISSING;
        }
        continue;
      }
      // type d'algo
      if (strcmp(argv[i], "--help") == 0) {
        printHelp();
        return 0;
      }

    }

    if (typeOfFunction == FUNC_LEARNING) {
      learning(refFileDir, histoName, num_color);

    }
    
  } catch (const char* e) {
    // error
    cerr << "Error:" << e << std::endl;
    printHelp();
  }
  return (EXIT_SUCCESS);
}

void printHelp() {

  cout << std::endl << "Using:" << std::endl;
  cout << "faceregc -learn-dir « répertoire de l'apprentissage » ";
  cout << "-name « nom de sample » ";
  cout << "-testing-file « fichier de test » ";
  cout << "-learning-file « fichier sorti de test » ";
  cout << "-num-color « niveau de gris » ";
  cout << "-num-group « nombre de groupe » ";
  cout << "-cmd « learn » ";
  cout << std::endl;
}

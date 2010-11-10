
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "motiondetector.h"

using namespace std;
using namespace cv;

bool is_win_mode;

void printHelp();

/*
 *
 */
int main(int argc, char** argv) {
  try {
    int typeOfFunction = 0;
    char* refFileDir = NULL;
    char* refFilePath = NULL;
    char* refOutFilePath = NULL;
    char* refRefFilePath = NULL;
    char* histoName = NULL;
    float threshold = 1;
    int queueSize = 5;
    int numberNeighbor = 10;
    int segmblocksize = 32;
    int numberGroup = 2;
    int fps = 10;
    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // directory reference
      if (strcmp(argv[i], "-out-dir") == 0) {
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
      if (strcmp(argv[i], "-video-file") == 0) {
        cout << "found" << endl;
        i++;
        if (i < argc) {
          refFilePath = argv[i];
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
      if (strcmp(argv[i], "-fps") == 0) {
        i++;
        if (i < argc) {
          fps = atoi(argv[i]);
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
      if (strcmp(argv[i], "-queue-size") == 0) {
        i++;
        if (i < argc) {
          queueSize = atoi(argv[i]);
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
      // directory reference
      if (strcmp(argv[i], "-win") == 0) {
        is_win_mode = true;
        continue;
      }

      // type d'algo
      if (strcmp(argv[i], "--help") == 0) {
        printHelp();
        return 0;
      }

    }
    cout << refFilePath << endl;
    MotionDetection(refFilePath, fps, queueSize);


  } catch (const char* e) {
    // error
    cerr << "Error:" << e << std::endl;
    printHelp();
  }
  return (EXIT_SUCCESS);
}

void printHelp() {

  cout << std::endl << "Using:" << std::endl;
  cout << "texture -learn-dir « répertoire de l'apprentissage » ";
  cout << "-name « nom de sample » ";
  cout << "-testing-file « fichier de test » ";
  cout << "-learning-file « fichier sorti de test » ";
  cout << "-segment-file « fichier de segmentation » ";
  cout << "-segment-block-size « size of segment block » ";
  cout << "-num-color « niveau de gris » ";
  cout << "-num-group « nombre de groupe » ";
  cout << "-cross-test-percent « percent » ";
  cout << "-num-neighbor « number of neighbor  » ";
  cout << "-cmd « learn | test | crosstest | segment» ";
  cout << std::endl;
}
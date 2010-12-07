
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
    int trackingType =0;
    int timetolive = 5;
    int queueSize = 5;
    int numberNeighbor = 10;
    int segmblocksize = 32;
    int rangeMatching = 30;
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
      if (strcmp(argv[i], "-tracking-type") == 0) {
        i++;
        if (i < argc) {
          if (strcmp(argv[i], STR_TRACKING_POSITION) == 0)
            trackingType = TRACKING_POSITION;
          else if (strcmp(argv[i], STR_TRACKING_POSITION_VELOCITY) == 0)
            trackingType = TRACKING_POSITION_VELOCITY;
          else
            throw ERR_FILE_MISSING;
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
      if (strcmp(argv[i], "-time-to-live") == 0) {
        i++;
        if (i < argc) {
          timetolive = atof(argv[i]);
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
      if (strcmp(argv[i], "-range-matching") == 0) {
        i++;
        if (i < argc) {
          rangeMatching = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-queue-bg-size") == 0) {
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
    MotionDetection(refFilePath, fps, queueSize, rangeMatching, refFileDir, trackingType,timetolive);


  } catch (const char* e) {
    // error
    cerr << "Error:" << e << std::endl;
    printHelp();
  }
  return (EXIT_SUCCESS);
}

void printHelp() {

  cout << std::endl << "Using:" << std::endl;
  cout << "motiondetector -video-file « chemin de video » ";
  cout << "-queue-bg-size « nombre de trame de plan arrière » ";
  cout << "-fps « la distance entre deux trames à manipuler » ";
  cout << "-time-to-live « temps de supprimer » ";
  cout << "-range-matching « la taille de fenêtre de chercher des objets cohérents » ";
  cout << "-out-dir « la répertoire de sortie » ";

//  cout << "-num-color « niveau de gris » ";
//  cout << "-num-group « nombre de groupe » ";
//  cout << "-cross-test-percent « percent » ";
//  cout << "-num-neighbor « number of neighbor  » ";
  cout << "-tracking-type « pos | posveloc » ";
  cout << std::endl;
}

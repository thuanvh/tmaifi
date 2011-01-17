
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#include "vision.h"

using namespace std;
using namespace cv;

bool is_win_mode;

void printHelp();

/*
 * 
 */
int main(int argc, char** argv) {
  try {
    int point_count = 20;
    bool isQuiet = false;
    char* outimg = NULL;
    char* img1 = NULL;
    char* img2 = NULL;
    int type = USE_SURF;
    int range = 20;

    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // directory reference
      if (strcmp(argv[i], "-img1") == 0) {
        i++;
        if (i < argc) {
          img1 = argv[i];
        } else {
          //          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-img2") == 0) {
        // 	cout << "found" << endl;
        i++;
        if (i < argc) {
          img2 = argv[i];
        } else {
          //          throw ERR_FILE_MISSING;
        }
        continue;
      }

      // directory reference
      if (strcmp(argv[i], "-out") == 0) {
        // 	cout << "found" << endl;
        i++;
        if (i < argc) {
          outimg = argv[i];
        } else {
          //          throw ERR_FILE_MISSING;
        }
        continue;
      }

      // directory reference
      if (strcmp(argv[i], "-q") == 0) {
        isQuiet = true;
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-point-count") == 0) {
        i++;
        if (i < argc) {
          point_count = atoi(argv[i]);
        } else {
          //                throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-range") == 0) {
        i++;
        if (i < argc) {
          range = atoi(argv[i]);
        } else {
          //          throw ERR_FILE_MISSING;
        }
        continue;
      }
      if (strcmp(argv[i], "-type") == 0) {
        i++;
        if (i < argc) {
          if (strcmp(argv[i], STR_SIFT) == 0) {
            type = USE_SIFT;
          } else if (strcmp(argv[i], STR_SURF) == 0) {
            type = USE_SURF;
          }
        } else {
          //          throw ERR_FILE_MISSING;
        }
        continue;
      }

      // type d'algo
      if (strcmp(argv[i], "--help") == 0) {
        printHelp();
        return 0;
      }

    }

    FeatureVision3d surfv3;
    surfv3.range = range;
    surfv3.isQuiet = isQuiet;
    surfv3.point_count = point_count;
    surfv3.method = type;
    surfv3.matching(img1, img2, outimg);


  } catch (const char* e) {
    // error
    cerr << "Error:" << e << std::endl;
    printHelp();
  }
  return (EXIT_SUCCESS);
}

void printHelp() {

  cout << std::endl << "Using:" << std::endl;
  cout << "vision3d [options]" << endl;
  cout << "-img1 « image gauche » " << endl;
  cout << "-img2 « image droite » " << endl;
  cout << "-type « sift | surf » " << endl;

  cout << "-out « nom des images de sortie » " << endl;
  cout << "-q « silent sans fenetre » " << endl;

  cout << "-range « rayon de recherche des points d'intere » " << endl;
  cout << "-point-count « nombre de points utilise pour calculer la matrice fondamentale » " << endl;

  
  cout << "--help \"print this help\" " << endl;
  cout << std::endl;
}

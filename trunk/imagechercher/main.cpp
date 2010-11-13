
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
    double colorWeight = 0.2;
    //    int crossTestPercent = 60;
    int numberReturn = 10;
    //    int segmblocksize = 32;
    //    int numberGroup = 2;
    int num_color = 32;
    int gris_texture = 24;

    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // directory reference
      if (strcmp(argv[i], "-dir") == 0) {
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
      if (strcmp(argv[i], "-test-file") == 0) {
        i++;
        if (i < argc) {
          refFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-att-file") == 0) {
        i++;
        if (i < argc) {
          refOutFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-ref-file") == 0) {
        i++;
        if (i < argc) {
          refRefFilePath = argv[i];
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // directory reference
      if (strcmp(argv[i], "-p") == 0) {
        i++;
        if (i < argc) {
          colorWeight = atof(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // histogram color size
      if (strcmp(argv[i], "-m") == 0) {
        i++;
        if (i < argc) {
          num_color = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }
      // histogram color size
      if (strcmp(argv[i], "-t") == 0) {
        i++;
        if (i < argc) {
          gris_texture = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }

      // nombre de valuer return


      if (strcmp(argv[i], "-n") == 0) {
        i++;
        if (i < argc) {
          numberReturn = atoi(argv[i]);
        } else {
          throw ERR_FILE_MISSING;
        }
        continue;
      }


      // type d'algo
      if (strcmp(argv[i], "-cmd") == 0) {

        if (++i < argc) {

          if (strcmp(argv[i], STR_EXTRACT) == 0) {
            typeOfFunction = FUNC_EXTRACT;
            continue;
          }
          if (strcmp(argv[i], STR_SEARCH) == 0) {
            typeOfFunction = FUNC_SEARCH;
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

    if (typeOfFunction == FUNC_EXTRACT) {
      extract(refFileDir, histoName, gris_texture, num_color);

    } else if (typeOfFunction == FUNC_SEARCH) {

      search(refOutFilePath, refFilePath, numberReturn, colorWeight, refRefFilePath);

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
  cout << "imagechercher -dir « répertoire de l'apprentissage » ";
  cout << "-name « nom de sample » ";
  cout << "-testing-file « fichier de test » ";
  cout << "-learning-file « fichier sorti de test » ";
  //  cout << "-segment-file « fichier de segmentation » ";
  //  cout << "-segment-block-size « size of segment block » ";
  cout << "-t « niveau de gris » ";
  cout << "-m « nombre de couleur » ";
  //  cout << "-cross-test-percent « percent » ";
  cout << "-n « number of neighbor  » ";
  cout << "-cmd « extract | search » ";
  cout << std::endl;
}

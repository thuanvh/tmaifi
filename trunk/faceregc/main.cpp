
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

int main_texture(int argc, char** argv);
int mainpca(int argc, char** argv);
//int num_color = 32;

/*
 *
 */
int main(int argc, char** argv) {
  try {
    
    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {

      // type d'algo
      if (strcmp(argv[i], "-maincmd") == 0) {

        if (++i < argc) {

          if (strcmp(argv[i], "texture") == 0) {
            main_texture(argc,argv);
            continue;
          }
          if (strcmp(argv[i], "pca") == 0) {
            mainpca(argc,argv);
            continue;
          }

        } else {
          throw ERR_FUNC_MISSING;
        }
        continue;
      }

    }

    
  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << "faceregc -learn-dir « répertoire de l'apprentissage » -histoname « nom de histogramme » -testing-file « fichier de test » -testing-out-file « fichier sorti de test » -threshold « seuil » [–win] -cmd « learning | test » -num-color « nombre de couleur »" << std::endl;
  }
  return (EXIT_SUCCESS);
}


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
    int typeOfFunction = 0;
    char* outDir = NULL;
    char* inDir = NULL;
    char* img1 = NULL;
    char* img2 = NULL;

    int cmd;
    int adapt_wsize;
    float adapt_tp;
    int px_seuil_delta;
    float px_seuil_px;
    double lk_seuil_h, lk_seuil_p;
    double histo_seuil;
    int ct_apertureSize, ct_canny_contourDistance, ct_canny_contourThreshold, ct_canny_threshold1, ct_canny_threshold2;

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

//      // directory reference
//      if (strcmp(argv[i], "-method") == 0) {
//        i++;
//        if (i < argc) {
//          //timetolive = atof(argv[i]);
//          if (strcmp(argv[i], STR_PIXEL) == 0) {
//            method = PixelToPixel;
//          } else if (strcmp(argv[i], STR_LIKELIHOOD) == 0) {
//            method = LikelihoodRatio;
//          } else if (strcmp(argv[i], STR_CONTOUR) == 0) {
//            method = Contour;
//          } else if (strcmp(argv[i], STR_HISTOGRAM) == 0) {
//            method = Histogram;
//          }
//        } else {
//          throw "method non specified";
//        }
//        continue;
//      }
//
//      // directory reference
//      if (strcmp(argv[i], "-px-seuil-delta") == 0) {
//        i++;
//        if (i < argc) {
//          px_seuil_delta = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-px-seuil-px") == 0) {
//        i++;
//        if (i < argc) {
//          px_seuil_px = atof(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-lk-seuil-h") == 0) {
//        i++;
//        if (i < argc) {
//          lk_seuil_h = atof(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-lk-seuil-p") == 0) {
//        i++;
//        if (i < argc) {
//          lk_seuil_p = atof(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-histo-seuil") == 0) {
//        i++;
//        if (i < argc) {
//          histo_seuil = atof(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-ct-contourDistance") == 0) {
//        i++;
//        if (i < argc) {
//          ct_canny_contourDistance = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-ct-contourThreshold") == 0) {
//        i++;
//        if (i < argc) {
//          ct_canny_contourThreshold = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-ct-canny_threshold1") == 0) {
//        i++;
//        if (i < argc) {
//          ct_canny_threshold1 = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-ct-apertureSize") == 0) {
//        i++;
//        if (i < argc) {
//          ct_apertureSize = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      // directory reference
//      if (strcmp(argv[i], "-ct-canny_threshold2") == 0) {
//        i++;
//        if (i < argc) {
//          ct_canny_threshold2 = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//
//      if (strcmp(argv[i], "-adapt-tp") == 0) {
//        i++;
//        if (i < argc) {
//          adapt_tp = atof(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//      if (strcmp(argv[i], "-adapt-wsize") == 0) {
//        i++;
//        if (i < argc) {
//          adapt_wsize = atoi(argv[i]);
//        } else {
//          throw ERR_FILE_MISSING;
//        }
//        continue;
//      }
//
//      // directory reference
//      if (strcmp(argv[i], "-cmd") == 0) {
//        i++;
//        if (i < argc) {
//          //timetolive = atof(argv[i]);
//          if (strcmp(argv[i], STR_CMD_TEST_ONE) == 0) {
//            cmd = CMD_TEST_ONE;
//          } else if (strcmp(argv[i], STR_CMD_TEST_ALL) == 0) {
//            cmd = CMD_TEST_ALL;
//          } else if (strcmp(argv[i], STR_CMD_TEST_ADAPT) == 0) {
//            cmd = CMD_TEST_ADAPT;
//          }
//        } else {
//          throw "method non specified";
//        }
//        continue;
//      }

      // type d'algo
      if (strcmp(argv[i], "--help") == 0) {
        printHelp();
        return 0;
      }

    }

    SurfVision3d surfv3;
    surfv3.range=20;
    surfv3.matching(img1,img2);
    // init algorithm
//    detectMethod* algo;
//    switch (method) {
//      case PixelToPixel:
//        algo = new pixelToPixelMethod();
//        ((pixelToPixelMethod*) algo)->seuil_delta = px_seuil_delta;
//        ((pixelToPixelMethod*) algo)->seuil_pixel = px_seuil_px;
//        break;
//      case LikelihoodRatio:
//        algo = new likelihoodRatioMethod();
//        ((likelihoodRatioMethod*) algo)->seuil_h = lk_seuil_h;
//        ((likelihoodRatioMethod*) algo)->seuil_p = lk_seuil_p;
//        break;
//      case Contour:
//        algo = new contourMethod();
//        ((contourMethod*) algo)->apertureSize = ct_apertureSize;
//        ((contourMethod*) algo)->canny_threshold1 = ct_canny_threshold1;
//        ((contourMethod*) algo)->canny_threshold2 = ct_canny_threshold2;
//        ((contourMethod*) algo)->contourDistance = ct_canny_contourDistance;
//        ((contourMethod*) algo)->contourThreshold = ct_canny_contourThreshold;
//        break;
//      case Histogram:
//        algo = new histogramMethod();
//        ((histogramMethod*) algo)->seuil_histo = histo_seuil;
//        break;
//    }
//    switch (cmd) {
//      case CMD_TEST_ONE:
//      {
//        plandetecter pdetecter;
//        pdetecter.method = (AlgoType) method;
//        pdetecter.algo = algo;
//
//        pdetecter.matrix = new matrixConfusion();
//        // 	cout<<"ok";
//
//        plandetectertest pdtone;
//        // 	cout<<inDir<<endl;
//        pdtone.attachPlanDetecter(inDir, pdetecter.matrix);
//        // 	pdetecter.matrix->print();
//        pdetecter.detectPlan(inDir);
//      }
//        break;
//      case CMD_TEST_ALL:
//      {
//        plandetectertest pdt;
//        pdt.testPlanDetecter(inDir, (AlgoType) method, algo);
//      }
//        break;
//      case CMD_TEST_ADAPT:
//      {
//        plandetectertest pdtadapt;
//
//        pdtadapt.testThresholdAdaptifAlgo(inDir, (AlgoType) method, algo, adapt_wsize, adapt_tp);
//      }
//        break;
//      default:
//        break;
//    }

  } catch (const char* e) {
    // error
    cerr << "Error:" << e << std::endl;
    printHelp();
  }
  return (EXIT_SUCCESS);
}

void printHelp() {

  cout << std::endl << "Using:" << std::endl;
  cout << "plandetecter [options]" << endl;
  cout << "-video-file « chemin de video » " << endl;
  cout << "-in-dir « la répertoire des fichiers d'image » " << endl;
  cout << "-method « pixel | contour | likelihood | histogram » " << endl;

  cout << "-px-seuil-delta « Seuil delta de la methode pixel » " << endl;
  cout << "-px-seuil-px « Seuil pixel de la methode pixel » " << endl;

  cout << "-lk-seuil-h « Seuil h de la methode likelihood » " << endl;
  cout << "-lk-seuil-p « Seuil p de la methode likelihood » " << endl;

  cout << "-histo-seuil « Seuil de la methode histograme » " << endl;

  cout << "-ct-contourDistance « La distance de voisin de la methode contour » " << endl;
  cout << "-ct-contourThreshold « Seuil de la methode Contour » " << endl;
  cout << "-ct-apertureSize « ApertureSize de Canny » " << endl;
  cout << "-ct-canny_threshold1 « Seuil 1 de Canny » " << endl;
  cout << "-ct-canny_threshold2 « Seuil 2 de Canny » " << endl;

  cout << "-adapt-wsize « Taille de fenêtre d'adaptive » " << endl;
  cout << "-adapt-tp « Valeur de coeff tp d'adaptive » " << endl;
  cout << "-cmd « testone | testall | testadaptive » " << endl;

  //  cout << "-num-color « niveau de gris » ";
  //  cout << "-num-group « nombre de groupe » ";
  //  cout << "-cross-test-percent « percent » ";
  //  cout << "-num-neighbor « number of neighbor  » ";
  cout << "--help \"print this help\" " << endl;
  cout << std::endl;
}

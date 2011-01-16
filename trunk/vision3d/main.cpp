
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

    int range=20;
    
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
      

      // type d'algo
      if (strcmp(argv[i], "--help") == 0) {
        printHelp();
        return 0;
      }

    }

    SurfVision3d surfv3;
    surfv3.range = range;
    surfv3.isQuiet = isQuiet;
    surfv3.point_count=point_count;
    surfv3.matching(img1, img2, outimg);
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

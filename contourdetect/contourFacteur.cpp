
#include "contourFacteur.h"
#include <opencv/cxcore.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

int getEdgeCount(IplImage* image) {
  int count = cvCountNonZero(image);
  return count;
}

void compaire(uchar* a, uchar* b, int& count) {
  if (*a != 0) {
    //cout<<" "<<(int)*a;
    if (*a == *b) {
      //std::cout<<"oki";
      count++;
      *a = 0;
      *b = 0;
    }
  }
}

Facteur getFacteur(IplImage* imageCalcule, IplImage* imageRef, int size) {
  IplImage* copyImageCalcule = cvCloneImage(imageCalcule);
  IplImage* copyImageRef = cvCloneImage(imageRef);

  uchar* ptr = (uchar*) copyImageCalcule->imageData;
  uchar* ptrRef = (uchar*) copyImageRef->imageData;


  const int w = copyImageCalcule->width;
  const int h = copyImageCalcule->height;

  int countCorrect = 0;

  int r = size / 2;
  for (int i = 0; i < h; i++) {// rowindex
    for (int j = 0; j < w; j++) {//colindex
      int delta = i * w + j;
      uchar* curptr = ptr + delta;
      if (*curptr == 0) continue;

      for (int k = -r; k <= r; k++) { // row index matrix
        if (i + k >= 0 && i + k < h) {
          for (int l = -r; l <= r; l++) { // col index matrix
            if (j + l >= 0 && j + l < w) {
//              if(i==0)
//                printf("%d,%d,%d,%d \n", i, j, k, l);
              compaire(curptr, ptrRef + delta + k * w + l, countCorrect);
            }
          }
        }
      }
    }
  }

  Facteur f;
  f.countCorrect = countCorrect;
  f.countFauxPositif = getEdgeCount(copyImageCalcule);
  f.countFauxNegatif = getEdgeCount(copyImageRef);
  f.countEdgeDetected = f.countCorrect + f.countFauxPositif;
  f.countEdgeRef = f.countCorrect + f.countFauxNegatif;
  f.countTotal = (countCorrect + f.countFauxNegatif + f.countFauxPositif);
  f.P = (double) countCorrect / f.countTotal;
  f.TFP = (double) f.countFauxPositif / f.countTotal;
  f.TFN = (double) f.countFauxNegatif / f.countTotal;

  cvReleaseImage(&copyImageCalcule);
  cvReleaseImage(&copyImageRef);
  return f;
}

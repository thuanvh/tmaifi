

#include <opencv/cxcore.h>

struct Facteur {
  int countEdgeDetected;
  int countEdgeRef;
  int countCorrect;
  int countFauxPositif;
  int countFauxNegatif;
  int countTotal;
  double P;
  double TFP;
  double TFN;
};

int getEdgeCount(IplImage* image) {
  int count = cvCountNonZero(image);
  return count;
}

void compaire(uchar* a, uchar* b, int& count) {
  if (a != 0)
    if (a == b) {
      count++;
      *a = 0;
      *b = 0;
    }
}

int getFacteur(IplImage* imageCalcule, IplImage* imageRef) {
  IplImage* copyImageCalcule = cvCloneImage(imageCalcule);
  IplImage* copyImageRef = cvCloneImage(imageRef);

  uchar* ptr = (uchar*) copyImageCalcule->imageData;
  uchar* ptrRef = (uchar*) copyImageRef->imageData;

  int i = 0;
  int w = copyImageCalcule->width;
  int h = copyImageCalcule->height;

  int countCorrect = 0;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      int delta = i * w + j;
      uchar* curptr = ptr + delta;
      if (*ptr == 0) continue;
      if (*ptr == *(ptrRef + delta)) {
        compaire(ptr, ptrRef + delta, &countCorrect);
      } else if (j > 0 && *ptr == *(ptrRef + delta - 1)) {
        compaire(ptr, ptrRef + delta - 1, &countCorrect);
      } else if (j < h - 1 && *ptr == *(ptrRef + delta + 1)) {
        compaire(ptr, ptrRef + delta + 1, &countCorrect);
      } else if (i > 0 && *ptr == *(ptrRef + delta - w)) {
        compaire(ptr, ptrRef + delta - w, &countCorrect);
      } else if (i > 0 && j > 0 && *ptr == *(ptrRef + delta - w - 1)) {
        compaire(ptr, ptrRef + delta - w - 1, &countCorrect);
      } else if (i > 0 && j < h - 1 && *ptr == *(ptrRef + delta - w + 1)) {
        compaire(ptr, ptrRef + delta - w + 1, &countCorrect);
      } else if (i < w - 1 && j > 0 && *ptr == *(ptrRef + delta + w - 1)) {
        compaire(ptr, ptrRef + delta + w - 1, &countCorrect);
      } else if (i < w - 1 && j < h - 1 && *ptr == *(ptrRef + delta + w + 1)) {
        compaire(ptr, ptrRef + delta + w + 1, &countCorrect);
      } else if (i < w - 1 && *ptr == *(ptrRef + delta + w)) {
        compaire(ptr, ptrRef + delta + w, &countCorrect);
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
  return f;
}

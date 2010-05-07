/* 
 * File:   contourFacteur.h
 * Author: thuan
 *
 * Created on May 5, 2010, 12:10 PM
 */

#ifndef _CONTOURFACTEUR_H
#define	_CONTOURFACTEUR_H
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
int getEdgeCount(IplImage* image) ;
void compaire(uchar* a, uchar* b, int& count);
Facteur getFacteur(IplImage* imageCalcule, IplImage* imageRef, int size=3);
#endif	/* _CONTOURFACTEUR_H */


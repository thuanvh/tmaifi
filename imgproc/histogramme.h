/* 
 * File:   histogramme.h
 * Author: thuan
 *
 * Created on 26 mars 2010, 17:31
 */
#include <opencv/highgui.h>

#ifndef _HISTOGRAMME_H
#define	_HISTOGRAMME_H
/*
 * Generer une histogramme
 */
int* getHistogramme(IplImage* imageSource);
/*
 * Generer une histogramme
 */
int** getHistogrammeColor(IplImage* imageSource);
/*
 * Generer une image histogramme a partir d'une image fournie
 */
IplImage* imageHistogramme(IplImage* imageSource,CvScalar color=CV_RGB(123,123,123));
/*
 * dessiner des coordinates de graphe
 */
void drawCoordinate(IplImage* img, int deltaX, int deltaY, int width, int height, int maxY, CvScalar coordinateColor);
/*
 * calculer la valeur de point x,y sur la coordinate
 */
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY, int height);
/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char* filepath, const char* newExtension);
/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(char** filepath,int length);
#endif	/* _HISTOGRAMME_H */


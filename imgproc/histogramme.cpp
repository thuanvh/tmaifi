#include "histogramme.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

int* getHistogramme(IplImage* imageSource) {
  uchar* ptr = NULL;
  ptr = (uchar*) imageSource->imageData;
  int size = imageSource->imageSize;
  int i = 0;

  int* grayArray = new int[256];
  for (i = 0; i < 256; i++)
    grayArray[i] = 0;

  while (i < size) {
    //std::cout<<*ptr<<"-"<<grayArray[*ptr]<<std::endl;
    //printf("%u - %u\n",*ptr,grayArray[*ptr]);
    grayArray[*ptr]++;
    i++;
    ptr++;
  }
  return grayArray;
}

/*
 * return rgb histogramme
 */
int** getHistogrammeColor(IplImage* imageSource) {
  //uchar* ptr = NULL;
  //ptr = (uchar*) imageSource->imageData;
  int size = imageSource->imageSize;
  int i = 0;

  int* redArray = new int[256];
  int* greenArray = new int[256];
  int* blueArray = new int[256];
  for (i = 0; i < 256; i++) {
    redArray[i] = 0;
    greenArray[i] = 0;
    blueArray[i] = 0;
  }
  uchar* ptr = (uchar*) imageSource->imageData;
  for (int i = 0; i < imageSource->height; i++) {
    for (int j = 0; j < imageSource->width; j++) {
      uchar* p = &CV_IMAGE_ELEM(imageSource, uchar, i, j * 3);
      redArray[p[0]]++;
      blueArray[p[1]]++;
      greenArray[p[2]]++;
      //CvPoint3D32f* pt = &((CvPoint3D32f*) points->data.fl)[i * imageSource->width + j];

      //      pt->x = p[0];
      //      pt->y = p[1];
      //      pt->z = p[2];

    }
  }
//  while (i < imageSource->imageSize) {
//    //std::cout<<*ptr<<"-"<<grayArray[*ptr]<<std::endl;
//    //printf("%u - %u\n",*ptr,grayArray[*ptr]);
//    //int loc = (*ptr)*3;
//    int r = *(ptr++);
//    int b = *(ptr++);
//    int g = *(ptr++);
//    redArray[r]++;
//    blueArray[b]++;
//    greenArray[g]++;
//    i++;
//    //ptr++;
//  }
  //  for (i = 0; i < 256; i++) {
  //    printf("%d %d %d-",redArray[i],greenArray[i],blueArray[i]);
  //  }
  int** hist = new int*[3];
  hist[0] = redArray;
  hist[1] = greenArray;
  hist[2] = blueArray;
  return hist;
}

/*
 * Generer une image histogramme a partir d'une image fournie
 */
IplImage* imageHistogramme(IplImage* imageSource, CvScalar color) {

  int* grayArray = getHistogramme(imageSource);
  int i = 0;
  int maxNumberOfPoint = 0; // le nombre maximal de point dans histogramme
  for (i = 0; i < 256; i++) {
    //printf("%u - %u : ",i,grayArray[i]);
    if (maxNumberOfPoint < grayArray[i])
      maxNumberOfPoint = grayArray[i];
  }

  int imageWidth = 256;
  int imageHeight = 200;
  int deltaX = 50;
  int deltaY = 30;
  // Creer une image de histogramme
  IplImage* histogrammeImg = cvCreateImage(cvSize(imageWidth + 2 * deltaX, imageHeight + 2 * deltaY), IPL_DEPTH_8U, 3);

  drawCoordinate(histogrammeImg, deltaX, deltaY, imageWidth, imageHeight, maxNumberOfPoint, CV_RGB(123, 0, 123));
  // Dessiner histogramme sur image
  double ratio = imageHeight / (double) maxNumberOfPoint;
  for (i = 0; i < 256; i++) {
    int y = (int) (ratio * grayArray[i]);
    cvLine(histogrammeImg, getCoordinate(i, 0, deltaX, deltaY, imageHeight),
            getCoordinate(i, y, deltaX, deltaY, imageHeight), color);
  }
  return histogrammeImg;
}

/*
 * dessiner des coordinates de graphe
 */
void drawCoordinate(IplImage* img, int deltaX, int deltaY, int width, int height, int maxY, CvScalar coordinateColor) {
  // dessiner la ligne x
  cvLine(img, cvPoint(deltaX, deltaY + height), cvPoint(img->width - deltaX, deltaY + height), coordinateColor);
  // dessiner la ligne y
  cvLine(img, cvPoint(deltaX, deltaY + height), cvPoint(deltaX, deltaY), coordinateColor);
  // dessiner des repere sur la ligne y
  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 0.5);
  char buffer [50];
  double ratio = height / (double) maxY;
  for (int i = 0; i <= 4; i++) {
    int y = i * maxY / 4;
    sprintf(buffer, "%d", y);
    cvPutText(img, buffer, cvPoint(0, deltaY + height - y * ratio), &font, coordinateColor);
    cvLine(img, cvPoint(deltaX, deltaY + height - y * ratio), cvPoint(deltaX - 5, deltaY + height - y * ratio), coordinateColor);
  }
  for (int i = 50; i < 255; i += 50) {
    int x = i;
    sprintf(buffer, "%d", x);
    cvPutText(img, buffer, cvPoint(x + deltaX, deltaY + height + 20), &font, coordinateColor);
    cvLine(img, cvPoint(x + deltaX, deltaY + height + 5), cvPoint(x + deltaX, deltaY + height), coordinateColor);
  }

}

/*
 * calculer la valeur de point x,y sur la coordinate
 */
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY, int height) {
  return cvPoint(x + deltaX, deltaY + height - y);
}

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char* filepath, const char* newExtension) {
  std::string a(filepath);
  std::string b(newExtension);
  // on n'obtient que quinze caracteres de le nom de fichier originaire
  //a=a.substr(0,15);
  return (a + b).c_str();
}

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(char** filepath, int length) {
  std::string a("");
  if (length > 0) {
    a += filepath[0];
    // on n'obtient que quinze caracteres de le nom de fichier originaire
    //a=a.substr(0,15);

  }
  for (int i = 1; i < length; i++) {
    a += filepath[i];
  }
  return a.c_str();
}

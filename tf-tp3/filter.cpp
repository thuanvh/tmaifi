#include "filter.h"
#include "fourierTransform.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

IplImage* circleFourierImage(IplImage* image, float ratio, bool low) {
  int w = image->width;
  int h = image->height;
  int rw = (int) w * ratio / 2;
  int rh = (int) h * ratio / 2;
  IplImage* imgTemplateHigh = cvCloneImage(image); //cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  CvScalar elipseColor = cvScalar(0);
  //cvCircle(imgTemplateHigh, cvPoint(rw,rh), 150, cvScalar(255), -1);
  cvEllipse(imgTemplateHigh, cvPoint(0, 0), cvSize(rw, rh), 0, 270, 360, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(w - 1, 0), cvSize(rw, rh), 0, 180, 270, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(0, h - 1), cvSize(rw, rh), 0, 0, 90, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(w - 1, h - 1), cvSize(rw, rh), 0, 90, 180, elipseColor, -1);

  IplImage* imgCircle = cvCloneImage(imgTemplateHigh);
  if (low)
    cvXor(image, imgTemplateHigh, imgCircle);

  cvReleaseImage(&imgTemplateHigh);

  return imgCircle;
}

IplImage** filterImage(IplImage** imgFouriers, float ratio, bool low) {

  // creer une image couper
  IplImage* fdftR = circleFourierImage(imgFouriers[0], ratio, low);
  IplImage* fdftI = circleFourierImage(imgFouriers[1], ratio, low);

  //  cvShowImage("Output Filter invert r source", fdftR);
  //  cvShowImage("Output Filter invert i high", fdftI);
  //  cvWaitKey(0);

  // fourier image
  IplImage* fftImg = cvCreateImage(cvSize(imgFouriers[0]->width, imgFouriers[0]->height), IPL_DEPTH_64F, 2);
  cvMerge(fdftR, fdftI, NULL, NULL, fftImg);

  // inverser l'image fourier
  IplImage** idft = fourierInverseTransform(fftImg);

  // rendre l'image visible
  fourierVisibiliser(fdftR, fdftI, idft[3]);

  cvReleaseImage(&fdftR);
  cvReleaseImage(&fdftI);
  cvReleaseImage(&fftImg);
  return idft;
}

IplImage** filterSinusoidNoise(IplImage** imgFouriers, int threshold) {
  IplImage* fdftR = cvCloneImage(imgFouriers[0]); //image reel
  IplImage* fdftI = cvCloneImage(imgFouriers[1]); //image imaginaire
  
  IplImage* ftimg8 = imgFouriers[3]; 

  int n = 3; // size of matrix
  int j = 1;
  //int s = n / 2;
  int w = ftimg8->width;
  int h = ftimg8->height;
  std::cout << ftimg8->width << "," << ftimg8->widthStep << "," << ftimg8->height << std::endl;
  uchar* ptr = (uchar*) ftimg8->imageData;

  int count = 0;
  // centre de l'image
  int cx = ftimg8->width / 2;
  int cy = ftimg8->height / 2;
  while (j <= ftimg8->height / 2) {
    int x = j * ftimg8->width;
    for (int i = 1; i < ftimg8->width - 1; i++) {
      //      bool find = false;
      x++;

      // trouver pics plus grand que ses voisins
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= threshold) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j, i - 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j - 1, i)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j - 1, i + 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j - 1, i - 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j, i + 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j + 1, i - 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j + 1, i + 1)) continue;
      if ((int) CV_IMAGE_ELEM(ftimg8, double, j, i) <= (int) CV_IMAGE_ELEM(ftimg8, double, j + 1, i)) continue;

      //(i,row)
      printf("%f %f %f \n%f %f %f\n%f %f %f\n",
              CV_IMAGE_ELEM(ftimg8, double, j - 1, i - 1), CV_IMAGE_ELEM(ftimg8, double, j - 1, i), CV_IMAGE_ELEM(ftimg8, double, j - 1, i + 1),
              CV_IMAGE_ELEM(ftimg8, double, j, i - 1), CV_IMAGE_ELEM(ftimg8, double, j, i), CV_IMAGE_ELEM(ftimg8, double, j, i + 1),
              CV_IMAGE_ELEM(ftimg8, double, j + 1, i - 1), CV_IMAGE_ELEM(ftimg8, double, j + 1, i), CV_IMAGE_ELEM(ftimg8, double, j + 1, i + 1));
      // sauf le centre de l'image
      if (i == cx && j == cy) continue;
      count++;

      // trouver le point correspondant dans l'image reel et l'image imaginaire
      int col = i, row = j;
      if (col < cx) {
        col = col + cx;
        col %= w;
        row = row + cy;
        row %= h;
      } else {
        col = col - cx;
        col %= w;
        row = row + cy;
        row %= h;
      }
      std::cout << i << "," << j << " - " << col << "," << row << std::endl;
      //cvValue(imgFouriers[0],cvScalar(0),)
      CV_IMAGE_ELEM(fdftR, double, row, col) = 0; //fdftR->imageData[row * w + col] = 0;
      CV_IMAGE_ELEM(fdftI, double, row, col) = 0; //fdftI->imageData[row * w + col] = 0;
     

      // trouver le point symmetric
      col = ftimg8->width - i;
      row = ftimg8->height - j;
      // le point de l'image reel et l'image imaginaire
      if (col < cx) {
        col = col - cx;
        col %= w;
        row = row - cy;
        row %= h;
      } else {
        col = col + cx;
        col %= w;
        row = row - cy;
        row %= h;
      }
      CV_IMAGE_ELEM(fdftR, double, row, col) = 0;
      CV_IMAGE_ELEM(fdftI, double, row, col) = 0;
      
    }
    j++;
  }
  std::cout << "count:" << count << std::endl;
  IplImage* fftImg = cvCreateImage(cvSize(imgFouriers[0]->width, imgFouriers[0]->height), IPL_DEPTH_64F, 2);
  //IplImage* fftImgHigh = cvCreateImage(cvSize(dft[0]->width, dft[0]->height), IPL_DEPTH_64F, 2);

  cvMerge(fdftR, fdftI, NULL, NULL, fftImg);
  //cvMerge(fdftR[1], fdftI[1], NULL, NULL, fftImgHigh);

  IplImage** idft = fourierInverseTransform(fftImg);

  fourierVisibiliser(fdftR, fdftI, idft[3]);
  
  cvReleaseImage(&fdftR);
  cvReleaseImage(&fdftI);
  return idft;
}

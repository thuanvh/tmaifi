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

  //IplImage* imgTemplate = cvCloneImage(imgTemplateHigh);
  IplImage* imgCircle = cvCloneImage(imgTemplateHigh);
  if (low)
    cvXor(image, imgTemplateHigh, imgCircle);

  //IplImage* imgCircle = cvCloneImage(image);
  //IplImage* imgHight=cvCloneImage(image);
  //cvAnd(image, imgTemplate, imgCircle);
  //cvAnd(imgHight, imgTemplateHigh, imgHight);

  //IplImage** imagesRet=new IplImage*[2];
  //imagesRet[0]=imgLow;
  //imagesRet[1]=imgHight;
//  cvShowImage("Output Filter invert source", image);
//  cvShowImage("Output Filter invert high", imgTemplateHigh);
//  cvShowImage("Output Filter invert tem", imgTemplate);
//  cvShowImage("Output Filter invert circle", imgCircle);
//  cvWaitKey(0);
  cvReleaseImage(&imgTemplateHigh);
//  cvReleaseImage(&imgTemplate);

  return imgCircle;
}

IplImage* Circle2(IplImage* image, float ratio, bool low) {
  //IplImage* image = image;
  int w = image->width;
  int h = image->height;
  int rw = (int) w*ratio;
  int rh = (int) h*ratio;
  IplImage* imgTemplateHigh = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
  CvScalar elipseColor = cvScalar(0);
  //filterLow=true;
  if (low) {
    std::cout << "abc";
    cvZero(imgTemplateHigh);
    elipseColor = cvScalar(0xffff);
    //cvNot(imgTemplate, imgTemplate);
    //cvNot(image,imgCircle);
    //cvAnd(image, imgTemplate, imgCircle);
  } else {
    //cvAnd(image, imgTemplate, imgCircle);
    std::cout << "abc";
    cvSet(imgTemplateHigh, cvScalar(0xffff));
    elipseColor = cvScalar(0x00);
  }

  //cvCircle(imgTemplateHigh, cvPoint(rw,rh), 150, cvScalar(255), -1);
  cvEllipse(imgTemplateHigh, cvPoint(0, 0), cvSize(rw, rh), 0, 270, 360, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(w - 1, 0), cvSize(rw, rh), 0, 180, 270, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(0, h - 1), cvSize(rw, rh), 0, 0, 90, elipseColor, -1);
  cvEllipse(imgTemplateHigh, cvPoint(w - 1, h - 1), cvSize(rw, rh), 0, 90, 180, elipseColor, -1);

  IplImage* imgTemplate = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);

  IplImage* imgCircle = cvCloneImage(image);

  cvConvert(imgTemplateHigh, imgTemplate);
  cvAnd(image, imgTemplate, imgCircle);

  //IplImage* imgHight=cvCloneImage(image);

  //cvAnd(imgHight, imgTemplateHigh, imgHight);

  //IplImage** imagesRet=new IplImage*[2];
  //imagesRet[0]=imgLow;
  //imagesRet[1]=imgHight;
  cvShowImage("Output Filter invert source", image);
  cvShowImage("Output Filter invert real high", imgTemplateHigh);
  cvShowImage("Output Filter invert real temp", imgTemplate);
  cvShowImage("Output Filter invert real Circle", imgCircle);
  cvWaitKey(0);
  return imgCircle;
}

IplImage** filterImage(IplImage** imgFouriers, float ratio, bool low) {
  //IplImage** dft = fourierTransform(imgSource);

  IplImage* fdftR = circleFourierImage(imgFouriers[0], ratio, low);
  IplImage* fdftI = circleFourierImage(imgFouriers[1], ratio, low);
  
  cvShowImage("Output Filter invert r source", fdftR);
  cvShowImage("Output Filter invert i high", fdftI);
  cvWaitKey(0);
  
  IplImage* fftImg = cvCreateImage(cvSize(imgFouriers[0]->width, imgFouriers[0]->height), IPL_DEPTH_64F, 2);
  //IplImage* fftImgHigh = cvCreateImage(cvSize(dft[0]->width, dft[0]->height), IPL_DEPTH_64F, 2);

  cvMerge(fdftR, fdftI, NULL, NULL, fftImg);
  //cvMerge(fdftR[1], fdftI[1], NULL, NULL, fftImgHigh);

  IplImage** idft = fourierInverseTransform(fftImg);

  fourierVisibiliser(fdftR, fdftI, idft[3]);
  return idft;
}

IplImage* filterSinusoidNoise(IplImage** imgFouriers, float ratio, bool low) {
  IplImage* imgF = imgFouriers[3];
  int n = 3; // size of matrix
  int row = 1;
  int s = n / 2;
  int w = imgF->width;
  while (row < imgF->height / 2) {
    int x = row * imgF->width;
    for (int i = 1; i < imgF->width - 1; i++) {
      bool find = false;
      x += i;
      if (imgF->imageData[x] <= imgF->imageData[x - s]) break;
      if (imgF->imageData[x] <= imgF->imageData[x - w]) break;
      if (imgF->imageData[x] <= imgF->imageData[x - w + 1]) break;
      if (imgF->imageData[x] <= imgF->imageData[x - w - 1]) break;
      if (imgF->imageData[x] <= imgF->imageData[x + s]) break;
      if (imgF->imageData[x] <= imgF->imageData[x + w - 1]) break;
      if (imgF->imageData[x] <= imgF->imageData[x + w + 1]) break;
      if (imgF->imageData[x] <= imgF->imageData[x + w]) break;
      //(i,row)
      int cx=imgF->width/2;
      int cy=imgF->height/2;
      int x,y,x1,y1;
      if(i<cx){
        x=i+cx;
        y=row+cy;
      }else{
        x=i-cx;
        y=y+cy;
      }
      //cvValue(imgFouriers[0],cvScalar(0),)
      imgF->imageData[x*w+y]=0;
    }
    row++;
  }
}

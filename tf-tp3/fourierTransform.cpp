#include "fourierTransform.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

IplImage** fourierTransform(IplImage* imgSource) {
  int w = cvGetOptimalDFTSize(imgSource->width);
  int h = cvGetOptimalDFTSize(imgSource->height);
  std::cout << w << " " << h << std::endl;
  if (w < h) w = h;
  else h = w;
  //for transforming input size formatsrc
  IplImage* src1 = cvCreateImage(cvSize(w, h), 8, 1);

  //resize input image as src1, with zero padding for extending size
  cvZero(src1);
  cvSetImageROI(src1, cvRect(0, 0, imgSource->width, imgSource->height));
  cvCopy(imgSource, src1);
  cvResetImageROI(src1);

  //for FFT - fourier transform
  IplImage* reImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  IplImage* imImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  IplImage* fftImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 2);
  //相应结果及显示 outputs for fourier and inverse fourier transform respectively
  IplImage* abs_fft = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);

  //input real part and imaginary part of the image
  cvConvertScale(src1, reImg, 1, 0);
  cvZero(imImg); //initializing zero for imaginary input

  cvMerge(reImg, imImg, NULL, NULL, fftImg);
  //Fourier transform
  cvDFT(fftImg, fftImg, CV_DXT_FORWARD/*|CV_DXT_SCALE*/);
  //divide fourier result into real and imginary part
  cvSplit(fftImg, reImg, imImg, NULL, NULL);

  fourierVisibiliser(reImg, imImg, abs_fft);

  IplImage** imagesReturn = new IplImage*[4];
  imagesReturn[0] = reImg;
  imagesReturn[1] = imImg;
  imagesReturn[2] = fftImg;
  imagesReturn[3] = abs_fft;
  return imagesReturn;
}

IplImage** fourierInverseTransform(IplImage* imgFourierTransform) {
  int w = imgFourierTransform->width;
  int h = imgFourierTransform->height;
  //inverse Fourier transform, intput format: IPL_DEPTH32F 2
  IplImage* ireImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  IplImage* iimImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  IplImage* ifftImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 2);
  IplImage* abs_ifft = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 1);
  //IplImage* fftImg = cvCreateImage(cvSize(w, h), IPL_DEPTH_64F, 2);
  //output image for inverse fourier transform IPL_DPETH_8U
  IplImage* inv_src = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
  //abs_ifft = absImageFourier(ireImg, iimImg);

  //inverse fourier transform
  cvDFT(imgFourierTransform, ifftImg, CV_DXT_INV_SCALE/*CV_DXT_INVERSE*/);
  cvSplit(ifftImg, ireImg, iimImg, NULL, NULL);

  //fourierVisibiliser(ireImg, iimImg, abs_ifft);
  absImageFourier(ireImg, iimImg, abs_ifft);
  cvConvertScale(abs_ifft, inv_src);

  IplImage** imagesReturn = new IplImage*[5];
  imagesReturn[0] = ireImg;
  imagesReturn[1] = iimImg;
  imagesReturn[2] = ifftImg;
  imagesReturn[3] = abs_ifft;
  imagesReturn[4] = inv_src;
  return imagesReturn;
}

void absImageFourier(IplImage* realImage, IplImage* imgImage, IplImage* absImage) {
  //  //abs_fft = sqrt(reIm^2 + imImg^2)
  IplImage* a = cvCloneImage(realImage);
  IplImage* b = cvCloneImage(imgImage);
  cvPow(realImage, a, 2); //reImg^2
  cvPow(imgImage, b, 2); //imImg^2
  cvAdd(a, b, a);
  cvPow(a, a, 0.5); //sqrt(abs_ifft)
  cvCopy(a,absImage);
  cvReleaseImage(&b);
  cvReleaseImage(&a);
}

void fourierVisibiliser(IplImage *reImg, IplImage* imImg, IplImage *dst) {
  IplImage* image_Re = cvCloneImage(reImg);
  IplImage* image_Im = cvCloneImage(imImg);
  int nRow, nCol, i, j, cy, cx;
  double scale, shift, tmp13, tmp24;


  // Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)
  cvPow(image_Re, image_Re, 2.0);
  cvPow(image_Im, image_Im, 2.0);
  cvAdd(image_Re, image_Im, image_Re);
  cvPow(image_Re, image_Re, 0.5);

  
  // Compute log(1 + Mag)
  cvAddS(image_Re, cvScalar(1.0), image_Re); // 1 + Mag
  cvLog(image_Re, image_Re); // log(1 + Mag)
  
  // Rearrange the quadrants of Fourier image so that the origin is at
  // the image center
  nRow = reImg->height;
  nCol = reImg->width;
  cy = nRow / 2; // image center
  cx = nCol / 2;
  for (j = 0; j < cy; j++) {
    for (i = 0; i < cx; i++) {
      tmp13 = CV_IMAGE_ELEM(image_Re, double, j, i);
      CV_IMAGE_ELEM(image_Re, double, j, i) = CV_IMAGE_ELEM(
              image_Re, double, j + cy, i + cx);
      CV_IMAGE_ELEM(image_Re, double, j + cy, i + cx) = tmp13;

      tmp24 = CV_IMAGE_ELEM(image_Re, double, j, i + cx);
      CV_IMAGE_ELEM(image_Re, double, j, i + cx) =
              CV_IMAGE_ELEM(image_Re, double, j + cy, i);
      CV_IMAGE_ELEM(image_Re, double, j + cy, i) = tmp24;
    }
  }

  double minVal = 0, maxVal = 0;
  // Localize minimum and maximum values
  cvMinMaxLoc(image_Re, &minVal, &maxVal);

  printf("\nmin = %g, max = %g\n", minVal, maxVal);

  //printf("\n Image size : %i x %i pixels\n", image_Re->width, image_Re->height);

  // Normalize image (0 - 255) to be observed as an u8 image
  scale = 255 / (maxVal - minVal);
  shift = -minVal * scale;
  cvConvertScale(image_Re, dst, scale, shift);
  
  //cvCopyImage(image_Re,dst);
  cvReleaseImage(&image_Re);
  cvReleaseImage(&image_Im);
}



#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char* filepath, const char* newExtension) {
  std::string a(filepath);
  std::string b(newExtension);
  // on n'obtient que quinze caracteres de le nom de fichier originaire
//  a=a.substr(0,15);
//  std::cout<<a+b<<std::endl;
  return (a + b).c_str();

}

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char** filepath, int length) {
  std::string a("");
  if (length > 0) {
    a += filepath[0];
    // on n'obtient que quinze caracteres de le nom de fichier originaire
//    a=a.substr(0,15);

  }
  for (int i = 1; i < length; i++) {
    a += filepath[i];
  }
//  std::cout<<a<<std::endl;
  return a.c_str();
}

void releaseMemory(IplImage** images,int length){
  for(int i=0; i<length; i++){
    cvReleaseImage(&(images[i]));
  }
  delete images;
}

IplImage* convertToImage8U(IplImage* image){
  //output image for inverse fourier transform IPL_DPETH_8U
  IplImage* inv_src = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
  cvConvertScale(image, inv_src);
  return inv_src;
}

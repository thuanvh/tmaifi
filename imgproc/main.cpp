/*
 * File:   main.cpp
 * Author: thuan
 *
 * Created on May 13, 2010, 1:24 AM
 */
/*
 * File:   main.cpp
 * Author: thuan
 *
 * Created on April 26, 2010, 5:06 PM
 */

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "segmentAlgo.h"
#include "imageSegment.h"
#include "PyrMeanShiftSegmentationDemo.h"
#include "pyrSegmentationDemo.h"
#include "kmeansDemo.h"
#include "watershedDemo.h"
#include "histogramme.h"
#include "contraste.h"

#define CONTOUR_WATERSHED 1
#define CONTOUR_MEANSHIFT 2
#define CONTOUR_PYRSEGMENTATION 4
#define CONTOUR_PYRMEANSHIFTFILTERING 8
#define CONTOUR_KMEANS2 16

#define STR_WATERSHED "watershed"
#define STR_MEANSHIFT "meanshift"
#define STR_PYRSEGMENTATION "pyr"
#define STR_PYRMEANSHIFTFILTERING "pyrmeanshift"
#define STR_KMEANS2 "kmeans"
using namespace std;

//int main(int argc, char** argv) {
//  vector<int> nums;
//  nums.push_back(-19);
//  nums.push_back(-1);
//  nums.push_back(0);
//  nums.push_back(5);
//  nums.push_back(8);
//  nums.push_back(8);
//  nums.push_back(11);
//
//  pair<vector<int>::iterator, vector<int>::iterator> result;
//  int new_val = 8;
//
//  result = equal_range(nums.begin(), nums.end(), new_val);
//
//  cout << "The first place that " << new_val << " could be inserted is before "
//          << *result.first << ", and the last place that it could be inserted is before "
//          << *result.second << endl;
//}

/*
 *
 */
int main(int argc, char** argv) {
  try {
    char* filePath = NULL;


    int typeOfAlgo = 0;

    char* refFilePath = NULL;

    bool openWindow = false;
    // Analyser des parametres entres
    for (int i = 0; i < argc; i++) {
      // open window
      if (strcmp(argv[i], "-w") == 0) {
        openWindow = true;
        continue;
      }
      // fichier image
      if (i >= argc - 1) {
        filePath = argv[i];
        //cout<<filePath;
      }
    }
    if (filePath == NULL) {
      throw "File error";
    }

    IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
    if (imgSource == NULL) {
      throw "Image File Error";
    }

    IplImage* imgOrigine = cvCloneImage(imgSource);
    //    cvReleaseImage(&imageNew);
    //    cvWaitKey(0);
    //    return 0;
    int delta = 3;
    int low = 2;
    uchar* p = (uchar*) imgSource->imageData;
    int red = *(p++);
    int blue = *(p++);
    int green = *(p++);
    int localsize = 3;
    //cvSmooth(imgSource, imgSource, CV_MEDIAN, localsize, localsize);
    //cvFloodFill(imgSource, cvPoint(0, 0), cvScalar(red + low, blue + low, green + low), cvScalar(delta + low, delta + low, delta + low), cvScalar(delta, delta, delta));
    cvFloodFill(imgSource, cvPoint(0, 0), cvScalar(0, 0, 0), cvScalar(delta + low, delta + low, delta + low), cvScalar(delta, delta, delta));
    //cvFloodFill(imgSource, cvPoint(0, 0), cvScalar(255-red, blue, 255-green), cvScalar(delta, delta, delta), cvScalar(delta, delta, delta));
    //cvFloodFill(imgSource, cvPoint(0, 0), cvScalar(0, 0, 0), cvScalar(delta, delta, delta), cvScalar(delta, delta, delta));
    cvShowImage("imageBg", imgSource);
    //cvWaitKey(0);
    //return 0;

    /*
        IplImage* imageGaussian = cvCloneImage(imgSource);
        //

        //cvSmooth(imgSource, imageGaussian, CV_MEDIAN, localsize, localsize);
        //    //cvReleaseImage(&imgSource);
        //    imgSource = imageGaussian;

        IplImage* imageNew = cvCloneImage(imageGaussian);
        imageNew = egaliserColor(imageGaussian);
        cvShowImage("egaliser", imageNew);
        //    imgSource = imageNew;

        SegImage* segImage = kmeansSegmentation(imageNew, 2);
        //SegImage* segImage = pyrSegmentation(imageNew, 10,10);
        IplImage* imageSegmentShow = cvCreateImage(cvGetSize(imgSource), 8, 3);
        IplImage* hybridImage = cvCreateImage(cvGetSize(imgSource), 8, 3);
        IplImage* img_gray = cvCreateImage(cvGetSize(imgSource), 8, 3);
        IplImage* imgGray = cvCreateImage(cvGetSize(imgSource), 8, 1);
        //IplImage* imgGray = cvCreateImage(cvGetSize(imgSource), 8, 1);
        //cvCvtColor(imgOrigine, imgGray, CV_RGB2GRAY); //(imgSource);
        cvCvtColor(imgSource, imgGray, CV_RGB2GRAY); //(imgSource);
        //egaliser(imgGray);
        cvShowImage("imgGray", imgGray);
        //    cvCvtColor(imgGray, img_gray, CV_GRAY2BGR);
        //

        segImage->colorerSegImage(imgOrigine, imageSegmentShow);
        cvAddWeighted(imageSegmentShow, 0.9, img_gray, 0.1, 0, hybridImage);
        cvShowImage("Kmeans Segmentation", hybridImage);
        /*
            // Contours
            int threshold1 = 100;
            int threshold2 = 50;
            int aperture_size = 3;
            IplImage* imgCanny = cvCloneImage(imgGray); //cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
            //IplImage* imgCanny = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
            cvCanny(imgGray, imgCanny, threshold1, threshold2, aperture_size);
            cvShowImage("image canny", imgCanny);

            //    IplImage* imgLaplace = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32F, 1);
            //    IplImage* imgLaplaceDisplay = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
            //    cvLaplace(imgGray, imgLaplace, aperture_size);
            //    //cvAbs(imgLaplace, imgLaplace);
            //    //cvThreshold(imgLaplace, imgLaplace, threshold1, 255, CV_THRESH_BINARY);
            //    cvConvert(imgLaplace, imgLaplaceDisplay);
            //    IplImage* imgCanny=imgLaplaceDisplay;

            //Draw contour
            CvMemStorage* storage = cvCreateMemStorage(0);
            CvSeq* contours = 0;
            CvMat* color_tab;
            int i, j, comp_count = 0;
            //cvSaveImage( "wshed_mask.png", marker_mask );
            //marker_mask = cvLoadImage( "wshed_mask.png", 0 );
            cvFindContours(imgCanny, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
            //cvFindContours(imgCanny, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
            //cvFindContours(imgCanny, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);
            //cvFindContours(imgCanny, storage, &contours, sizeof (CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);

            //IplImage* markers = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_8U, 1);
            IplImage* markers = cvCreateImage(cvSize(imgSource->width, imgSource->height), IPL_DEPTH_32S, 1);
            cvZero(markers);
            for (; contours != 0; contours = contours->h_next, comp_count++) {
              //cvDrawContours(markers, contours, cvScalarAll(255), cvScalarAll(255), 0, -1, 8, cvPoint(5, 5));
              cvDrawContours(markers, contours, cvScalarAll(comp_count + 1), cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
            }

            cvWatershed(imgSource, markers);

            IplImage* markersShow = cvCreateImage(cvGetSize(imgSource), IPL_DEPTH_8U, 1);

            cvZero(markersShow);
            cvScale(markers, markersShow);
            //cvShowImage("image markers", markersShow);
     */

    //Post segmentation
    //segImage->graySegImage(imgGray);
    IplImage* imgGray = cvCreateImage(cvGetSize(imgSource), 8, 1);
    //IplImage* imgGray = cvCreateImage(cvGetSize(imgSource), 8, 1);
    //cvCvtColor(imgOrigine, imgGray, CV_RGB2GRAY); //(imgSource);
    cvCvtColor(imgSource, imgGray, CV_RGB2GRAY); //(imgSource);
    SegImage* segImage = new SegImage();
    segImage->image = imgGray;
    uchar* p00 = &CV_IMAGE_ELEM(segImage->image, uchar, 0, 0);
    IplImage* imgMask = cvCloneImage(segImage->image);
    if (*p00 != 0) {
      cvThreshold(imgMask, imgMask, 0, 255, CV_THRESH_BINARY_INV);
    } else
      cvThreshold(imgMask, imgMask, 0, 255, CV_THRESH_BINARY);
    IplImage* imgTransform = cvCloneImage(imgMask);
    IplImage* imgTemp = cvCloneImage(imgMask);
    int erodesize = 3;
    int cvArrayClose[9] = {
      1, 0, 1,
      0, 1, 0,
      1, 0, 1
    };
    IplConvKernel* eleCross = cvCreateStructuringElementEx(erodesize, erodesize, 1, 1, CV_SHAPE_CROSS, cvArrayClose);
    IplConvKernel* eleRect = cvCreateStructuringElementEx(erodesize, erodesize, 1, 1, CV_SHAPE_RECT, cvArrayClose);
    IplConvKernel* eleEllipse = cvCreateStructuringElementEx(erodesize, erodesize, 1, 1, CV_SHAPE_ELLIPSE, cvArrayClose);
    IplConvKernel* eleCust = cvCreateStructuringElementEx(erodesize, erodesize, 1, 1, CV_SHAPE_CUSTOM, cvArrayClose);


    cvErode(imgTransform, imgTransform, eleCross, 2);
    cvDilate(imgMask, imgTransform, eleCross, 5);
    cvErode(imgTransform, imgTransform, eleCross, 7);
    //cvErode(imgTransform, imgTransform, eleEllipse, 2);
    //cvMorphologyEx(imgTransform, imgTransform, imgTemp, eleCross, CV_MOP_OPEN);
    //cvDilate(imgTransform, imgTransform, ele, 1);
    //cvMorphologyEx(imgTransform,imgTransform,imgTemp,0,CV_MOP_CLOSE);
    //cvMorphologyEx(imgSource,imgErode,imgTemp,0,CV_MOP_GRADIENT);
    //cvMorphologyEx(imgSource,imgErode,imgTemp,0,CV_MOP_TOPHAT);
    //cvMorphologyEx(imgSource,imgErode,imgTemp,0,CV_MOP_BLACKHAT);
    cvShowImage("erode", imgTransform);

    IplImage* imgResult = cvCloneImage(imgOrigine);
    IplImage* imgMaskResult = cvCloneImage(imgOrigine);
    cvCvtColor(imgTransform, imgMaskResult, CV_GRAY2BGR);
    cvAnd(imgOrigine, imgMaskResult, imgResult);
    cvShowImage("result image", imgResult);

    cvWaitKey(0);
    cvReleaseImage(&imgSource);

  } catch (const char* e) {
    // error
    std::cerr << "Error:" << e << std::endl;
    std::cerr << std::endl << "Using:" << std::endl;
    std::cerr << argv[0] << " [-w] image" << std::endl;
  }
  return (EXIT_SUCCESS);
}


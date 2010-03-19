/* 
 * File:   main.cpp
 * Author: thuan
 *
 * Created on 15 mars 2010, 00:03
 */

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <stdio.h>

/*
 * 
 */
int main(int argc, char** argv) {
    try {
        // Des parametres simples
        int lineIndex = -1; //ligne horizental du fichier
        int graphSize = 300; // la hauteur de image de niveau de gris, de rouge,
        bool isColor = false;
        char* filePath = NULL;
        bool isModeWindows = false;
        CvScalar colorOfLine = CV_RGB(122, 122, 122);

        // Des images de rapport de sortie



        // Analyser des parametres entres
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-l") == 0) {
                i++;
                if (i < argc) {
                    lineIndex = atoi(argv[i]);
                } else {
                    throw "Line index is missing";
                }
                continue;
            }
            if (strcmp(argv[i], "-w") == 0) {
                isModeWindows = true;
            }
            if (i == argc - 1) {
                filePath = argv[i];
            }
        }


        IplImage* imgSource = cvLoadImage(filePath, CV_LOAD_IMAGE_UNCHANGED);
        if (!imgSource) {
            std::cout << "Image Error" << filePath << std::endl;
            //throw "Image Error:";
        }
        if (lineIndex == -1) {
            lineIndex = imgSource->height / 2;
        }
        //std::cout << "abc";
        //IplImage* img = cvLoadImage("/home/thuan/Pictures/astroboy1.jpg");
        //IplImage* img = cvLoadImage("/home/thuan/Pictures/echec_scolaire.jpg");
        //IplImage* img = cvLoadImage("/home/thuan/Pictures/test.png", 1);
        //Fig0236-a-letter_T.tif
        //IplImage* img = cvLoadImage("/media/DATA/thuan/learn/informatique/traitementdimage/tp/data/images-Berkeley/22090.jpg");
        //IplImage* img = cvLoadImage("/media/DATA/thuan/learn/informatique/traitementdimage/tp/data/images-Berkeley/169012.jpg");
        if (isModeWindows)
            cvNamedWindow("Windows", CV_WINDOW_AUTOSIZE);

        // Verifier que c'est une image colore
        isColor = (imgSource -> nChannels == 3);
        if (isColor) {
            //CvSize size = new CvSize(300,img->width);
            //IplImage* imgGraph = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);

            IplImage* imgRed = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);
            IplImage* imgBlu = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);
            IplImage* imgGreen = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);
            IplImage* imgRGB = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);

            CvScalar s;
            uchar r, g, b;
            uchar r1, g1, b1;
            //std::cout << img->width << " " << img->widthStep << std::endl;
            uchar* ptr = (uchar*) (imgSource->imageData + lineIndex * imgSource->widthStep);
            if (ptr != 0) {
                b1 = *(ptr);
                g1 = *(ptr++);
                r1 = *(ptr++);
            }

            for (int i = 1; i < imgSource->width; i++) {

                b = *(ptr++);
                g = *(ptr++);
                r = *(ptr++);

                // image RGB
                cvLine(imgRGB, cvPoint(i - 1, b1), cvPoint(i, b), CV_RGB(0, 0, 255), 1, 8);
                cvLine(imgRGB, cvPoint(i - 1, g1), cvPoint(i, g), CV_RGB(0, 255, 0), 1, 8);
                cvLine(imgRGB, cvPoint(i - 1, r1), cvPoint(i, r), CV_RGB(255, 0, 0), 1, 8);
                // image Red
                cvLine(imgRed, cvPoint(i - 1, r1), cvPoint(i, r), CV_RGB(255, 0, 0), 1, 8);
                // image Green
                cvLine(imgGreen, cvPoint(i - 1, g1), cvPoint(i, g), CV_RGB(0, 255, 0), 1, 8);
                // image Blu
                cvLine(imgBlu, cvPoint(i - 1, b1), cvPoint(i, b), CV_RGB(0, 0, 255), 1, 8);

                b1 = b;
                g1 = g;
                r1 = r;
            }
            cvLine(imgSource, cvPoint(0, lineIndex), cvPoint(imgSource->width - 1, lineIndex), colorOfLine);
            if (isModeWindows) {
                cvShowImage("Output Source", imgSource);
                cvShowImage("Output Red", imgRed);
                cvShowImage("Output Blu", imgBlu);
                cvShowImage("Output Green", imgGreen);
                cvShowImage("Output RGB", imgRGB);

            }
            cvWaitKey(0);
            cvReleaseImage(&imgSource);
            cvReleaseImage(&imgRed);
            cvReleaseImage(&imgBlu);
            cvReleaseImage(&imgGreen);
            cvReleaseImage(&imgRGB);


        } else {
            //CvSize size = new CvSize(300,img->width);
            //IplImage* imgGraph = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);
            IplImage* imgGray = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);


            uchar g, g1;
            //std::cout << img->width << " " << img->widthStep << std::endl;
            uchar* ptr = (uchar*) (imgSource->imageData + lineIndex * imgSource->widthStep);
            if (ptr != 0) {
                g1 = *(ptr);
            }

            for (int i = 1; i < imgSource->width; i++) {

                g = *(ptr++);
                // image Gray
                cvLine(imgGray, cvPoint(i - 1, g1), cvPoint(i, g), CV_RGB(0, 10, 100), 1, 8);

                g1 = g;
            }
            cvLine(imgSource, cvPoint(0, lineIndex), cvPoint(imgSource->width - 1, lineIndex), colorOfLine);
            if (isModeWindows) {
                cvShowImage("Output Source", imgSource);
                cvShowImage("Output Gray", imgGray);
            }
            cvWaitKey(0);
            cvReleaseImage(&imgSource);
            cvReleaseImage(&imgGray);
        }

        if (isModeWindows) {
            cvDestroyWindow("Windows");
        }

        // Save image
        //  if(!cvSaveImage(outFileName,img)) printf("Could not save: %s\n",outFileName);
    } catch (char* e) {
        std::cout << "Error:" << e << std::endl;
        std::cout << "Using:" << std::endl;
        std::cout << "tp1 -l <index de ligne> [-w] fichierImage" << std::endl;
    }
    return (EXIT_SUCCESS);
}


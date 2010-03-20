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
#include <cstring>
#include <stdio.h>

#define COLOR_COORDINATE CV_RGB(120, 10, 120)

const char* getFilePathName(char* filepath, char* newExtension);
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY);
void drawCoordinate(IplImage* img, int deltaX, int deltaY);
IplImage* initGraphImage(IplImage* imgSource, int deltaX, int deltaY);
/*
 * 
 */
int main(int argc, char** argv) {
    try {
        // Des parametres simples
        int lineIndex = -1; //ligne horizental du fichier
        //int graphSize = 255; // la hauteur de image de niveau de gris, de rouge,
        bool isColor = false; // est une image colore ou non
        char* filePath = NULL; // la chemin de l'image
        bool isModeWindows = false; //determinier si le programme marche sous le mode de fenette
        int deltaX = 30; //la valeur de bord avec le graphe
        int deltaY = 10; //la valeur de bord y avec le graphe
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
            throw "Image Error:";
        }
        if (lineIndex == -1) {
            lineIndex = imgSource->height / 2;
        }
        
        if (isModeWindows)
            cvNamedWindow("Windows", CV_WINDOW_AUTOSIZE);

        // Verifier que c'est une image colore ou non
        isColor = (imgSource -> nChannels == 3);
        if (isColor) {
            //CvSize size = new CvSize(300,img->width);
            //IplImage* imgGraph = cvCreateImage(cvSize(imgSource->width, graphSize), IPL_DEPTH_8U, 3);

            IplImage* imgRed = initGraphImage(imgSource, deltaX, deltaY);
            IplImage* imgBlue = cvCloneImage(imgRed);
            IplImage* imgGreen = cvCloneImage(imgRed);
            IplImage* imgRGB = cvCloneImage(imgRed);


            uchar r, g, b;
            uchar r1, g1, b1;
            //std::cout << img->width << " " << img->widthStep << std::endl;
            uchar* ptr = (uchar*) (imgSource->imageData + lineIndex * imgSource->widthStep);
            // des valeurs initiales
            if (ptr != 0) {
                b1 = *(ptr);
                g1 = *(ptr++);
                r1 = *(ptr++);
            }

            for (int i = 1; i < imgSource->width; i++) {
                // des valeurs souvantes
                b = *(ptr++);
                g = *(ptr++);
                r = *(ptr++);

                // image RGB
                cvLine(imgRGB, getCoordinate(i - 1, b1, deltaX, deltaY),
                        getCoordinate(i, b, deltaX, deltaY), CV_RGB(0, 0, 255), 1, 8);
                cvLine(imgRGB, getCoordinate(i - 1, g1, deltaX, deltaY),
                        getCoordinate(i, g, deltaX, deltaY), CV_RGB(0, 255, 0), 1, 8);
                cvLine(imgRGB, getCoordinate(i - 1, r1, deltaX, deltaY),
                        getCoordinate(i, r, deltaX, deltaY), CV_RGB(255, 0, 0), 1, 8);
                
                // image Red
                cvLine(imgRed, getCoordinate(i - 1, r1, deltaX, deltaY),
                        getCoordinate(i, r, deltaX, deltaY), CV_RGB(255, 0, 0), 1, 8);
                // image Green
                cvLine(imgGreen, getCoordinate(i - 1, g1, deltaX, deltaY),
                        getCoordinate(i, g, deltaX, deltaY), CV_RGB(0, 255, 0), 1, 8);
                // image Blu
                cvLine(imgBlue, getCoordinate(i - 1, b1, deltaX, deltaY),
                        getCoordinate(i, b, deltaX, deltaY), CV_RGB(0, 0, 255), 1, 8);

                b1 = b;
                g1 = g;
                r1 = r;
            }
            cvLine(imgSource, cvPoint(0, lineIndex), cvPoint(imgSource->width - 1, lineIndex), colorOfLine);
            // Ouvrir des fenetes
            if (isModeWindows) {
                cvShowImage("Output Source", imgSource);
                cvShowImage("Output Red", imgRed);
                cvShowImage("Output Blue", imgBlue);
                cvShowImage("Output Green", imgGreen);
                cvShowImage("Output RGB", imgRGB);

            }
            // savegarder les images sous fichiers
            cvSaveImage(getFilePathName(filePath, ".Red.jpg"), imgRed);
            cvSaveImage(getFilePathName(filePath, ".Green.jpg"), imgGreen);
            cvSaveImage(getFilePathName(filePath, ".Blue.jpg"), imgBlue);
            cvSaveImage(getFilePathName(filePath, ".RGB.jpg"), imgRGB);
            cvSaveImage(getFilePathName(filePath, ".Line.jpg"), imgSource);

            // viter la memoire
            cvWaitKey(0);
            cvReleaseImage(&imgSource);
            cvReleaseImage(&imgRed);
            cvReleaseImage(&imgBlue);
            cvReleaseImage(&imgGreen);
            cvReleaseImage(&imgRGB);


        } else {
            // init une image de graphe
            IplImage* imgGray = initGraphImage(imgSource, deltaX, deltaY);

            uchar g, g1; // valeur de niveau de gris

            // pointer de la zone de donnees de image
            uchar* ptr = (uchar*) (imgSource->imageData + lineIndex * imgSource->widthStep);
            if (ptr != 0) {
                g1 = *(ptr); // la valeur premiere de niveau de gris
            }

            for (int i = 1; i < imgSource->width; i++) {
                g = *(ptr++); // la valeur souvante
                // dessiner le ligne de niveau de gris sur l'image Gray
                cvLine(imgGray, getCoordinate(i - 1, g1, deltaX, deltaY),
                        getCoordinate(i, g, deltaX, deltaY), CV_RGB(100, 100, 100), 1, 8);
                g1 = g;
            }

            // dessiner une ligne sur l'image de source
            cvLine(imgSource, cvPoint(0, lineIndex), cvPoint(imgSource->width - 1, lineIndex), colorOfLine);
            // Si dans le mode interactif ou non
            if (isModeWindows) {
                cvShowImage("Output Source", imgSource);
                cvShowImage("Output Gray", imgGray);
            }
            // sauvegarder sous fichier
            cvSaveImage(getFilePathName(filePath, ".Gray.jpg"), imgGray);
            cvSaveImage(getFilePathName(filePath, ".Line.jpg"), imgSource);

            // viter la memoire
            cvWaitKey(0);
            cvReleaseImage(&imgSource);
            cvReleaseImage(&imgGray);
        }

        // fermer le fenete si sous la mode de interactif
        if (isModeWindows) {
            cvDestroyWindow("Windows");
        }
        
    } catch (const char* e) {
        // error 
        std::cout << "Error:" << e << std::endl;
        std::cout << "Using:" << std::endl;
        std::cout << "tp1 -l <index de ligne> [-w] fichierImage" << std::endl;
    }
    return (EXIT_SUCCESS);
}

/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(char* filepath, char* newExtension) {
    std::string a(filepath);
    std::string b(newExtension);
    return (a + b).c_str();
}

/* initialiser une image de graphe
 *
 */
IplImage* initGraphImage(IplImage* imgSource, int deltaX, int deltaY) {
    IplImage* newImg = cvCreateImage(cvSize(imgSource->width + 2 * deltaX, 255 + 2 * deltaY), IPL_DEPTH_8U, 3);
    drawCoordinate(newImg, deltaX, deltaY);
    return newImg;
}

/*
 * dessiner des coordinates de graphe
 */
void drawCoordinate(IplImage* img, int deltaX, int deltaY) {
    // dessiner la ligne x
    cvLine(img, cvPoint(deltaX, deltaY + 255), cvPoint(img->width - deltaX, deltaY + 255), COLOR_COORDINATE);
    // dessiner la ligne y
    cvLine(img, cvPoint(deltaX, deltaY + 255), cvPoint(deltaX, deltaY), COLOR_COORDINATE);
    // dessiner des repere sur la ligne y
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 0.5);
    char buffer [50];
    for (int i = 0; i <= 255; i += 15) {
        sprintf(buffer, "%d", i);
        cvPutText(img, buffer, cvPoint(0, deltaY + 255 - i), &font, COLOR_COORDINATE);
    }

}

/*
 * calculer la valeur de point x,y sur la coordinate
 */
CvPoint getCoordinate(int x, int y, int deltaX, int deltaY) {
    return cvPoint(x + deltaX, deltaY + 255 - y);
}
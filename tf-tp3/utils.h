/* 
 * File:   utils.h
 * Author: thuan
 *
 * Created on April 17, 2010, 2:41 PM
 */

#ifndef _UTILS_H
#define	_UTILS_H
#include <stdlib.h>

#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char* filepath, const char* newExtension);
/*
 * generer la chemin de fichier
 *
 */
const char* getFilePathName(const char** filepath,int length);
/*
 * supprimer une serie d'image
 */
void releaseMemory(IplImage** images,int length);
/*
 * convertir une image a image 8U
 */
IplImage* convertToImage8U(IplImage* image);
#endif	/* _UTILS_H */


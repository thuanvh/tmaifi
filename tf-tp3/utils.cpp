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


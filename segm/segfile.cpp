#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "segfile.h"
using namespace std;

SegImage::SegImage() {
  width = 0;
  height = 0;
  segments = 0;
  gray = false;
  invert = false;
  flipflop = false;
  image = NULL;
}

string trim(string str) {

  while(str[0]==' '){
  
  str.erase(str.begin());
  }

  while(str[str.size()-1]==' '){
    
  str.erase(str.end());
  }
  return str;
}

/*
 * Read seg file
 *
format ascii cr
date Fri Mar 9 16:43:30 2001
image 33039
user 1102
width 321
height 481
segments 12
gray 0
invert 0
flipflop 0
data
 */
SegImage* readSegFile(char* fileSeg) {

  char str[2000];
  fstream file_op(fileSeg);

  SegImage* segimage=new SegImage();

  // read header
  while (!file_op.eof()) {
    file_op.getline(str, 2000);
    string a(str);
    if (a[0] == '#')
      continue;

    int pos = a.find(' ');
    string cmd = a.substr(0, pos);
    string strValue = a.substr(pos + 1);
    strValue = trim(strValue);
    if (cmd.compare("format") == 0) {
      pos = strValue.find(' ');
      string ascii = strValue.substr(0, pos );
      if (ascii.compare("ascii") == 0) {
        strValue = strValue.substr(pos + 1);
        strValue = trim(strValue);
        if (strValue[0] == 'c' && strValue[1] == 'r') {
          continue;
        } else {
          throw "Error: Not seg cr file.";
        }

      } else {
        throw "Error: Not seg ascii file.";
      }
    } else if (cmd.compare("date") == 0) {
      continue;
    } else if (cmd.compare("image") == 0) {
      continue;
    } else if (cmd.compare("user") == 0) {
      continue;
    } else if (cmd.compare("width") == 0) {
      pos = strValue.find(' ');
      string strwidth = strValue.substr(0, pos);
      segimage->width = atoi(strwidth.c_str());
    } else if (cmd.compare("height") == 0) {
      pos = strValue.find(' ');
      string strheight = strValue.substr(0, pos);
      segimage->height = atoi(strheight.c_str());
    } else if (cmd.compare("segments") == 0) {
      pos = strValue.find(' ');
      string strsegments = strValue.substr(0, pos );
      segimage->segments = atoi(strsegments.c_str());
    } else if (cmd.compare("gray") == 0) {
      pos = strValue.find(' ');
      string strgray = strValue.substr(0, pos );
      segimage->gray = (strgray[0] == '0') ? false : true;
    } else if (cmd.compare("invert") == 0) {
      pos = strValue.find(' ');
      string strinvert = strValue.substr(0, pos );
      segimage->invert = (strinvert[0] == '0') ? false : true;
    } else if (cmd.compare("flipflop") == 0) {
      pos = strValue.find(' ');
      string strflipflop = strValue.substr(0, pos );
      segimage->flipflop = (strflipflop[0] == '0') ? false : true;
    } else if (cmd.compare("data") == 0) {
      break;
    }

  }
  //
  segimage->image = cvCreateImage(cvSize(segimage->width, segimage->height), IPL_DEPTH_8U, 1);


  // read data
  while (!file_op.eof()) {
    file_op.getline(str, 2000);
    string a(str);
    int pos = a.find(' ');
    string strseg = trim(a.substr(0, pos ));
    int seg = atoi(strseg.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strrow = trim(a.substr(0, pos ));
    int row = atoi(strrow.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strcol1 = trim(a.substr(0, pos ));
    int col1 = atoi(strcol1.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strcol2 = trim(a.substr(0, pos ));
    int col2 = atoi(strcol2.c_str());

    int min = col1 < col2 ? col1 : col2;
    int max = col1 > col2 ? col1 : col2;
    for (int i = min; i <= max; i++) {
      cvSetAt(segimage->image, cvScalar(seg), row, i);
    }
  }
  file_op.close();
  return segimage;
}

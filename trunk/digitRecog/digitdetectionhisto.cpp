
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "digitdetection.h"

using namespace std;
using namespace cv;

#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define NUM_DIGIT 10

void learningHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath) {
  // read training image
  ifstream ifimage;
  ifimage.open(trainingimage, ios_base::in);
  // read training label
  ifstream iflabel;
  iflabel.open(traininglabel, ios_base::in);

  string imageline;
  string labelvalue;

  // histogram array
  float*** histos = new float**[NUM_DIGIT];

  for (int i = 0; i < NUM_DIGIT; i++) {
    histos[i] = new float*[IMAGE_HEIGHT];
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
      histos[i][j] = new float[IMAGE_WIDTH]; //{0};
      for (int k = 0; k < IMAGE_WIDTH; k++) {
        histos[i][j][k] = 0;
      }
    }
  }
  int learningImageNumber[NUM_DIGIT] = {0};

  while (iflabel.good()) {
    getline(iflabel, labelvalue);
    int label = atoi(labelvalue.c_str());
    cout << label;
    if (label >= 0 && label < NUM_DIGIT) {
      Mat mat;
      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);

      float** img;
      img = histos[label];

      learningImageNumber[label]++;
      // Read image
      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
        //cout<<imageline<<endl;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          img[i][j] += (imageline[j] == ' ') ? 0 : 1;
          //cout<<img[i][j];
          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
          //mat.setTo(a);
        }
        //cout<<endl;
      }

      // show image
      //imshow("Learning Digit", mat);
      //waitKey();
      mat.release();
    }
  }


  // normalize histogram
  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
      for (int j = 0; j < IMAGE_WIDTH; j++) {
        histos[histIndex][i][j] /= (float) learningImageNumber[histIndex];
      }
    }
  }
  // save histogram
  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    char fileout[255];
    sprintf(fileout, "%s/%d.histo", outputDirPath, histIndex);
    //string a = string(outputDirPath) + "/" + histIndex + ".histo";
    SaveHistograme(histos[histIndex], fileout);
  }
  // show histogram
  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    char name[255];
    sprintf(name, "histo%d", histIndex);
    showHistogram(histos[histIndex], name, outputDirPath);
  }

  //ifs.read()
}

void showHistogram(float** hist, const char* histoname, const char* outputDirPath) {
  int scale = 5;
  Mat histImg = Mat::zeros(IMAGE_WIDTH * scale, IMAGE_HEIGHT * scale, CV_8U);
  //  int total = 0;
  cout << endl;
  for (int h = 0; h < IMAGE_HEIGHT; h++) {
    for (int w = 0; w < IMAGE_WIDTH; w++) {
      float binVal = hist[h][w];

      //      total += binVal;
      int intensity = cvRound(binVal * 255);
      cout << intensity << "\t";
      rectangle(histImg, Point(w*scale, h * scale),
        Point((w + 1) * scale - 1, (h + 1) * scale - 1),
        Scalar::all(intensity),
        CV_FILLED);
    }
    cout << endl;
  }
  cout << endl;
  //cout << "total point" << total << endl;
  //  namedWindow("Source", 1);
  //  imshow("Source", src);

  //namedWindow(itoa(label), 1);
  imshow(histoname, histImg);
  char file[255];
  sprintf(file, "%s/%s.png", outputDirPath, histoname);
  imwrite(file, histImg);
  waitKey();
  histImg.release();
}

void SaveHistograme(float** hist, char* name) {
  ofstream ofs;
  ofs.open(name);
  for (int a = 0; a < IMAGE_HEIGHT; a++) {
    for (int b = 0; b < IMAGE_WIDTH; b++) {
      ofs << hist[a][b] << " ";
    }
    ofs << endl;
  }
  ofs.close();
}

void LoadHistograme(float** hist, const char* name) {
  ifstream ifs;
  ifs.open(name);

  for (int a = 0; a < IMAGE_HEIGHT; a++) {
    for (int b = 0; b < IMAGE_WIDTH; b++) {
      float value = 0;
      ifs >> hist[a][b];
      //cout << hist[a][b];
    }
    //cout << endl;
  }
  ifs.close();

}

void TestingHisto(char* histoDirPath, char* testImagePath, const char* testLabelPath) {
  // histogram array
  float*** histos = new float**[NUM_DIGIT];

  for (int i = 0; i < NUM_DIGIT; i++) {
    histos[i] = new float*[IMAGE_HEIGHT];
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
      histos[i][j] = new float[IMAGE_WIDTH]; //{0};
      for (int k = 0; k < IMAGE_WIDTH; k++) {
        histos[i][j][k] = 0;
      }
    }
  }
  // load histogram
  cout << "loading histogram" << endl;
  for (int i = 0; i < NUM_DIGIT; i++) {
    char filename[255];
    sprintf(filename, "%s/%d.histo", histoDirPath, i);
    //filename += string(histoDirPath) + "/" + i + ".histo";
    LoadHistograme(histos[i], filename);
  }
  // read testing image
  cout << "loading testing image" << endl;
  ifstream ifimage;
  ifimage.open(testImagePath, ios_base::in);
  // read testing label
  ifstream iflabel;
  iflabel.open(testLabelPath, ios_base::in);

  string imageline;
  string labelvalue;

  // histogram array
  int total = 0;
  int good = 0;
  int bad = 0;
  while (iflabel.good()) {
    getline(iflabel, labelvalue);
    cout << labelvalue << endl;
    int label = atoi(labelvalue.c_str());
    cout << label << endl;
    if (label >= 0 && label < NUM_DIGIT) {
      Mat mat;
      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);

      int** img = new int*[IMAGE_HEIGHT];
      for (int j = 0; j < IMAGE_HEIGHT; j++) {
        img[j] = new int[IMAGE_WIDTH]; //{0};
        for (int k = 0; k < IMAGE_WIDTH; k++) {
          img[j][k] = 0;
        }
      }
      // Read image
      cout << "Reading image " << label << endl;
      //waitKey();
      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          img[i][j] += (imageline[j] == ' ') ? 0 : 1;
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
          //mat.setTo(Scalar((imageline[j] == ' ') ? 255 : 0));
        }
      }

      // Evaluer Bayes
      int digitChoose = 0;
      float valueMax = 0;
      for (int digit = 0; digit < NUM_DIGIT; digit++) {
        float prob = calculProb(img, histos[digit]);
        cout << digit << ":" << prob << endl;
        if (valueMax < prob) {
          valueMax = prob;
          digitChoose = digit;
        }
      }
      total++;
      if (label != digitChoose)
      {
        cout << label << digitChoose << "false" << endl;
        bad++;
      } else {
        cout << label << digitChoose << "true" << endl;
        good++;
      }

      // show image
      imshow("Learning Digit", mat);
      //waitKey();
      mat.release();
    }
  }
  cout << "total:" << total << "\t" << "good:" << good << "\tbad:" << bad << endl;
}

float calculProb(int** img, float** hist) {
  float prob = 1;
  for (int i = 0; i < IMAGE_HEIGHT; i++) {
    for (int j = 0; j < IMAGE_WIDTH; j++) {
      if (img[i][j] == 1) {
        prob *= (hist[i][j] + 1);
      }
    }
  }
  return prob;
}


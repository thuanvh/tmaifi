
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
#include "facedetection.h"

using namespace std;
using namespace cv;

#define IMAGE_WIDTH 60
#define IMAGE_HEIGHT 70
#define NUM_DIGIT 2

void extractAttributes(const char* faceFile, const char* faceLabelFile, const char* dataoutfile, const char* sampleFaceHistoFile, int startRow, int startCol, int width, int height) {
  // read training image
  ifstream ifimage;
  ifimage.open(faceFile, ios_base::in);
  // read training label
  ifstream iflabel;
  iflabel.open(faceLabelFile, ios_base::in);

  ofstream ofdata;
  ofdata.open(dataoutfile);
  //  cout<<"wrote";

  cout << "width:" << width << "-" << "height" << height << endl;



  ofdata << "@RELATION digit" << endl;
  ofdata << "@ATTRIBUTE distance_histo			integer" << endl;
  ofdata << "@ATTRIBUTE number_symetric_point 		integer" << endl;
  //  ofdata << "@ATTRIBUTE number_ending_point_q1 		integer" << endl;
  //  ofdata << "@ATTRIBUTE number_ending_point_q2 		integer" << endl;
  //  ofdata << "@ATTRIBUTE number_ending_point_q3 		integer" << endl;
  //  ofdata << "@ATTRIBUTE number_ending_point_q4 		integer" << endl;
  ofdata << "@ATTRIBUTE class 		{0,1}	" << endl;
  ofdata << "@DATA" << endl;

  string imageline;
  string labelvalue;

  // histogram array
  float** histos = new float*[IMAGE_HEIGHT];
  for (int j = 0; j < IMAGE_HEIGHT; j++) {
    histos[j] = new float[IMAGE_WIDTH]; //{0};
    for (int k = 0; k < IMAGE_WIDTH; k++) {
      histos[j][k] = 0;
    }
  }

  // Load face region histogramme
  cout << "Load histos" << endl;
  LoadHistograme(histos, sampleFaceHistoFile);
  cout << "End Load histos" << endl;

  float min = 1;
  float max = 0;
  float** faceHisto = new float*[height];
  for (int j = 0; j < height; j++) {
    faceHisto[j] = new float[width]; //{0};
    for (int k = 0; k < width; k++) {
      faceHisto[j][k] = histos[j + startRow][k + startCol];
      min = (faceHisto[j][k] < min) ? faceHisto[j][k] : min;
      max = (faceHisto[j][k] > max) ? faceHisto[j][k] : max;
    }
  }
  cout << "min: " << min << " max: " << max << endl;
  // normalize

  for (int j = 0; j < height; j++) {
    for (int k = 0; k < width; k++) {
      float x = faceHisto[j][k];
      faceHisto[j][k] = (x - min)*1 / (max - min);
      cout << faceHisto[j][k] << "\t";
    }
    cout << endl;
  }


  float** subregion = new float*[height];
  for (int j = 0; j < height; j++) {
    subregion[j] = new float[width]; //{0};
    for (int k = 0; k < width; k++) {
      subregion[j][k] = 0;
    }
  }

  cout << "End load sub face histos" << endl;
  //  getchar();
  int learningImageNumber[NUM_DIGIT] = {0};
  int learningtotal = 0;
  while (iflabel.good()) {
    getline(iflabel, labelvalue);
    int label = atoi(labelvalue.c_str());
    cout << ++learningtotal << "-" << label;
    if (label >= 0 && label < NUM_DIGIT) {

      Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);

      learningImageNumber[label]++;
      // Read image
      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
        //        cout << imageline << endl;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          //          histos[label][i][j] += (imageline[j] == ' ') ? 0 : 1;
          //          cout<<histos[label][i][j]<<" ";
          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
          //mat.setTo(a);
        }
        //        cout << endl;
      }

      //calculate the max value of prob
      float max = 4000;
      float maxIndexRow;
      float maxIndexCol;
      //      for (int i = 0; i < IMAGE_HEIGHT - height; i++) {
      //        for (int j = 0; j < IMAGE_WIDTH - width; j++) {
      for (int i = startRow - HISTO_DELTA; i < startRow + HISTO_DELTA; i++) {
        for (int j = startCol - HISTO_DELTA; j < startCol + HISTO_DELTA; j++) {
          //          cout << i << " " << j << " ";
          //          getchar();
          //          Mat subregion(mat, Rect(i, j, i + height, j + width));
          for (int k = 0; k < height; k++) {
            for (int l = 0; l < width; l++) {
              subregion[k][l] = (mat.at<uchar > (i + k, j + l) == 0) ? 1 : 0;
              //              cout<<subregion[k][l];
            }
            //            cout<<endl;
          }
          //          imshow("subreg",subregion);
          //          waitKey();
          // Calculer la distance Khi-2
          float prob = calculProb(subregion, faceHisto, height, width);
          cout << prob << " ";
          //          getchar();
          if (max > prob) {
            max = prob;
            maxIndexRow = i;
            maxIndexCol = j;
          }
        }
      }

      for (int k = 0; k < height; k++) {
        for (int l = 0; l < width; l++) {
          subregion[k][l] = (mat.at<uchar > (maxIndexRow + k, maxIndexCol + l) == 0) ? 1 : 0;
        }
      }
      int nb_sym_points = 0;
      for (int k = 0; k < height; k++) {
        for (int l = 0; l < width / 2; l++) {
          if (subregion[k][l] > 0 && subregion[k][width - l] > 0) {
            nb_sym_points++;
          }
        }
      }

      // print data file
      ofdata << (int) max << "," << nb_sym_points << "," << label << endl;
      cout << " max: " << (int) max << "," << nb_sym_points << "," << label << endl;
      // print image file
      char file[255];
      Mat mat3;
      cvtColor(mat, mat3, CV_GRAY2BGR);
      rectangle(mat3, Rect(maxIndexCol, maxIndexRow, width, height), Scalar(255, 0, 0));
      sprintf(file, "%d.%d.learn.jpg", learningtotal, label);
      imwrite(file, mat3);
      // show image
      //      imshow("Learning Digit", mat);
      //      waitKey();
      //      mat.release();
    }
  }
  ofdata.close();
  ifimage.close();
  iflabel.close();
}

void testingHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath, const char* command) {
  int width = 30;
  int height = 35;
  int startRow = 20;
  int startCol = 15;

  Mat histImg;
  //imshow(histoname, histImg);
  char file[255];
  sprintf(file, "%s/histo1.png", outputDirPath);
  //imread(file, histImg);
  cout << "reading " << file << endl;
  if ((histImg = imread(file, 0)).data) {
    Mat histImg3;
    cvtColor(histImg, histImg3, CV_GRAY2BGR);
    rectangle(histImg3,
      Rect(startCol*HISTO_SCALE,
      startRow*HISTO_SCALE,
      (width) * HISTO_SCALE,
      (height) * HISTO_SCALE), Scalar(255, 0, 0));
    sprintf(file, "%s/histo1.facezone.png", outputDirPath);
    imwrite(file, histImg3);
    cout << "write " << file;
  }

  char sampleFaceHistoFile[255];
  sprintf(sampleFaceHistoFile, "%s/1.histo", outputDirPath);
  char outfile[255];
  sprintf(outfile, "%s/%s.arff", outputDirPath, command);
  extractAttributes(trainingimage, traininglabel, outfile, sampleFaceHistoFile, startRow, startCol, width, height);
}

void learningHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath, const char* command) {
  generateHisto(trainingimage, traininglabel, outputDirPath);

  testingHisto(trainingimage, traininglabel, outputDirPath, command);
}

void generateHisto(const char* trainingimage, const char* traininglabel, const char* outputDirPath) {
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
  //  float histos[NUM_DIGIT][IMAGE_HEIGHT][IMAGE_WIDTH];

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
  int learningtotal = 0;
  while (iflabel.good()) {
    getline(iflabel, labelvalue);
    int label = atoi(labelvalue.c_str());
    cout << ++learningtotal << "-" << label << endl;
    if (label >= 0 && label < NUM_DIGIT) {
      //      Mat mat;
      //      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);
      Mat mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);

      //      float** img;
      //      img = histos[label];

      learningImageNumber[label]++;
      // Read image
      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
        //        cout << imageline << endl;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          histos[label][i][j] += (imageline[j] == ' ') ? 0 : 1;
          //          cout<<histos[label][i][j]<<" ";
          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
          //mat.setTo(a);
        }
        //        cout << endl;
      }

      char file[255];
      sprintf(file, "%d.%d.jpg", learningtotal, label);
      imwrite(file, mat);
      // show image
      //      imshow("Learning Digit", mat);
      //      waitKey();
      //      mat.release();
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

  ifimage.close();
  iflabel.close();
}

void showHistogram(float** hist, const char* histoname, const char* outputDirPath) {
  int scale = HISTO_SCALE;
  Mat histImg = Mat::zeros(IMAGE_HEIGHT * scale, IMAGE_WIDTH * scale, CV_8UC1);
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
  //  waitKey();
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

float calculProb(const Mat& img, float** hist) {
  float prob = 0;
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      if (img.at<uchar > (i, j) == 0) {
        prob += hist[i][j];
      }
    }
  }
  return prob;
}

float calculProb(float** img, float** hist, int height, int width) {
  float prob = 0;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int imgval = (img[i][j] == 0) ? 0 : 1;
      if (imgval != 0 || hist[i][j] != 0) {
        prob += pow(imgval - hist[i][j], 2) / (imgval + hist[i][j]);
      }
    }
  }
  return prob;
}

void freememory(float** img, int height, int width) {
  for (int i = 0; i < height; i++) {
    delete img[i];
  }
}

void freememory(float*** img, int height, int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      delete img[i][j];
    }
    delete img[i];
  }
}

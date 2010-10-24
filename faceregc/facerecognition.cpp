
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
#include "facerecognition.h"

using namespace std;
using namespace cv;

#define IMAGE_WIDTH 60
#define IMAGE_HEIGHT 70
#define NUM_DIGIT 2

void extractSUFT(const Mat& img, vector<KeyPoint>& allKeyPoints, vector<float>& alldescriptors) {
  SURF surf;
  vector<KeyPoint> keypoints;
  vector<float> descriptors;

  surf(img, Mat(), keypoints, descriptors);
  int size = surf.descriptorSize();
  vector<int> ptpairs;
  findPairs(keypoints, descriptors, allKeyPoints, alldescriptors, ptpairs, size);
}

double compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length) {
  double total_cost = 0;
  assert(length % 4 == 0);
  int i, j;
  for (i = start1, j = start2; i < length; i += 4, j += 4) {
    double t0 = d1[i] - d2[j];
    double t1 = d1[i + 1] - d2[j + 1];
    double t2 = d1[i + 2] - d2[j + 2];
    double t3 = d1[i + 3] - d2[j + 3];
    total_cost += t0 * t0 + t1 * t1 + t2 * t2 + t3*t3;
    if (total_cost > best)
      break;
  }
  return total_cost;
}

int naiveNearestNeighbor(const vector<float>& vec, int laplacian,
  const vector<KeyPoint>& model_keypoints,
  const vector<float>& model_descriptors, int start, int length) {

  int i, neighbor = -1;
  double d, dist1 = 1e6, dist2 = 1e6;

  int start1 = start;
  for (i = 0; i < model_keypoints.size(); i++) {
    const KeyPoint* kp = (const KeyPoint*) &model_keypoints[i];


    //    if (laplacian != kp->laplacian)
    //      continue;
    int start2 = i*length;
    d = compareSURFDescriptors(vec, model_descriptors, dist2, start1, start2, length);
    if (d < dist1) {
      dist2 = dist1;
      dist1 = d;
      neighbor = i;
    } else if (d < dist2)
      dist2 = d;
  }
  if (dist1 < 0.6 * dist2)
    return neighbor;
  return -1;
}

void findPairs(const vector<KeyPoint>& objectKeypoints, const vector<float>& objectDescriptors,
  const vector<KeyPoint>& imageKeypoints, const vector<float>& imageDescriptors, vector<int>& ptpairs, int size) {
  int i;
  ptpairs.clear();
  for (i = 0; i < objectKeypoints.size(); i++) {
    const KeyPoint* kp = (const KeyPoint*) &objectKeypoints[i];
    int start1 = i*size;
    int nearest_neighbor = naiveNearestNeighbor(objectDescriptors, kp->response, imageKeypoints, imageDescriptors, start1, size);
    if (nearest_neighbor >= 0) {
      ptpairs.push_back(i);
      ptpairs.push_back(nearest_neighbor);
    }
  }
}

void extractAttributes(const char* faceFile, const char* faceLabelFile, const char* dataoutfile, const char* sampleFaceHistoFile, int startRow, int startCol, int width, int height) {

  // read training image
  ifstream ifimage;
  ifimage.open(faceFile, ios_base::in);
  // read training label
  ifstream iflabel;
  iflabel.open(faceLabelFile, ios_base::in);

  ofstream ofdata;
  ofdata.open(dataoutfile);

  cout << "width:" << width << "-" << "height" << height << endl;

  ofdata << "@RELATION digit" << endl;
  ofdata << "@ATTRIBUTE distance_histo			integer" << endl;
  ofdata << "@ATTRIBUTE number_symetric_point 		integer" << endl;
  ofdata << "@ATTRIBUTE number_symetric_point_neg 		integer" << endl;
  //  ofdata << "@ATTRIBUTE  nb_points_up integer" << endl;
  //  ofdata << "@ATTRIBUTE  nb_points_down integer" << endl;
  //  ofdata << "@ATTRIBUTE  nb_points_left integer" << endl;
  //  ofdata << "@ATTRIBUTE  nb_points_right integer" << endl;
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
      // number of point symmetric and asymmetric
      int nb_sym_points = 0;
      int nb_sym_points_neg = 0;
      int nb_points_up = 0;
      int nb_points_down = 0;
      int nb_points_left = 0;
      int nb_points_right = 0;
      for (int k = 0; k < height; k++) {
        for (int l = 0; l < width / 2; l++) {
          if (subregion[k][l] > 0 && subregion[k][width - l] > 0) {
            nb_sym_points++;
          }
          if (subregion[k][l] != subregion[k][width - l]) {
            nb_sym_points_neg++;
          }
        }
      }
      for (int k = 0; k < height; k++) {
        for (int l = 0; l < width; l++) {
          if (subregion[k][l] > 0) {
            if (k < height / 2) {
              nb_points_up++;
            } else {
              nb_points_down++;
            }
            if (l < width / 2) {
              nb_points_left++;
            } else {
              nb_points_right++;
            }
          }
        }
      }

      // print data file
      ofdata
        << (int) max
        << "," << nb_sym_points
        << "," << nb_sym_points_neg
        //        << "," << (nb_points_up - nb_points_down<0?-1:1)
        ////        << "," << nb_points_down
        //        << "," << (nb_points_left - nb_points_right<0?-1:1)
        ////        << "," << nb_points_right
        << "," << label << endl;

      cout
        << " max: " << (int) max
        << "," << nb_sym_points
        << "," << nb_sym_points_neg
        << "," << nb_points_up - nb_points_down
        //        << "," << nb_points_down
        << "," << nb_points_left - nb_points_right
        //        << "," << nb_points_right
        << "," << label << endl;
      // print image file
      char file[255];
      Mat mat3;
      cvtColor(mat, mat3, CV_GRAY2BGR);
      rectangle(mat3, Rect(maxIndexCol, maxIndexRow, width, height), Scalar(0, 0, 255));
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

void testing(const char* dirPath, const char* name) {
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

  char outFileName[255];
  sprintf(outFileName, "%s", name);
  ifstream outfile;
  outfile.open(outFileName);

  vector<KeyPoint> allKeyPoints;
  vector<float> alldescriptors;

  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;


    Mat src;

    if (filename == "")
      continue;
    bool debug = false;

    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;

    extractSUFT( src, allKeyPoints,  alldescriptors);
  }
}

void learning(const char* dirPath, const char* name) {
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

  char outFileName[255];
  sprintf(outFileName, "%s", name);
  ofstream outfile;
  outfile.open(outFileName);

  vector<KeyPoint> allKeyPoints;
  vector<float> alldescriptors;

  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;


    Mat src;

    if (filename == "")
      continue;
    bool debug = false;

    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;

    extractSUFT( src, allKeyPoints,  alldescriptors);
  }
//  generateHisto(trainingimage, traininglabel, outputDirPath);
//
//  testingHisto(trainingimage, traininglabel, outputDirPath, command);
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

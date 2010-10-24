
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

void extractSUFT(const Mat& img, vector<KeyPoint>& allKeyPoints, vector<float>& alldescriptors, ostream& featurefile, bool createDict, ostream& dictFile) {
  SURF surf;
  vector<KeyPoint> keypoints;
  vector<float> descriptors;
  surf.hessianThreshold = 0.00001;
  surf.nOctaveLayers = 4;
  surf.nOctaves = 4;
  surf.extended = 1;
  surf(img, Mat(), keypoints, descriptors);

  int size = surf.descriptorSize();
  vector<int> ptpairs;
  //  cout<<"size:"<<size<<endl;
  findPairs(keypoints, descriptors, allKeyPoints, alldescriptors, ptpairs, size);
  //  getchar();
  if (createDict) {
    for (int i = 0; i < ptpairs.size(); i += 2) {

      if (ptpairs[i + 1] < 0) {
        // add keypoint to dictionary
        allKeyPoints.push_back(keypoints[ptpairs[i]]);
        saveToDict(keypoints[ptpairs[i]], descriptors, dictFile, i / 2 * size, size);
        for (int j = 0; j < size; j++) {
          alldescriptors.push_back(descriptors[j + i / 2 * size]);
        }
        ptpairs[i + 1] = allKeyPoints.size() - 1;
      }
    }
  }
  // Save index to fichier
  for (int i = 0; i < ptpairs.size(); i += 2) {
    if (ptpairs[i + 1] >= 0)
      featurefile << ptpairs[i + 1] << " ";
  }
}

void saveToDict(const KeyPoint& kp, const vector<float>& descriptors, ostream& dictFile, int start, int size) {
  dictFile << kp.angle << " ";
  dictFile << kp.class_id << " ";
  dictFile << kp.octave << " ";
  dictFile << kp.pt.x << " ";
  dictFile << kp.pt.y << " ";
  dictFile << kp.response << " ";
  dictFile << kp.size << " ";

  for (int i = 0; i < size; i++) {
    dictFile << descriptors[i + start] << " ";
  }
  dictFile << endl;
}

void loadFromDict(vector<KeyPoint>& kplist, vector<float>& descriptors, istream& dictFile) {
  while (dictFile.good()) {
    KeyPoint kp;
    string line;
    getline(dictFile, line);

    stringstream ss(stringstream::in | stringstream::out);
    ss << line;
    ss >> kp.angle;
    ss >> kp.class_id;
    ss >> kp.octave;
    ss >> kp.pt.x;
    ss >> kp.pt.y;
    ss >> kp.response;
    ss >> kp.size;
    while (ss.good()) {
      float val;
      ss >> val;
      descriptors.push_back(val);
    }
    kplist.push_back(kp);
  }
}

double compareSURFDescriptors(const vector<float>& d1, const vector<float>& d2, double best, int start1, int start2, int length) {
  double total_cost = 0;
  assert(length % 4 == 0);
  int i, j;
  for (i = start1, j = start2; i < start1 + length; i += 4, j += 4) {
    double t0 = d1[i] - d2[j];
    double t1 = d1[i + 1] - d2[j + 1];
    double t2 = d1[i + 2] - d2[j + 2];
    double t3 = d1[i + 3] - d2[j + 3];
    total_cost += t0 * t0 + t1 * t1 + t2 * t2 + t3*t3;
    //    cout << total_cost << " ";
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
    //    cout<<start1<<"-"<<start2<<" ";
    d = compareSURFDescriptors(vec, model_descriptors, dist2, start1, start2, length);
    if (d < dist1) {
      dist2 = dist1;
      dist1 = d;
      neighbor = i;
    } else if (d < dist2)
      dist2 = d;
  }
  //  cout<<endl;
  //  cout<<"dist12:"<<dist1<<"-"<<dist2<<endl;
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
    //    if (nearest_neighbor >= 0) {
    ptpairs.push_back(i);
    ptpairs.push_back(nearest_neighbor);
//    cout << i << "," << nearest_neighbor << " ";
    //    }
  }
}

void extractAttributes(const char* indexFile, int numberFields) {

  // read training image
  ifstream ifimage;
  ifimage.open(indexFile, ios_base::in);

  char arffFile[255];
  sprintf(arffFile, "%s.arff", indexFile);
  ofstream ofdata;
  ofdata.open(arffFile);

  //  cout << "width:" << width << "-" << "height" << height << endl;

  ofdata << "@RELATION digit" << endl;
  for (int i = 0; i < numberFields; i++) {
    ofdata << "@ATTRIBUTE surfPoint_" << i << "			integer" << endl;
  }
  ofdata << "@ATTRIBUTE class 		{";
  for (int i = 1; i <= 40; i++) {
    ofdata << "s" << i;
    if (i < 40)
      ofdata << ",";
  }
  ofdata << "}	" << endl;
  ofdata << "@DATA" << endl;

  while (ifimage.good()) {
    string line;
    getline(ifimage, line);
    stringstream ss(stringstream::in | stringstream::out);
    ss << line;
    string filename;
    ss >> filename;
    if(filename.compare("")==0)
      break;
    string classname;
    ss >> classname;
    vector<int> vec;
    while (ss.good()) {
      int value;
      ss >> value;
      vec.push_back(value);
    }
    for (int i = 0; i < numberFields; i++) {
      bool one = false;
      for (int j = 0; j < vec.size(); j++) {
        if (i == vec[j]) {
          ofdata << "1,";
          one = true;
          break;
        }
      }
      if (!one) {
        ofdata << "0,";
      }
    }
    ofdata << classname << endl;
  }
  ofdata.close();
  ifimage.close();

}

void testing(const char* dirPath, const char* name) {
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

  char outFileName[255];
  sprintf(outFileName, "%s.test", name);
  ofstream outfile;
  outfile.open(outFileName);

  char inDictFileName[255];
  sprintf(inDictFileName, "%s.dict", name);
  ifstream indictfile;
  indictfile.open(inDictFileName);

  vector<KeyPoint> allKeyPoints;
  vector<float> alldescriptors;
  // load dictionary
  loadFromDict(allKeyPoints, alldescriptors, indictfile);

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
    outfile << filename << " ";
    outfile << classname << " ";
    extractSUFT(src, allKeyPoints, alldescriptors, outfile, false, outfile);
    outfile << endl;
  }
  indictfile.close();
  outfile.close();
  listFileStream.close();
  extractAttributes(outFileName, allKeyPoints.size());
}

void learning(const char* dirPath, const char* name) {
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

  char outFileName[255];
  sprintf(outFileName, "%s.learn", name);
  ofstream outfile;
  outfile.open(outFileName);

  char outDictFileName[255];
  sprintf(outDictFileName, "%s.dict", name);
  ofstream outdictfile;
  outdictfile.open(outDictFileName);

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
    outfile << filename << " ";
    outfile << classname << " ";
    extractSUFT(src, allKeyPoints, alldescriptors, outfile, true, outdictfile);
    outfile << endl;
  }
  outdictfile.close();
  outfile.close();
  listFileStream.close();
  extractAttributes(outFileName, allKeyPoints.size());
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

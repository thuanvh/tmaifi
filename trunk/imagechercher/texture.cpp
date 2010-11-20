
#include <vector>
#include <map>
#include "texture.h"
#include <opencv/cvaux.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include "assignmentoptimal.h"
using namespace cv;
using namespace std;

/*
 * Apprendtissage a partir d'une serie d'image de peau
 */
void extract(const char* dirPath, const char* name, int graySize, int colorSize) {

  // read list of file and its class
  DIR *dp;
  struct dirent *ep;
  vector<string> files;

  dp = opendir(dirPath);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      files.push_back((string) ep->d_name);
    }

    (void) closedir(dp);
  } else
    perror(ERR_DIR_OPEN);

  sort(files.begin(), files.end());

  char outFileName[255];
  sprintf(outFileName, "%s", name);
  ofstream outfile;
  outfile.open(outFileName);

  //int size = 32;
  // initialize list of matrix
  double*** concurrenceArray = new double**[NUM_MATRIX];
  for (int i = 0; i < NUM_MATRIX; i++) {
    concurrenceArray[i] = new double*[graySize];
    for (int j = 0; j < graySize; j++) {
      concurrenceArray[i][j] = new double[graySize];
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }

  outfile << dirPath << " " << name << " " << graySize << " " << colorSize << " ";
  outfile << NUM_MATRIX_ATT * NUM_MATRIX << " " << colorSize * colorSize * colorSize << endl;

  for (int i = 0; i < files.size(); i++) {
    string filename = files.at(i);
    Mat src;

    if (filename == "")
      continue;
    bool debug = false;

    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 1)).data)
      continue;

    outfile << filename << " ";

    extractTexture(src, graySize, concurrenceArray, outfile);
    extractHistoColor(src, colorSize, outfile);

    outfile << endl;

  }

  cout << "free memory" << endl;
  freeMatrix(concurrenceArray, graySize);
  outfile.close();
}

void extractHuMomentImage(const Mat& img, ostream& output) {
  //  cout << "convert gray" << endl;
  Mat srcgray;
  cvtColor(img, srcgray, CV_RGB2GRAY);

  //  cout << "contour" << endl;
  Mat dst;
  //  Canny(srcgray,dst,150, 180);
  Laplacian(srcgray, dst, 5);
  Mat imgerode;
  uchar a[9] = {0, 1, 0,
    1, 1, 1,
    0, 1, 0};
  Mat mat4c(3, 3, CV_8UC1, a);
  erode(dst, imgerode, mat4c, Point(-1, -1), 1);
  ////  namedWindow("Components", 1);
  //  imshow("Components", imgerode);

  //  vector<vector<Point> > contours;
  //  vector<Vec4i> hierarchy;
  //  findContours(srcgray, contours, hierarchy,
  //    CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  //  cout << "end find contour"<<endl;
  //  // iterate through all the top-level contours,
  //  // draw each connected component with its own random color
  //  int idx = 0;
  //  Mat dst = Mat::zeros(img.rows, img.cols, CV_8UC1);
  //  Scalar color(255, 255, 255);
  //  for (; idx >= 0; idx = hierarchy[idx][0]) {
  //    drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
  //  }
  //  cout << "end draw contour" << endl;
  //
  //  imshow("gray", srcgray);
  //  imshow("contour", dst);
  //  waitKey();

  //  Moments moment = moments(dst);
  Moments moment = moments(imgerode);

  double huValue[7];
  HuMoments(moment, huValue);
  for (int i = 0; i < 7; i++) {
    output << huValue[i] << " ";
  }
}

void extractHuMoment(const char* dirPath, const char* name) {
  DIR *dp;
  struct dirent *ep;
  vector<string> files;

  dp = opendir(dirPath);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      files.push_back((string) ep->d_name);
    }

    (void) closedir(dp);
  } else
    perror(ERR_DIR_OPEN);

  sort(files.begin(), files.end());

  char outFileName[255];
  sprintf(outFileName, "%s", name);
  ofstream outfile;
  outfile.open(outFileName);

  outfile << dirPath << " " << name << " " << endl;

  for (int i = 0; i < files.size(); i++) {
    string filename = files.at(i);
    Mat src;

    if (filename == "")
      continue;
    bool debug = false;

    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 1)).data)
      continue;

    outfile << filename << " ";


    extractHuMomentImage(src, outfile);
    outfile << endl;
  }

  cout << "free memory" << endl;

  outfile.close();
}

void normalizeHistogram(MatND& hist, int totalpixel, int degree) {
  if (degree == 2) {
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++) {
        hist.at<float>(a, b) /= totalpixel;
      }
  } else if (degree == 3) {
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++) {
        for (int c = 0; c < hist.size[2]; c++) {
          hist.at<float>(a, b, c) /= totalpixel;
        }
      }
  }
}

int histNo(const MatND& hist, int degree) {
  int histno = 0;
  if (degree == 2) {
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++) {
        histno += hist.at<float> (a, b);
      }
  } else if (degree == 3) {
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++)
        for (int c = 0; c < hist.size[2]; c++) {
          histno += hist.at<float> (a, b, c);
        }
  }
  return histno;
}

void SaveMatrix(const MatND& hist, ostream& outfile, int degree) {
  if (degree == 2) {
    for (int a = 0; a < hist.size[0]; a++) {
      for (int b = 0; b < hist.size[1]; b++) {
        outfile << hist.at<float>(a, b) << " ";
        //      cout << hist.at<float>(a, b) << " ";
      }
    }
  } else if (degree == 3) {
    for (int a = 0; a < hist.size[0]; a++) {
      for (int b = 0; b < hist.size[1]; b++) {
        for (int c = 0; c < hist.size[2]; c++) {
          outfile << hist.at<float>(a, b, c) << " ";
          //      cout << hist.at<float>(a, b) << " ";
        }
      }
    }
  }

  //  getchar();
}

void resizeImageColor(const Mat& src, Mat& dst, int num_color) {
  typedef Vec<uchar, 3 > VT;
  MatConstIterator_<VT> it = src.begin<VT > (),
    it_end = src.end<VT > ();
  MatIterator_<VT> itdest = dst.begin<VT > ();
  double scale = (double) num_color / 256;
  for (; it != it_end; ++it, ++itdest) {

    VT srcval = *it;
    *itdest = VT(srcval[0] * scale, srcval[1] * scale, srcval[2] * scale);
  }
}

void extractHistoColorLab(const Mat & src, int colorSize, ostream & outfile) {
  MatND hist; // histogram lab



  int histSize[] = {colorSize, colorSize};

  float aranges[] = {-256, 256};
  float branges[] = {-256, 256};

  const float* ranges[] = {aranges, branges};

  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {1, 2};
  //from 256 to 32
  Mat dst = src.clone();
  resizeImageColor(src, dst, colorSize);
  //  cout<<colorSize<<endl;
  //  imshow("image 256 bit", src);
  //  imshow("image 32 bit", dst);
  //  waitKey();
  Mat srclab;
  cvtColor(dst, srclab, CV_BGR2Lab);

  calcHist(&srclab, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);

  int nbhist = histNo(hist, 2);
  //  cout << "nb hist" << nbhist << endl;
  normalizeHistogram(hist, nbhist, 2);
  SaveMatrix(hist, outfile, 2);

}

void extractHistoColorRGB(const Mat & src, int colorSize, ostream & outfile) {
  MatND hist; // histogram rgb

  int histSize[] = {colorSize, colorSize, colorSize};

  float rranges[] = {0, 256};
  float granges[] = {0, 256};
  float branges[] = {0, 256};

  const float* ranges[] = {rranges, granges, branges};

  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {0, 1, 2};
  //from 256 to 32
  Mat dst = src.clone();
  resizeImageColor(src, dst, colorSize);
  //  cout<<colorSize<<endl;
  //  imshow("image 256 bit", src);
  //  imshow("image 32 bit", dst);
  //  waitKey();
  //  Mat srclab;
  //  cvtColor(dst, srclab, CV_BGR2Lab);

  //calcHist(&srclab, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
  calcHist(&dst, 1, channels, Mat(), hist, 3, histSize, ranges, true, false);

  int nbhist = histNo(hist, 3);
  //  cout << "nb hist" << nbhist << endl;
  normalizeHistogram(hist, nbhist, 3);
  SaveMatrix(hist, outfile, 3);

}

void extractHistoColor(const Mat & src, int colorSize, ostream & outfile) {
  extractHistoColorRGB(src, colorSize, outfile);
  //  extractHistoColorLab(src, colorSize,outfile);
}

void extractTexture(const Mat & src, int graySize, double*** concurrenceArray, ostream & outfile) {
  //  cout << "size=" << graySize << endl;
  Mat srcgray;
  cvtColor(src, srcgray, CV_RGB2GRAY);
  reduireNiveauDeGris(srcgray, graySize);
  //  if (debug) {
  //    cout << "end niveau de gris" << endl;
  //    getchar();
  //  }

  setZero(concurrenceArray, graySize);

  //  if (debug) {
  //  cout << "begin calcul matrix" << endl;
  //    for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
  //      printMatrix(concurrenceArray[matrixIndex], graySize);
  //    }
  //    getchar();
  //  }

  //    double*** matrices = calculerMatrixCooccurence(src, size);
  calculerMatrixCooccurence(srcgray, concurrenceArray, graySize);

  //  if (debug) {
  //  cout << "end calcul matrix" << endl;
  //    getchar();
  //  }




  for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
    //      printMatrix(concurrenceArray[matrixIndex], size);
    vector<double> vvalue;
    extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, &outfile, vvalue);
    //      getchar();
  }
}

void printVector(const vector<double>& vector) {
  for (int i = 0; i < vector.size(); i++) {
    cout << vector[i] << " ";
  }
  cout << endl;
}

void printMatrix(double** dirPath, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      cout << dirPath[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

//void extract(const char* dirPath, const char* name, int graySize, int colorSize) {
//  extraitFeatureVector(dirPath, name, graySize);
//}

void separateCrossTesting(const char* filename, const char* fileLearn, const char* fileTest, int percent) {

  ofstream ofFileLearn;
  ofFileLearn.open(fileLearn);
  ofstream ofFileTest;
  ofFileTest.open(fileTest);
  ifstream ifFile;
  ifFile.open(filename);
  srand(time(NULL));
  while (ifFile.good()) {
    string line;
    getline(ifFile, line);
    if (rand() % 100 < percent) {
      ofFileLearn << line << endl;
    } else {
      ofFileTest << line << endl;
    }
  }
  ofFileTest.close();
  ofFileLearn.close();
  ifFile.close();
}

void dosearchHuMoment(const char* filesearch, const char* fileLearn, int numberNeighbor, const double* huMomentArray,
  int huMomentArraySize, vector<double*>& distanceVector,
  vector<string*>& fileResultVector) {


  double maxKDistance = 1e6;

  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);

  while (ifFileLearn.good()) {
    // load learning vector
    string learnline;
    getline(ifFileLearn, learnline);

    stringstream sslearn(learnline);
    string learnfilename;
    sslearn >> learnfilename;

    if (learnfilename.compare("") == 0)
      break;
    if (learnfilename.compare(filesearch) == 0)
      continue;

    double* huMomentEle = new double[huMomentArraySize];

    for (int i = 0; i < huMomentArraySize; i++) {
      sslearn >> huMomentEle[i];
      //      cout << huMomentEle[i] << " ";
    }
    //    cout << endl;

    // calculate distance between 2 vector
    double dist = getHuMomentVectorDistance(huMomentEle, huMomentArray);

    //    cout << learnfilename << " hu moment distance: " << dist << endl;

    //      cout << "distance:" << dist << endl;
    if (maxKDistance > dist || distanceVector.size() < numberNeighbor) {
      //      cout << dist << "<" << maxKDistance << endl;
      //              cout << "change k=" << k << endl;

      if (distanceVector.size() == 0 || maxKDistance < dist) {
        //        cout << "init to end" << endl;
        distanceVector.push_back(new double(dist));
        fileResultVector.push_back(new string(learnfilename));
        maxKDistance = dist;
        continue;
      }
      vector<double*>::iterator itdistance = distanceVector.begin();
      vector<string*>::iterator itclass = fileResultVector.begin();
      //      cout << "begin for" << (itdistance < distanceVector.end()) << endl;
      for (; itdistance <= distanceVector.end(); ++itdistance, ++itclass) {
        //        cout << "itdistance" << endl;
        if ((**itdistance) > dist) {
          //          cout << "insert" << endl;
          double* a = new double;
          *a = dist;
          distanceVector.insert(itdistance, a);
          fileResultVector.insert(itclass, new string(learnfilename));
          break;
        }
      }
      if (distanceVector.size() >= numberNeighbor) {
        distanceVector.pop_back();
        fileResultVector.pop_back();
        maxKDistance = **(distanceVector.end());
      }
      //        if (itdistance > distanceVector.end()) {
      //          distanceVector.push_back(dist);
      //          classVector.push_back(learnclassname);
      //          maxKDistance = dist;
      //        }

      //        maxKDistance = *(distanceVector.end());
    }
    //      cout << "end" << endl;
    delete [] huMomentEle;

  }


  // show content:
  //  for (it = classCount.begin(); it != classCount.end(); it++)
  //    cout << (*it).first << " => " << (*it).second << endl;
  //
  //  cout << endl;
  ifFileLearn.close();
  //  return maxClass;

}

void dosearch(const char* filesearch, const char* fileLearn, int numberNeighbor, const double* textureArray,
  const double* colorHistoArray, int textureArraySize, int colorHistoSize, vector<double*>& distanceVector,
  vector<string*>& fileResultVector, double colorWeight) {


  double maxKDistance = 1e6;
  double maxKIndex = 0;

  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);



  while (ifFileLearn.good()) {
    // load learning vector
    string learnline;
    getline(ifFileLearn, learnline);

    stringstream sslearn(learnline);
    string learnfilename;
    //    string learnclassname;
    sslearn >> learnfilename;
    //    sslearn >> learnclassname;
    if (learnfilename.compare("") == 0)
      break;
    if (learnfilename.compare(filesearch) == 0)
      continue;

    double* textureEle = new double[textureArraySize];
    double* colorHistoEle = new double[colorHistoSize];
    for (int i = 0; i < textureArraySize; i++) {
      sslearn >> textureEle[i];
    }
    for (int i = 0; i < colorHistoSize; i++) {
      sslearn >> colorHistoEle[i];
    }

    // calculate distance between 2 vector
    double texturedist = getTextureVectorDistance(textureEle, textureArray, textureArraySize);
    double colorHistoDist = getColorHistoDistance(colorHistoEle, colorHistoArray, colorHistoSize);

    double dist = (1 - colorWeight) * texturedist + colorWeight*colorHistoDist;
    //    cout << learnfilename << " texture:" << texturedist << " " << "color:" << colorHistoDist << " total:" << dist << endl;

    //      cout << "distance:" << dist << endl;
    if (maxKDistance > dist || distanceVector.size() < numberNeighbor) {
      //      cout << dist << "<" << maxKDistance << endl;
      //              cout << "change k=" << k << endl;

      if (distanceVector.size() == 0 || maxKDistance < dist) {
        //        cout << "init to end" << endl;
        distanceVector.push_back(new double(dist));
        fileResultVector.push_back(new string(learnfilename));
        maxKDistance = dist;
        continue;
      }
      vector<double*>::iterator itdistance = distanceVector.begin();
      vector<string*>::iterator itclass = fileResultVector.begin();
      //      cout << "begin for" << (itdistance < distanceVector.end()) << endl;
      for (; itdistance <= distanceVector.end(); ++itdistance, ++itclass) {
        //        cout << "itdistance" << endl;
        if ((**itdistance) > dist) {
          //          cout << "insert" << endl;
          double* a = new double;
          *a = dist;
          distanceVector.insert(itdistance, a);
          fileResultVector.insert(itclass, new string(learnfilename));
          break;
        }
      }
      if (distanceVector.size() >= numberNeighbor) {
        distanceVector.pop_back();
        fileResultVector.pop_back();
        maxKDistance = **(distanceVector.end());
      }
      //        if (itdistance > distanceVector.end()) {
      //          distanceVector.push_back(dist);
      //          classVector.push_back(learnclassname);
      //          maxKDistance = dist;
      //        }

      //        maxKDistance = *(distanceVector.end());
    }
    //      cout << "end" << endl;
    delete [] textureEle;
    delete [] colorHistoEle;
  }


  // show content:
  //  for (it = classCount.begin(); it != classCount.end(); it++)
  //    cout << (*it).first << " => " << (*it).second << endl;
  //
  //  cout << endl;
  ifFileLearn.close();
  //  return maxClass;

}

int filename2Id(string filename) {
  string name = string(filename);
  int indexName = name.rfind(".");
  if (indexName < 0)
    return -1;
  string pref = name.substr(0, indexName);
  int id = atoi(pref.c_str());
  //  cout << id << " ";
  return id;
}

int filename2IdCoil_100(string filename) {
  int indexName = filename.find("_");
  string pref = filename.substr(3, indexName);
  int id = atoi(pref.c_str());
  return id;
}

void evalueSearchCoil_100(const char* fileTest, vector<string*>& resultFileVector, int& trueTotal) {

  trueTotal = 0;
  int fileIndex = filename2IdCoil_100(fileTest);


  //  cout << "min-max" << min << " " << max << endl;
  for (int i = 0; i < resultFileVector.size(); i++) {
    int id = filename2IdCoil_100(*resultFileVector[i]);
    if (id == fileIndex) {
      trueTotal++;
    }
  }
}

void evalueSearch(const char* fileRef, const char* fileTest, vector<string*>& resultFileVector, int& trueTotal) {
  ifstream ifFileRef;
  ifFileRef.open(fileRef);
  vector<int> classIndex;
  while (ifFileRef.good()) {
    int value;
    ifFileRef >> value;
    classIndex.push_back(value);
  }

  int fileIndex = filename2Id(fileTest);
  int min, max;
  for (int i = 0; i < classIndex.size() - 1; i++) {
    if (classIndex[i + 1] > fileIndex) {
      min = classIndex[i];
      max = classIndex[i + 1] - 1;
      break;
    }
  }

  //  cout << "min-max" << min << " " << max << endl;
  for (int i = 0; i < resultFileVector.size(); i++) {
    int id = filename2Id(*resultFileVector[i]);
    if (id >= min && id <= max) {
      trueTotal++;
    }
  }
}

void searchHuMoment(const char* fileLearn, const char* fileTest, int k, const char* fileRef, const char* refFileOutDir) {
  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);

  stringstream ssfirst(stringstream::in | stringstream::out);
  ssfirst << firstline;
  string dirPath;
  string name;
  int graySize, colorSize;
  ssfirst >> dirPath;
  ssfirst >> name;
  cout << fileLearn << "-" << dirPath << "-" << name << endl;
  cout << "File test:" << fileTest << endl;

  double* huMomentArray = new double[7];

  while (ifFileLearn.good()) {
    // load testing vector
    string testline;
    getline(ifFileLearn, testline);
    //    cout << testline << endl;

    stringstream sstest(stringstream::in | stringstream::out);
    sstest << testline;
    //    cout << "end init" << endl;
    string testfilename;
    //    string testclassname;
    sstest >> testfilename;
    //    cout << testfilename << endl;

    if (testfilename.compare("") == 0) {
      break;
    }
    if (testfilename.compare(fileTest) != 0) {
      continue;
    }

    cout << "hu momment array test";
    for (int i = 0; i < 7; i++) {
      sstest >> huMomentArray[i];
      //      cout << huMomentArray[i] << " ";
    }

    //    cout << endl;
    //    getchar();
    break;
  }
  ifFileLearn.close();
  //  for (int i = 0; i < 7; i++) {
  //
  //    cout << huMomentArray[i] << " ";
  //  }
  //  cout << endl;
  cout << "end get hu moment" << endl;
  // Search
  vector<double*> distanceVector;
  vector<string*> fileResultVector;
  dosearchHuMoment(fileTest, fileLearn, k, huMomentArray,
    7, distanceVector, fileResultVector);

  cout << "end search" << endl;
  // Print out the result
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    cout << *fileResultVector[i] << " " << *distanceVector[i] << endl;
  }

  //  if (fileRef != NULL) {
  int trueTotal = 0;
  evalueSearchCoil_100(fileTest, fileResultVector, trueTotal);
  cout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;
  //  }

  //  getchar();
  ofstream htmlout;
  char filehtml[255];
  sprintf(filehtml, "%s/%s.%d.hu.html", refFileOutDir, fileTest, k);
  //  cout << filehtml << endl;
  htmlout.open(filehtml);
  //  getchar();
  htmlout << "<image src=\"" << dirPath << "/" << fileTest << "\" width=100 height=100 />" << fileTest << endl;
  htmlout << "<br/>Result Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;
  htmlout << "<br/>" << endl;
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    htmlout << "<div style=\"float:left;margin:2px;\"><image src=\"" << dirPath << "/" << *fileResultVector[i] << "\" width=100 height=100 /><br/>";
    htmlout << *fileResultVector[i] << "<br/>" << *distanceVector[i] << "</div>" << endl;
  }
  htmlout.close();

  freeVector(distanceVector, distanceVector.size());
  freeVector(fileResultVector, fileResultVector.size());
  delete []huMomentArray;

}

void search(const char* fileLearn, const char* fileTest, int k, double colorWeight, const char* fileRef, const char* refFileOutDir) {
  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);
  stringstream ssfirst(stringstream::in | stringstream::out);
  ssfirst << firstline;
  int texture_att_size = 0;
  int color_histo_size = 0;
  string dirPath;
  string name;
  int graySize, colorSize;
  ssfirst >> dirPath;
  ssfirst >> name;
  ssfirst >> graySize;
  ssfirst >> colorSize;
  ssfirst >> texture_att_size;
  ssfirst >> color_histo_size;

  double* textureArray = new double[texture_att_size];
  double* colorHistArray = new double[color_histo_size];

  while (ifFileLearn.good()) {
    // load testing vector
    string testline;
    getline(ifFileLearn, testline);
    //    cout << testline << endl;

    stringstream sstest(stringstream::in | stringstream::out);
    sstest << testline;
    //    cout << "end init" << endl;
    string testfilename;
    //    string testclassname;
    sstest >> testfilename;
    if (testfilename.compare("") == 0) {
      break;
    }
    if (testfilename.compare(fileTest) != 0) {
      continue;
    }

    for (int i = 0; i < texture_att_size; i++) {
      sstest >> textureArray[i];
      cout << textureArray[i] << " ";
    }
    //    cout<<endl;
    for (int i = 0; i < color_histo_size; i++) {
      sstest >> colorHistArray[i];
      //      cout << colorHistArray[i] << " ";

    }
    //    cout<<endl;
    //    getchar();
    break;
  }
  ifFileLearn.close();
  cout << "end get texture and color att" << endl;
  // Search
  vector<double*> distanceVector;
  vector<string*> fileResultVector;
  dosearch(fileTest, fileLearn, k, textureArray,
    colorHistArray, texture_att_size, color_histo_size, distanceVector, fileResultVector, colorWeight);

  cout << "end search" << endl;
  // Print out the result
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    cout << *fileResultVector[i] << " " << *distanceVector[i] << endl;
  }
  int trueTotal = 0;
  if (fileRef != NULL) {
    evalueSearch(fileRef, fileTest, fileResultVector, trueTotal);
    cout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;
  }

  //  getchar();
  ofstream htmlout;
  char filehtml[255];
  sprintf(filehtml, "%s/%s.%d.%d.%d.%d.html", refFileOutDir, fileTest, k, graySize, colorSize, (int) (colorWeight * 100));
  //  cout << filehtml << endl;
  htmlout.open(filehtml);
  //  getchar();
  htmlout << "<image src=\"" << dirPath << "/" << fileTest << "\" width=100 height=100 />" << fileTest << endl;
  htmlout << ". Param: Couleur M :" << colorSize << ", Texture Gris: " << graySize << ", Poids de Couleur:" << colorWeight << ", Nombre de resultat:" << k << endl;
  htmlout << "<br/> Result ";
  htmlout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << "</br>" << endl;
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    htmlout << "<div style=\"float:left;margin:2px;\"><image src=\"" << dirPath << "/" << *fileResultVector[i] << "\" width=100 height=100 /><br/>";
    htmlout << *fileResultVector[i] << "<br/>" << *distanceVector[i] << "</div> " << endl;
  }
  htmlout.close();

  freeVector(distanceVector, distanceVector.size());
  freeVector(fileResultVector, fileResultVector.size());
  delete []textureArray;
  delete []colorHistArray;
}

double getVectorDistance(double* v1, double* v2, int size) {
  double distance = 0;
  // distance eclidienne
  for (int i = 0; i < size; i++) {
    distance += pow(v1[i] - v2[i], 2);
  }
  distance = sqrt(distance);
  // distance cosinus
  //  double v1v2 = 0;
  //  double dv1 = 0;
  //  double dv2 = 0;
  //  for (int i = 0; i < size; i++) {
  //    v1v2 += abs(v1[i] * v2[i]);
  //    dv1 += pow(v1[i], 2);
  //    dv2 += pow(v2[i], 2);
  //  }
  //  distance = 1-(v1v2 / (sqrt(dv1) * sqrt(dv2)));

  return distance;
}

double getHuMomentVectorDistance(const double* learningVector, const double* testingVector) {
  double distance = 0;

  for (int i = 0; i < 7; i++) {
    double ma = ((learningVector[i] >= 0) ? 1 : -1) * log(abs(learningVector[i]));
    double mb = ((testingVector[i] >= 0) ? 1 : -1) * log(abs(testingVector[i]));
    //cout << ma << "-" << mb << endl;
    //    cout << mb << "-" << ma << endl;
    // case 1
    //    if (ma != 0 && mb != 0)
    distance += abs(1 / ma - 1 / mb);
    // case 2
    //    distance += abs(ma-mb);
    // case 3
    //    distance += abs((ma-mb)/ma);
  }
  return distance;
}

double getColorHistoDistance(const double* learningVector, const double* testingVector, int colorSize) {
  double distance;
  //  for (int i=0; i < colorSize; i++) {
  //    distance += abs(learningVector[i] - testingVector[i]);
  //  }

  for (int i = 0; i < colorSize; i++) {
    if (learningVector[i] + testingVector[i] != 0) {
      distance += pow(learningVector[i] - testingVector[i], 2) / (learningVector[i] + testingVector[i]);
    }
  }
  return distance;
}

double getTextureVectorDistanceBipartiMatching(const double* learningVector, const double* testingVector, int textureSize) {
  int numberPara = NUM_MATRIX_ATT;
  double distanceMatching = 0;
  double distance[NUM_MATRIX][NUM_MATRIX];

  double* v1 = new double[numberPara];
  double* v2 = new double[numberPara];

  // calculate distance between each feature vector
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < NUM_MATRIX; j++) {
      for (int k = 0; k < numberPara; k++) {
        v1[k] = learningVector[i * numberPara + k];
        v2[k] = testingVector[j * numberPara + k];
      }
      distance[i][j] = getVectorDistance(v1, v2, numberPara);
      //      cout << distance[i][j] << " ";
    }
    //    cout << endl;
  }
  double assignment[NUM_MATRIX];
  //  double cost;

  assignmentoptimal(assignment, &distanceMatching, (double*) distance, NUM_MATRIX, NUM_MATRIX);

  delete [] v1;
  delete [] v2;
  //  cout << "end assignment " << distanceMatching << endl;
  return distanceMatching;
}

double getTextureVectorDistance(const double* learningVector, const double* testingVector, int textureSize) {

  double distanceMatching = 0;

  double distancePara[NUM_MATRIX_ATT];

  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < NUM_MATRIX_ATT; j++) {
      int index = j * NUM_MATRIX + i;
      //      cout << i<<"*"<<j<<"="<<index << ":" << learningVector[index] << " " << testingVector[index];
      distancePara[i] += abs(((double) (learningVector[index] - testingVector[index])) / testingVector[index]);
    }
  }
  for (int i = 0; i < NUM_MATRIX_ATT; i++) {
    distanceMatching += distancePara[i];
  }
  distanceMatching /= NUM_MATRIX_ATT*NUM_MATRIX;
  //  cout << "end assignment " << distanceMatching << endl;
  return distanceMatching;
}

void reduireNiveauDeGris(Mat& image, int size) {
  double a = (double) size / 256;
  //  cout<<"a="<<a;
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      image.at<uchar > (i, j) = (uchar) (a * image.at<uchar > (i, j));
      //            cout<<(int)image.at<uchar > (i, j)<<" ";
    }
    //        cout<<endl;
  }
  //  getchar();
}

void setZero(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < graySize; j++) {
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }
}

void freeVector(vector<int>** vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
  delete[] vector;
}

void freeVector(vector<double*> vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
}

void freeVector(vector<string*> vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
}

void freeMatrix(double** matrix, int size) {
  for (int i = 0; i < size; i++) {
    delete[] matrix[i];
  }
  delete matrix;
}

void freeMatrix(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {

    for (int j = 0; j < graySize; j++) {
      //      cout<<i<<","<<j<<" ";
      delete[] concurrenceArray[i][j]; // = new double[graySize];
    }
    delete[] concurrenceArray[i]; // = new double*[graySize];
  }
  //  delete[] concurrenceArray;
}
// calculer 8 matrices de co-occurences

void calculerMatrixCooccurence(const Mat& image, double*** concurrenceArray, int graySize) {

  // convertir image a partir de 256 a size
  int deltaX[NUM_MATRIX] = {1, 1, 0, -1, 2, 2, 0, -2};
  int deltaY[NUM_MATRIX] = {0, 1, 1, 1, 0, 2, 2, 2};
  int numberOfPair[NUM_MATRIX] = {0};


  // calculer matrix of concurrence
  //  cout << "begin calculer matrix of concurrence" << endl;
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      for (int k = 0; k < NUM_MATRIX; k++) {
        //        cout << k << " ";
        //        getchar();
        int ia = i + deltaY[k];
        int ja = j + deltaX[k];
        if (ia < image.rows && ja < image.cols) {
          int g1 = image.at<uchar > (i, j);
          int g2 = image.at<uchar > (ia, ja);
          //          cout << g1 << "-" << g2 << " ";getchar();
          concurrenceArray[k][g1][g2]++;
          //          cout << concurrenceArray[k][g1][g2] << " ";getchar();
          numberOfPair[k]++;
        }
      }
    }
  }

  //  cout << "begin divise" << endl;
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    cout << numberOfPair[i] << " ";
  //  }
  //  cout << endl;
  // normalize matrix of concurrence
  for (int i = 0; i < NUM_MATRIX; i++) {
    //    cout<<i<<endl;
    for (int j = 0; j < graySize; j++) {
      //      cout<<j<<endl;
      for (int k = 0; k < graySize; k++) {
        //        cout << concurrenceArray[i][j][k] << " ";
        concurrenceArray[i][j][k] /= numberOfPair[i];

      }
      //      cout << endl;
    }
    //    cout<<endl;
  }
  //  cout << "end divise" << endl;

  //  return concurrenceArray;
}

// calculer 14 parametres
// sauver le vecteur de caracteristique

void extraitCaracteristicVector(double** mat, int size, ostream* ofs, vector<double>& vvalue) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);
  double mean = para_mean(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);

  double sum_avg = para_sum_average(mat, size);

  vvalue.push_back(para_angular_second_moment(mat, size));
  //    vvalue.push_back(para_constrast(mat, size));
  vvalue.push_back(para_entropy(mat, size));
  vvalue.push_back(para_correlation(mat, size, meani, meanj, vari, varj));
  vvalue.push_back(para_dissimilarity(mat, size));
  vvalue.push_back(para_energy(mat, size));
  vvalue.push_back(para_homogenety(mat, size));

  //  vvalue.push_back(sum_avg);
  //  vvalue.push_back(para_sum_entropy(mat, size));
  //  vvalue.push_back(para_sum_of_squares__variance(mat, size, mean));
  //  vvalue.push_back(para_sum_variance(mat, size, sum_avg));
  //  vvalue.push_back(para_difference_entropy(mat, size));
  //  vvalue.push_back(para_difference_variance(mat, size));

  //  vvalue.push_back(para_shade(mat, size, meani, meanj));
  //  vvalue.push_back(para_prominence(mat, size, meani, meanj));

  // normalize value
  double vmean = 0;
  for (int i = 0; i < vvalue.size(); i++) {
    vmean += vvalue[i];
  }
  vmean /= vvalue.size();

  double sd = 0;
  for (int i = 0; i < vvalue.size(); i++) {
    sd += pow(vvalue[i] - vmean, 2);
  }
  sd = sqrt(sd / vvalue.size());


  for (int i = 0; i < vvalue.size(); i++) {
    vvalue[i] = (vvalue[i] - vmean) / sd;
  }

  // output to file
  if (ofs != NULL) {
    for (int i = 0; i < vvalue.size(); i++) {
      (*ofs) << vvalue[i] << " ";
    }
  }
}
//

void loadVector() {

}
//

double para_angular_second_moment(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }
  return value;
}

double para_constrast(double** mat, int size) {
  double value = 0;
  int number_of_gray = size;
  for (int n = 0; n < number_of_gray; n++) {
    float sub = 0;
    for (int i = 0; i < number_of_gray; i++) {
      for (int j = 0; j < number_of_gray; j++) {
        if (abs(i - j) == n) {
          sub += mat[i][j];
        }
      }
    }
    value += n * n*sub;
  }
  return value;
}

double para_mean(double** mat, int size) {
  double mean = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      mean += i * j * mat[i][j];

    }
  }
  return mean;
}

double para_meani(double** mat, int size) {
  // mean
  double meani = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      meani += i * mat[i][j];

    }
  }
  return meani;
}

double para_meanj(double** mat, int size) {
  // mean

  double meanj = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      meanj += j * mat[i][j];
    }
  }
  return meanj;
}

double para_vari(double** mat, int size, double meani) {
  // variance
  double vari = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      vari += mat[i][j] * pow(i - meani, 2);

    }
  }
  return vari;
}

double para_varj(double** mat, int size, double meanj) {
  // variance

  double varj = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      varj += mat[i][j] * pow(j - meanj, 2);
    }
  }
  return varj;
}

double para_correlation(double** mat, int size, double meani, double meanj, double vari, double varj) {

  // correlation
  double corr = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      corr += mat[i][j]*(i - meani)*(j - meanj) / sqrt(vari * varj);
    }
  }
  return corr;
}

double para_correlation(double** mat, int size) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);


  // correlation
  double corr = para_correlation(mat, size, meani, meanj, vari, varj);
  return corr;

}

double para_sum_of_squares__variance(double** mat, int size, double mean) {
  double var = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      var += pow(i - mean, 2) * mat[i][j];

    }
  }
  return var;
}

void para_inverse_difference_moment(double** mat, int size) {

}

double para_sum_average(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    value += k * p_i_add_j;
  }
  return value;
}

double para_sum_variance(double** mat, int size, double sum_e) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    value += pow(k - sum_e, 2) * p_i_add_j;
  }
  return value;
}

double para_sum_entropy(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    if (p_i_add_j != 0)
      value += p_i_add_j * log(p_i_add_j);
  }
  return -value;
}

double para_entropy(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (mat[i][j] != 0)
        value += mat[i][j] * log(mat[i][j]);
    }
  }
  return -value;
}

double para_difference_variance(double** mat, int size) {
  double value = 0;
  double p_i_sub_j = 0;
  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }
    value += pow(k, 2) * p_i_sub_j;
  }
  return value;
}

double para_difference_entropy(double** mat, int size) {
  //int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_sub_j = 0;
  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }
    if (p_i_sub_j != 0)
      value += p_i_sub_j * log(p_i_sub_j);
  }
  return -value;
}

void para_info_measure_of_correlation_1(double** mat, int size) {

}

void para_info_measure_of_correlation_2(double** mat, int size) {

}

void para_max_correlation_coeff(double** mat, int size) {

}

double para_dissimilarity(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += mat[i][j] * abs(i - j);
    }
  }
  return value;
}

double para_energy(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }
  return value;
}

double para_homogenety(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += (1 / (double) (1 + pow((i - j), 2))) * mat[i][j];
    }
  }
  return value;
}

double para_shade(double** mat, int size, int meani, int meanj) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 3) * mat[i][j];
    }
  }
  return value;

}

double para_prominence(double** mat, int size, int meani, int meanj) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 4) * mat[i][j];
    }
  }
  return value;

}

////double test_hung() {
////  while (1) {
////    hungarian_t prob;
////
////    /*
////    int r[4][4] =  {{  363, 626, 376,  46  },
////                    {  802, 993, 784, 953  },
////                    {  673,  23, 823, 207  },
////                    {  380, 451, 520, 646  }};
////     */
////    double* r;
////    int m, n;
////    m = n = 4;
////
////    //  parse_args(argc,argv);
////
////    //  if(!strlen(input_fname))
////    r = make_random_r(m, n);
////    //  else
////    //    r = make_r_from_ORlib(input_fname,&m,&n);
////
////    if (!r) {
////      puts("got NULL input");
////      exit(-1);
////    }
////
////    hungarian_init(&prob, (double*) r, m, n, HUNGARIAN_MIN);
////    hungarian_print_rating(&prob);
////    hungarian_solve(&prob);
////    hungarian_print_assignment(&prob);
////
////    printf("\nfeasible? %s\n",
////      (hungarian_check_feasibility(&prob)) ? "yes" : "no");
////    printf("benefit: %f\n\n", hungarian_benefit(&prob));
////
////    hungarian_fini(&prob);
////    free(r);
////  }
////}
//
//double*
//make_random_r(size_t m, size_t n) {
//  int i, j;
//  double* r;
//  time_t curr;
//  assert(r = (double*) malloc(sizeof (double) *m * n));
//  curr = time(NULL);
//  srand(curr);
//  for (i = 0; i < m; i++) {
//    for (j = 0; j < n; j++) {
//      r[i * n + j] = 1 + rand() / (double) 1000;
//    }
//  }
//  return (r);
//}
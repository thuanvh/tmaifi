
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

#define NUM_DIGIT 2
#define NUM_CLASS 40

void extractSUFTNoDict(const Mat& img, vector<KeyPoint>& allKeyPoints, vector<float>& alldescriptors,
  ostream& featurefile) {
  SURF surf;
  vector<KeyPoint> keypoints;
  vector<float> descriptors;
  surf.hessianThreshold = 0.001;
  surf.nOctaveLayers = 4;
  surf.nOctaves = 4;
  surf.extended = 2;
  surf(img, Mat(), keypoints, descriptors);

  int size = surf.descriptorSize();
  vector<int> ptpairs;
  //  cout<<"size:"<<size<<endl;
  findPairs(keypoints, descriptors, allKeyPoints, alldescriptors, ptpairs, size);

  // Save index to fichier
  for (int i = 0; i < ptpairs.size(); i += 2) {
    if (ptpairs[i + 1] >= 0) {
      featurefile << ptpairs[i + 1] << " ";

    }
  }
}

void extractSUFTDict(const Mat& img, vector<KeyPoint>& allKeyPoints, vector<float>& alldescriptors,
  vector<bool*>& allClassKey, int classKey,
  ostream& featurefile, ostream& dictFile) {
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
  //  if (createDict) {
  for (int i = 0; i < ptpairs.size(); i += 2) {

    if (ptpairs[i + 1] < 0) {
      // add keypoint to dictionary
      allKeyPoints.push_back(keypoints[ptpairs[i]]);
      saveToDict(keypoints[ptpairs[i]], descriptors, dictFile, i / 2 * size, size);
      bool* a = new bool[NUM_CLASS];
      for (int k = 0; k < NUM_CLASS; k++) {
        a[k] = false;
      }
      allClassKey.push_back(a);
      for (int j = 0; j < size; j++) {
        alldescriptors.push_back(descriptors[j + i / 2 * size]);
      }
      ptpairs[i + 1] = allKeyPoints.size() - 1;

    }
  }
  //  }
  // Save index to fichier
  for (int i = 0; i < ptpairs.size(); i += 2) {
    if (ptpairs[i + 1] >= 0) {
      featurefile << ptpairs[i + 1] << " ";
      allClassKey[ptpairs[i + 1]][classKey] = true;
    }
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
  if (dist1 < 0.8 * dist2)
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

void extractAttributes(const char* indexFile, const char* classKeyLookUpFile) {

  // read training image
  ifstream ifimage;
  ifimage.open(indexFile, ios_base::in);

  vector<bool*> classKeyLookUp;
  loadDictLookup(classKeyLookUp, classKeyLookUpFile);

  char arffFile[255];
  sprintf(arffFile, "%s.arff", indexFile);
  ofstream ofdata;
  ofdata.open(arffFile);

  //  cout << "width:" << width << "-" << "height" << height << endl;

  ofdata << "@RELATION digit" << endl;
  for (int i = 0; i < NUM_CLASS; i++) {
    ofdata << "@ATTRIBUTE nb_surfPoint_" << i << "			integer" << endl;
  }
  ofdata << "@ATTRIBUTE class 		{";
  for (int i = 1; i <= NUM_CLASS; i++) {
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
    if (filename.compare("") == 0)
      break;
    string classname;
    ss >> classname;
    //vector<int> vec;
    int list[NUM_CLASS] = {0};
    while (ss.good()) {
      int value;
      ss >> value;
      //vec.push_back(value);
      for (int i = 0; i < NUM_CLASS; i++) {
        list[i] += (classKeyLookUp[value][i] ? 1 : 0);
      }
    }
    for (int i = 0; i < NUM_CLASS; i++) {
      ofdata << list[i] << ",";
    }
    //    for (int i = 0; i < numberFields; i++) {
    //      bool one = false;
    //      for (int j = 0; j < vec.size(); j++) {
    //        if (i == vec[j]) {
    //          ofdata << "1,";
    //          one = true;
    //          break;
    //        }
    //      }
    //      if (!one) {
    //        ofdata << "0,";
    //      }
    //    }
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

  vector<bool* > allClassKey;
  // load dictionary
  loadFromDict(allKeyPoints, alldescriptors, indictfile);

  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;

    if (filename.compare("") == 0)
      break;
//    cout << filename;
    int classKey = atoi(classname.substr(1).c_str()) - 1;

    Mat src;


    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;
    outfile << filename << " ";
    outfile << classname << " ";
//    cout << "begin extract" << classKey << endl;
    extractSUFTNoDict(src, allKeyPoints, alldescriptors, outfile);
//    cout << "end extract" << endl;
//    getchar();
    outfile << endl;
  }
  indictfile.close();
  outfile.close();
  listFileStream.close();
  //  extractAttributes(outFileName, allKeyPoints.size());
  char classKeyLookUpFile[255];
  sprintf(classKeyLookUpFile, "%s.dictlookup", name);

  extractAttributes(outFileName, classKeyLookUpFile);
}

void saveDictLookup(const vector<bool*>& dictlookup, const char* filename) {
  ofstream dictlookupFile;
  dictlookupFile.open(filename);
  for (int i = 0; i < dictlookup.size(); i++) {
    for (int j = 0; j < NUM_CLASS; j++) {
      dictlookupFile << (dictlookup[i][j] ? "1" : "0") << " ";
    }
    dictlookupFile << endl;
  }
  dictlookupFile.close();
}

void loadDictLookup(vector<bool*>& dictlookup, const char* filename) {
  ifstream dictlookupFile;
  dictlookupFile.open(filename);
  while (dictlookupFile.good()) {
    bool* rec = new bool[NUM_CLASS];
    for (int j = 0; j < NUM_CLASS; j++) {
      int value;
      dictlookupFile >> value;
      rec[j] = (value == 0 ? false : true);
      //dictlookupFile << (dictlookup[i] ? "1", "0") << " ";
    }
    dictlookup.push_back(rec);
  }
  dictlookupFile.close();
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
  vector<bool* > allClassKey;
  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;
    if (filename.compare("") == 0)
      break;
    int classKey = atoi(classname.substr(1).c_str()) - 1;

    Mat src;

    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;
    outfile << filename << " ";
    outfile << classname << " ";
    extractSUFTDict(src, allKeyPoints, alldescriptors, allClassKey, classKey, outfile, outdictfile);
    outfile << endl;
  }
  outdictfile.close();
  outfile.close();
  listFileStream.close();

  char classKeyLookUpFile[255];
  sprintf(classKeyLookUpFile, "%s.dictlookup", name);
  saveDictLookup(allClassKey, classKeyLookUpFile);

  //  extractAttributes(outFileName, classKeyLookUpFile, allKeyPoints.size());
  extractAttributes(outFileName, classKeyLookUpFile);
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

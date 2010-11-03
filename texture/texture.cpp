
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

#include "assignmentoptimal.h"
using namespace cv;
using namespace std;

/*
 * Apprendtissage a partir d'une serie d'image de peau
 */
void extraitFeatureVector(const char* dirPath, const char* name, int graySize) {

  //  // load list of file in dir
  //  DIR *dp;
  //  struct dirent *ep;
  //  vector<string> files;
  //  vector<string> classes;
  //
  //  dp = opendir(dirPath);
  //  if (dp != NULL) {
  //    while (ep = readdir(dp)) {
  //      //      puts(ep->d_name);
  //      files.push_back((string) ep->d_name);
  //    }
  //    (void) closedir(dp);
  //  } else
  //    perror(ERR_DIR_OPEN);

  // read list of file and its class
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

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
  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;
    //    files.push_back(filename);
    //    classes.push_back(classname);

    Mat src;
    // calculer list of feature vector
    //    for (int fileindex = 0; fileindex < files.size(); fileindex++) {
    //      string filename = files.at(fileindex);
    if (filename == "")
      continue;
    bool debug = false;
    //    if (classname.compare("weave")==0 || classname.compare("wood")==0)
    //      continue;
    //    if (filename.compare("weave_000_06.pgm") == 0) {
    //      debug = true;
    //    }
    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;

    cout << "size=" << graySize << endl;
    reduireNiveauDeGris(src, graySize);
    if (debug) {
      cout << "end niveau de gris" << endl;
      getchar();
    }

    setZero(concurrenceArray, graySize);

    if (debug) {
      cout << "begin calcul matrix" << endl;
      for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
        printMatrix(concurrenceArray[matrixIndex], graySize);
      }
      getchar();
    }
    try {
      //    double*** matrices = calculerMatrixCooccurence(src, size);
      calculerMatrixCooccurence(src, concurrenceArray, graySize);
    } catch (...) {
      continue;
    }
    if (debug) {
      cout << "end calcul matrix" << endl;
      getchar();
    }
    //    cout << "begin extract" << endl;
    cout << filename << " " << classname << " ";
    outfile << filename << " " << classname << " ";


    for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
      //      printMatrix(concurrenceArray[matrixIndex], size);
      vector<double> vvalue;
      extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, &outfile, vvalue);
      //      getchar();
    }

    outfile << endl;
    //    cout << "end extract" << endl;
    //    }
  }
  listFileStream.close();
  cout << "free memory" << endl;
  freeMatrix(concurrenceArray, graySize);
  outfile.close();
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

void learning(const char* dirPath, const char* name, int graySize) {
  extraitFeatureVector(dirPath, name, graySize);
}

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

void crossTesting(const char* filename, int percent, int k) {
  char fileLearn[255];
  sprintf(fileLearn, "%s.%d.learn", filename, percent);
  char fileTest[255];
  sprintf(fileTest, "%s.%d.test", filename, percent);
  separateCrossTesting(filename, fileLearn, fileTest, percent);

  testing(fileLearn, fileTest, k);
}

string classifier(const char* fileLearn, int numberNeighbor, const vector<double>& testingVector, double* distance = NULL, double* percent = NULL) {
  vector<double> distanceVector;
  vector<string> classVector;

  double maxKDistance = 1000;
  double maxKIndex = 0;

  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);
  while (ifFileLearn.good()) {
    // load learning vector
    string learnline;
    getline(ifFileLearn, learnline);

    stringstream sslearn(learnline);
    string learnfilename;
    string learnclassname;
    sslearn >> learnfilename;
    sslearn >> learnclassname;
    if (learnfilename.compare("") == 0)
      break;
    vector<double> learningVector;
    //      cout << learnfilename << " " << learnclassname << endl;
    //      getchar();
    double learnvalue;
    while (sslearn >> learnvalue) {
      learningVector.push_back(learnvalue);
    }

    // calculate distance between 2 vector
    double dist = getTextureVectorDistance(learningVector, testingVector);

    //      cout << "distance:" << dist << endl;
    if (maxKDistance > dist || distanceVector.size() < numberNeighbor) {
      //        cout << dist << "<" << maxKDistance << endl;
      //        cout << "change k=" << k << endl;

      if (distanceVector.size() == 0 || maxKDistance < dist) {
        //          cout << "init to end" << endl;
        distanceVector.push_back(dist);
        classVector.push_back(learnclassname);
        maxKDistance = dist;
        continue;
      }
      vector<double>::iterator itdistance = distanceVector.begin();
      vector<string>::iterator itclass = classVector.begin();
      //        cout << "begin for" << (itdistance < distanceVector.end()) << endl;
      for (; itdistance <= distanceVector.end(); ++itdistance, ++itclass) {
        //          cout << "itdistance" << endl;
        if ((*itdistance) > dist) {
          //            cout << "insert" << endl;
          distanceVector.insert(itdistance, dist);
          classVector.insert(itclass, learnclassname);
          break;
        }
      }
      if (distanceVector.size() >= numberNeighbor) {
        distanceVector.pop_back();
        classVector.pop_back();
        maxKDistance = *(distanceVector.end());
      }
      //        if (itdistance > distanceVector.end()) {
      //          distanceVector.push_back(dist);
      //          classVector.push_back(learnclassname);
      //          maxKDistance = dist;
      //        }

      //        maxKDistance = *(distanceVector.end());
    }
    //      cout << "end" << endl;
  }
  // count class
  map<string, int> classCount;
  int maxCount = 0;
  string maxClass;
  if (distance != NULL) *distance = 1000;
  for (int i = 0; i < classVector.size(); i++) {
    classCount[classVector[i]] = classCount[classVector[i]] + 1;
    if (maxCount < classCount[classVector[i]]) {
      maxClass = classVector[i];
      maxCount = classCount[classVector[i]];
      if (distance != NULL) {
        if (distanceVector[i]<*distance) {
          *distance = distanceVector[i];
        }
      }
    }
    //    cout << classVector[i] << " " << distanceVector[i] << " ";
  }
  map<string, int>::iterator it;

  if (percent != NULL)
    *percent = maxCount / (double) numberNeighbor;

  // show content:
  //  for (it = classCount.begin(); it != classCount.end(); it++)
  //    cout << (*it).first << " => " << (*it).second << endl;
  //
  //  cout << endl;
  ifFileLearn.close();
  return maxClass;
}

void testing(const char* fileLearn, const char* fileTest, int k) {
  ifstream ifFileTest;
  ifFileTest.open(fileTest);
  int rightClass = 0;
  int falseClass = 0;
  while (ifFileTest.good()) {
    // load testing vector
    string testline;
    getline(ifFileTest, testline);
    //    cout << testline << endl;

    stringstream sstest(stringstream::in | stringstream::out);
    sstest << testline;
    //    cout << "end init" << endl;
    string testfilename;
    string testclassname;
    sstest >> testfilename;
    if (testfilename.compare("") == 0) {
      break;
    }

    sstest >> testclassname;
    cout << testfilename << " " << testclassname << endl;

    vector<double> testingVector;

    double testvalue;
    while (sstest.good()) {
      //      cout<<" digit"<<endl;
      sstest >> testvalue;
      testingVector.push_back(testvalue);
      //      cout << testvalue;
    }

    string maxClass = classifier(fileLearn, k, testingVector);

    cout << maxClass << " vs " << testclassname << endl;
    if (maxClass.compare(testclassname) == 0) {
      rightClass++;
    } else {
      falseClass++;
    }

  }
  cout << "Result: Right:" << rightClass << " False:" << falseClass << " Percent:" << ((float) rightClass / (rightClass + falseClass)) << endl;
  ifFileTest.close();
}

void segmenterCAH(const char* fileImage, const char* name, int graySize, int numberGroup, int segmblocksize) {
  // load file
  Mat img;
  if (!(img = imread(fileImage, 0)).data)
    return;
  reduireNiveauDeGris(img, graySize);

  Mat segmentImage = img.clone();
  segmentImage.setTo(Scalar(0, 0, 0));

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

  int row = img.rows / segmblocksize;
  int col = img.cols / segmblocksize;
  vector<double>*** vvalueTotal = new vector<double>**[row];
  for (int i = 0; i < row; i++) {
    vvalueTotal[i] = new vector<double>*[col];
    for (int j = 0; j < col; j++) {
      vvalueTotal[i][j] = NULL;
    }
  }
  Mat region;
  int halfsize = segmblocksize / 2;

  vector<double>* vvalue;
  for (int j = 0; j < img.rows; j += segmblocksize) {
    for (int k = 0; k < img.cols; k += segmblocksize) {
      //      cout << j << " - " << k;
      getRectSubPix(img, Size(segmblocksize, segmblocksize), Point2f(j + halfsize, k + halfsize), region);
      //        imshow("region", region);

      //      Mat trackImg = img.clone();
      //      rectangle(trackImg, Point(i, j), Point(i + segmblocksize, j + segmblocksize), Scalar(255, 0, 0));
      //        imshow("track image", trackImg);

      // get vector value of region
      //      Mat segmentSubRegion;
      //      getRectSubPix(segmentImage, Size(segmblocksize, segmblocksize), Point2f(j + halfsize, k + halfsize), segmentSubRegion);
      //        double max;
      //        minMaxLoc(segmentSubRegion, NULL, &max);
      //        if (max != 0) {
      //          cout << "no ";
      //          continue;
      //        }
      setZero(concurrenceArray, graySize);

      calculerMatrixCooccurence(region, concurrenceArray, graySize);

      vvalue = new vector<double>();

      for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
        extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, NULL, *vvalue);
      }
      //      cout << j / segmblocksize << "-" << k / segmblocksize << endl;
      vvalueTotal[j / segmblocksize][k / segmblocksize] = vvalue;
    }
  }
  //  getchar();
  int nbsur = row * col;
  vector<int>** vectorGroup = new vector<int>*[nbsur];
  double** distanceMatrix = new double*[nbsur];
  for (int i = 0; i < nbsur; i++) {
    distanceMatrix[i] = new double[nbsur];
    vectorGroup[i] = new vector<int>;
    for (int j = 0; j < nbsur; j++)
      distanceMatrix[i][j] = 0;
  }
  int distanceRow = 0;
  int distanceCol = 0;
  // calculer matrix of distance
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
//      cout << i << "-" << j << ": ";
      //      getchar();
      vector<double>* vvalue = vvalueTotal[i][j];
      //      printVector(*vvalue);
      int k = i, l = (j + 1) % col;
      if (l == 0) k = i + 1;
      distanceCol = distanceRow + 1;
      for (; k < row; k++, l = 0) {
        for (; l < col; l++) {
          //          cout << k << ":" << l << " ";
          vector<double>* vvalue2 = vvalueTotal[k][l];
          //          printVector(*vvalue2);
          double distance = getTextureVectorDistance(*vvalue, *vvalue2);
          //          cout << "a;ljfpoqiuwepoiquriop ";
          //          if(j==1)getchar();
          distanceMatrix[distanceRow][distanceCol] = distance;
          distanceMatrix[distanceCol][distanceRow] = distance;
          distanceCol++;
          //          cout << distance << " ";
          //          if(j==1)getchar();
        }
      }

      //      getchar();
      //            cout << endl;
      vectorGroup[distanceRow]->push_back(i);
      vectorGroup[distanceRow]->push_back(j);
      distanceRow++;

      //      cout << "end all" << endl;
    }
  }
//  cout << "end calcul distance" << endl;
//  getchar();
  int groupNumber = nbsur;

  while (groupNumber > numberGroup) {
//    cout << "Group: " << groupNumber<<endl;
//    for (int i = 0; i < groupNumber; i++) {
//      for (int j = 0; j < groupNumber; j++) {
//        cout << distanceMatrix[i][j] << "\t";
//      }
//      cout << endl;
//    }
//
//    for (int i = 0; i < groupNumber; i++) {
//      cout << "G[" << i << "]=";
//      for (int list = 0; list < vectorGroup[i]->size(); list += 2) {
//        int x = (*vectorGroup[i])[list];
//        int y = (*vectorGroup[i])[list + 1];
//        cout << "(" << x << "," << y << ")";
//      }
//      cout << endl;
//    }
    // find value minimal
    double minValue = 1e6;
    int minRow = 0;
    int minCol = 0;
    for (int i = 0; i < groupNumber; i++) {
      for (int j = 0; j < groupNumber; j++) {
        if (i == j) continue;
        if (minValue > distanceMatrix[i][j]) {
          minValue = distanceMatrix[i][j];
          minRow = i;
          minCol = j;
        }
      }
    }
//    cout << "min:" << minValue << " at " << minRow << "," << minCol << endl;
    // create new matrix
//    cout << "create new matrix" << endl;
    double** newMatrix = new double*[groupNumber - 1];
    vector<int>** newVectorGroup = new vector<int>*[groupNumber - 1];
    for (int i = 0; i < groupNumber - 1; i++) {
      newMatrix[i] = new double[groupNumber - 1];
      newVectorGroup[i] = new vector<int>;
      for (int j = 0; j < groupNumber - 1; j++) {
        newMatrix[i][j] = 0;
      }
    }
    // assign to new matrix
//    cout << "assign to new matrix" << endl;
    //    getchar();
    int newRow = 0, newCol = 0;
    for (int i = 0; i < groupNumber; i++, newRow++) {
      if (i == minRow || i == minCol) {
        newRow--;
        continue;
      }
      newCol = 0;
      for (int j = 0; j < groupNumber; j++, newCol++) {
        if (j == minCol || j == minRow) {
          newCol--;
          continue;
        }
        //        cout<<newRow<<"-"<<newCol<<" ";
        newMatrix[newRow][newCol] = distanceMatrix[i][j];
      }
    }
//    cout << "calcule new group distance" << endl;
    //    getchar();
    for (int i = 0, newRow = 0; i < groupNumber; i++, newRow++) {
      if (i == minRow || i == minCol) {
        newRow--;
        continue;
      }
      newMatrix[newRow][groupNumber - 2] =
        (distanceMatrix[i][minRow] > distanceMatrix[i][minCol] ? distanceMatrix[i][minRow] : distanceMatrix[i][minCol]);
      newMatrix[groupNumber - 2][newRow] = newMatrix[newRow][groupNumber - 2];
      for (int k = 0; k < vectorGroup[i]->size(); k++) {
        newVectorGroup[newRow]->push_back((*vectorGroup[i])[k]);
      }
    }

//    cout << "calcule new vectorgroup" << endl;
    //    getchar();
    for (int i = 0; i < vectorGroup[minRow]->size(); i++) {
      newVectorGroup[groupNumber - 2]->push_back((*vectorGroup[minRow])[i]);
    }
    for (int i = 0; i < vectorGroup[minCol]->size(); i++) {
      newVectorGroup[groupNumber - 2]->push_back((*vectorGroup[minCol])[i]);
    }

//    cout << "free memory" << endl;
    freeMatrix(distanceMatrix, groupNumber);
    distanceMatrix = newMatrix;
    freeVector(vectorGroup, groupNumber);
    vectorGroup = newVectorGroup;

    groupNumber--;
    
  }

  for (int i = 0; i < groupNumber; i++) {
    cout << "G[" << i << "]=";
    for (int list = 0; list < vectorGroup[i]->size(); list += 2) {
      int x = (*vectorGroup[i])[list];
      int y = (*vectorGroup[i])[list + 1];
      cout << "(" << x << "," << y << ")";
      rectangle(segmentImage, Rect(x*segmblocksize, y*segmblocksize, segmblocksize, segmblocksize), Scalar(i), CV_FILLED);
    }
  }
  cout<<endl;
//  for(int i=0; i<segmentImage.rows; i++){
//    for(int j=0; j<segmentImage.cols; j++){
//      cout<<(int)segmentImage.at<uchar>(i,j);
//    }
//    cout<<endl;
//  }
  Mat matzero = segmentImage.clone();
  matzero.setTo(Scalar(0));
  scaleAdd(segmentImage, 255 / groupNumber, matzero, segmentImage);
  //  imshow("source image", img);
//  imshow("segment image", segmentImage);
//  waitKey();
  char fileOutName[255];
  sprintf(fileOutName, "out/%s.segmentCAH.%d.%d.%d.jpg", fileImage, graySize, numberGroup, segmblocksize);
  cout << fileOutName << endl;

  imwrite(fileOutName, segmentImage);
  segmentImage.release();
  img.release();
  freeMatrix(concurrenceArray, graySize);
  freeMatrix(distanceMatrix, groupNumber);

  freeVector(vectorGroup, groupNumber);
}

void segmenter(const char* fileImage, const char* name, int graySize, int numberNeighbor, int segmblocksize) {
#define NUM_OF_SCALE 1
  int size[NUM_OF_SCALE] = {segmblocksize};
  // load file
  Mat img;
  if (!(img = imread(fileImage, 0)).data)
    return;
  reduireNiveauDeGris(img, graySize);

  Mat segmentImage = img.clone();
  segmentImage.setTo(Scalar(0, 0, 0));

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

  map<string, int> classMap;
  // diviser
  for (int i = 0; i < NUM_OF_SCALE; i++) {
    Mat region;
    int halfsize = size[i] / 2;
    for (int j = 0; j < img.rows; j += size[i]) {
      for (int k = 0; k < img.cols; k += size[i]) {
        cout << j << " - " << k << endl;
        getRectSubPix(img, Size(size[i], size[i]), Point2f(j + halfsize, k + halfsize), region);
        //        imshow("region", region);

        //        Mat trackImg = img.clone();
        //        rectangle(trackImg, Point(i, j), Point(i + size[i], j + size[i]), Scalar(255, 0, 0));
        //        imshow("track image", trackImg);

        // get vector value of region
        //        Mat segmentSubRegion;
        //        getRectSubPix(segmentImage, Size(size[i], size[i]), Point2f(j + halfsize, k + halfsize), segmentSubRegion);
        //        double max;
        //        minMaxLoc(segmentSubRegion, NULL, &max);
        //        if (max != 0) {
        //          cout << "no ";
        //          continue;
        //        }
        setZero(concurrenceArray, graySize);

        calculerMatrixCooccurence(region, concurrenceArray, graySize);

        vector<double> vvalue;

        for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
          extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, NULL, vvalue);
        }
        double percent;
        double distance;
        string classname = classifier(name, numberNeighbor, vvalue, &distance, &percent);
        cout << "distance:" << distance << " percent:" << percent << endl;
        //        if (percent < 0.5)
        //          continue;
        if (classMap[classname] == 0) {
          classMap[classname] = classMap.size() + 1;
        }

        rectangle(segmentImage, Rect(j, k, size[i], size[i]), Scalar(classMap[classname]), CV_FILLED);
        cout << "class: " << classMap[classname] << " " << classname << endl;

        //        getchar();
        //        waitKey();
      }
    }
  }
  Mat matzero = segmentImage.clone();
  matzero.setTo(Scalar(0));
  scaleAdd(segmentImage, 255 / classMap.size(), matzero, segmentImage);
  //  imshow("source image", img);
  //  imshow("segment image", segmentImage);
  //  waitKey();
  char fileOutName[255];
  sprintf(fileOutName, "out/%s.segment.%d.%d.%d.jpg", fileImage, graySize, numberNeighbor, segmblocksize);
  cout << fileOutName << endl;

  imwrite(fileOutName, segmentImage);
  segmentImage.release();
  img.release();
  freeMatrix(concurrenceArray, graySize);
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

double getTextureVectorDistance(const vector<double>& learningVector, const vector<double>& testingVector) {
  int numberPara = learningVector.size() / NUM_MATRIX;
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
  //  cout << "end assignment" << endl;
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    cout << i << "+" << assignment[i] << " ";
  //  }
  //  cout<<endl<<distanceMatching<<endl;
  //  test_hung();
  //  int intDistance[NUM_MATRIX][NUM_MATRIX];
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    for (int j = 0; j < NUM_MATRIX; j++) {
  //      intDistance[i][j] = (int) (distance[i][j]*10000);
  //      cout<<intDistance[i][j]<<" ";
  //    }
  //    cout<<endl;
  //  }

  //
  //  hungarian_t prob;
  //  hungarian_init(&prob, (double*) distance, NUM_MATRIX, NUM_MATRIX, HUNGARIAN_MIN);
  //  hungarian_print_rating(&prob);
  //  hungarian_solve(&prob);
  //  hungarian_print_assignment(&prob);
  //  for (int i = 0; i < prob.m; i++) {
  //    distanceMatching += distance[i][prob.a[i]];
  //  }
  //  //  getchar();
  //
  //  //  printf("\nfeasible? %s\n",
  //  //         (hungarian_check_feasibility(&prob)) ? "yes" : "no");
  //  //  printf("benefit: %d\n\n", hungarian_benefit(&prob));
  //
  //  hungarian_fini(&prob);
  delete [] v1;
  delete [] v2;
  //  cout << "end assignment " << distanceMatching << endl;
  return distanceMatching;
}

void reduireNiveauDeGris(Mat& image, int size) {
  double a = (double) size / 256;
  //  cout<<"a="<<a;
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      image.at<uchar > (i, j) = (uchar) (a * image.at<uchar > (i, j));
      //      cout<<(int)image.at<uchar > (i, j)<<" ";
    }
    //    cout<<endl;
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

  //  // initialize list of matrix
  //  double*** concurrenceArray = new double**[NUM_MATRIX];
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    concurrenceArray[i] = new double*[graySize];
  //    for (int j = 0; j < graySize; j++) {
  //      concurrenceArray[i][j] = new double[graySize];
  //      for (int k = 0; k < graySize; k++) {
  //        concurrenceArray[i][j][k] = 0;
  //      }
  //    }
  //  }

  //  cout << "begin start" << endl;
  // calculer matrix of concurrence
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      for (int k = 0; k < NUM_MATRIX; k++) {
        int ia = i + deltaY[k];
        int ja = j + deltaX[k];
        if (ia < image.rows && ja < image.cols) {
          int g1 = image.at<uchar > (i, j);
          int g2 = image.at<uchar > (ia, ja);
          //          cout<<g1<<"-"<<g2<<" ";
          concurrenceArray[k][g1][g2]++;
          //          cout<<concurrenceArray[k][g1][g2]<<" ";
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
  vvalue.push_back(para_constrast(mat, size));
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
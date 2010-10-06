
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
#include "digitdetectionMahalanobis.h"
#include <vector>
using namespace std;
using namespace cv;

#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define NUM_DIGIT 10

void learningMahalanobis(const char* trainingimage, const char* traininglabel, const char* outputDirPath) {
  // read training image
  ifstream ifimage;
  ifimage.open(trainingimage, ios_base::in);
  // read training label
  ifstream iflabel;
  iflabel.open(traininglabel, ios_base::in);

  string imageline;
  string labelvalue;

//  // histogram array
//  float*** histos = new float**[NUM_DIGIT];
//
//  for (int i = 0; i < NUM_DIGIT; i++) {
//    histos[i] = new float*[IMAGE_HEIGHT];
//    for (int j = 0; j < IMAGE_HEIGHT; j++) {
//      histos[i][j] = new float[IMAGE_WIDTH]; //{0};
//      for (int k = 0; k < IMAGE_WIDTH; k++) {
//        histos[i][j][k] = 0;
//      }
//    }
//  }
  int learningImageNumber[NUM_DIGIT] = {0};
  
  vector<Mat>  matarray[NUM_DIGIT];
  Mat matmeanarray[NUM_DIGIT];
  bool matmeancreate[NUM_DIGIT]={false};

  cout<<"begin reading"<<endl;
  while (iflabel.good())
  {
    getline(iflabel, labelvalue);
//    cout<<labelvalue<<endl;
    int label = atoi(labelvalue.c_str());
//    cout << label;
    if (label >= 0 && label < NUM_DIGIT) {
      Mat mat;
      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);
      
      //
      if(!matmeancreate[label]){
//        cout<<"create mean array"<<endl;
        matmeanarray[label].create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_32F);
        matmeancreate[label]=true;
        matmeanarray[label].zeros(IMAGE_WIDTH, IMAGE_HEIGHT, CV_32F);
      }
      learningImageNumber[label]++;
      Mat mean=matmeanarray[label];
      // Read image
      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
//        cout<<imageline<<endl;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          mean.at<float>(i,j) += (imageline[j] == ' ') ? 0 : 255;
//          cout<<mean.at<float>(i,j);
          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 0 : 255;
          //mat.setTo(a);
        }
//        cout<<endl;
      }
      matarray[label].push_back(mat);
//      getchar();
    }
  }

  // calcul mean
  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
      for (int j = 0; j < IMAGE_WIDTH; j++) {
//        cout<<matmeanarray[histIndex].at<float>(i,j)<<" ";
        matmeanarray[histIndex].at<float>(i,j) /= (float) learningImageNumber[histIndex];
      }
//      cout<<endl;
    }
//    cout<<learningImageNumber[histIndex]<<endl<<endl;
//    getchar();
  }

  for(int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    int size=matarray[histIndex].size();
    Mat* mat=new Mat[size];
    for(int i=0; i<size; i++){
      mat[i]=matarray[histIndex][i];
    }
    Mat cov;
    Mat mean=matmeanarray[histIndex];

    char meanfileout[255];
    sprintf(meanfileout,"%s/%d.mean",outputDirPath,histIndex);
    SaveMatrix(mean,meanfileout);
    

    calcCovarMatrix(mat,size,cov,mean,CV_COVAR_NORMAL);

    Mat invertCov;
    invert(cov,invertCov);

    char covfileout[255];
    sprintf(covfileout,"%s/%d.invcov",outputDirPath,histIndex);
    SaveMatrix(invertCov,covfileout);

    

//    char name[255];
//    sprintf(name, "histo%d", histIndex);
//    showMatrixMean(mean, name, outputDirPath);

    cov.release();
    invertCov.release();
    mean.release();
  }

  
//  // save histogram
//  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
//    char fileout[255];
//    sprintf(fileout, "%s/%d.histo", outputDirPath, histIndex);
//    //string a = string(outputDirPath) + "/" + histIndex + ".histo";
//    SaveHistograme(histos[histIndex], fileout);
//  }
//  // show histogram
//  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
//    char name[255];
//    sprintf(name, "histo%d", histIndex);
//    showHistogram(histos[histIndex], name, outputDirPath);
//  }

  //ifs.read()
  // Release image
  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
    int size=matarray[histIndex].size();
    for(int i=0; i<size; i++){
      Mat mat=matarray[histIndex][i];
      mat.release();
    }
    //matarray[histIndex]
  }

}

void showMatrixMean(Mat& mean, const char* histoname, const char* outputDirPath) {
  int scale = 5;
  Mat histImg = Mat::zeros(IMAGE_WIDTH * scale, IMAGE_HEIGHT * scale, CV_8U);
  //  int total = 0;
  cout << endl;
  for (int h = 0; h < IMAGE_HEIGHT; h++) {
    for (int w = 0; w < IMAGE_WIDTH; w++) {
      float binVal = mean.at<float>(h,w);

      //      total += binVal;
      int intensity = cvRound(binVal );
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
  sprintf(file, "%s/%s.mean.png", outputDirPath, histoname);
  imwrite(file, histImg);
  waitKey();
  histImg.release();
}

void SaveMatrix(Mat mat,const  char* matname) {
  ofstream ofs;
  ofs.open(matname);
  ofs<<mat.rows<<endl;
  ofs<<mat.cols<<endl;
  for (int a = 0; a < mat.rows; a++) {
    for (int b = 0; b < mat.cols; b++) {
      ofs<<mat.at<float>(a,b)<<" ";
    }
    ofs << endl;
  }
  ofs.close();
}

void LoadMatrix(Mat& mat, const char* name) {
  ifstream ifs;
  ifs.open(name);
  int rows=0;
  ifs>>rows;
  int cols=0;
  ifs>>cols;

  mat.create(rows,cols,CV_32F);
  for (int a = 0; a < rows; a++) {
    for (int b = 0; b < cols; b++) {
      float value = 0;
      ifs >> mat.at<float>(a,b);
      //cout << hist[a][b];
    }
    //cout << endl;
  }
  ifs.close();

}

void TestingMahalanobis(const char* histoDirPath,const  char* testImagePath, const char* testLabelPath) {
  Mat invcovArray[10];
  Mat meanArray[10];
  // load invert covariance
  cout << "loading invert covariance" << endl;
  for (int i = 0; i < NUM_DIGIT; i++) {
    char filename[255];
    sprintf(filename, "%s/%d.invcov", histoDirPath, i);
    //filename += string(histoDirPath) + "/" + i + ".histo";
    LoadMatrix(invcovArray[i], filename);

    char meanfname[255];
    sprintf(meanfname, "%s/%d.mean", histoDirPath, i);
    LoadMatrix(meanArray[i],meanfname);
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
      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_32F);

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
          mat.at<float > (i, j) = (imageline[j] == ' ') ? 0 : 255;
          //mat.setTo(Scalar((imageline[j] == ' ') ? 255 : 0));
        }
      }

      // Evaluer Bayes
      int digitChoose = 0;
      float valueMax = 0;
      for (int digit = 0; digit < NUM_DIGIT; digit++) {
//        cout<<mat.type()<<meanArray[digit].type()<<invcovArray[digit].type()<<":";
//        cout<<mat.size().height<<meanArray[digit].size().height<<invcovArray[digit].size().height<<endl;
//        cout<<mat.size().width<<meanArray[digit].size().width<<invcovArray[digit].size().width<<endl;
        float prob = Mahalanobis(mat,meanArray[digit],invcovArray[digit]);
        cout << digit << ":" << prob << endl;
        if (valueMax < prob) {
          valueMax = prob;
          digitChoose = digit;
        }
      }
      total++;
      if (label != digitChoose) {
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



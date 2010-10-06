
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
#include "digitdetectionpoint.h"
#include "skeletonization.h"
#include <opencv/ml.h>
using namespace std;
using namespace cv;

#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define NUM_DIGIT 10

void extractData(const char* imagein, const char* labelin, const char* dataoutfile) {
  // read training image
  ifstream ifimage;
  ifimage.open(imagein, ios_base::in);
  // read training label
  ifstream iflabel;
  iflabel.open(labelin, ios_base::in);
  //  cout<<"begin read";

  ofstream ofdata;
  ofdata.open(dataoutfile);
  //  cout<<"wrote";

  ofdata << "@RELATION digit" << endl;
  ofdata << "@ATTRIBUTE number_ending_point			integer" << endl;
  ofdata << "@ATTRIBUTE number_intersect_point 		integer" << endl;
  ofdata << "@ATTRIBUTE number_ending_point_q1 		integer" << endl;
  ofdata << "@ATTRIBUTE number_ending_point_q2 		integer" << endl;
  ofdata << "@ATTRIBUTE number_ending_point_q3 		integer" << endl;
  ofdata << "@ATTRIBUTE number_ending_point_q4 		integer" << endl;
  ofdata << "@ATTRIBUTE class 		{0,1,2,3,4,5,6,7,8,9}	" << endl;
  ofdata << "@DATA" << endl;

  string imageline;
  string labelvalue;

  while (iflabel.good()) {
    getline(iflabel, labelvalue);
    int label = atoi(labelvalue.c_str());
    //    cout << label;

    if (label >= 0 && label < NUM_DIGIT) {
      Mat mat;
      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);

      //      float** img;

      for (int i = 0; i < IMAGE_HEIGHT; i++) {
        getline(ifimage, imageline);
        //cout<<imageline<<endl;
        for (int j = 0; j < IMAGE_WIDTH; j++) {
          //          img[i][j] += (imageline[j] == ' ') ? 0 : 1;
          //cout<<img[i][j];
          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
          //mat.setTo(a);
        }
        //cout<<endl;
      }

      // show image
      imshow("Learning Digit", mat);
      char file[255];
      sprintf(file, "%d.png", label);
      imwrite(file, mat);

      Mat outmat2 = mat.clone();
      //      Mat mat2 = mat.clone();



      //      Mat outmat = mat.clone();
      //      cvRasterSkeleton(outmat);
      //Skeleton8bits(outmat, outmat2);
      threshold(outmat2, outmat2, 0, 255, CV_THRESH_BINARY_INV);
      ThinImage(outmat2);
      bitwise_not(outmat2, outmat2);
      char file2[255];
      sprintf(file2, "%d.thin.png", label);
      imwrite(file2, outmat2);
      threshold(outmat2, outmat2, 0, 255, CV_THRESH_BINARY);
      //dilate(mat2, outmat2, convolution2, Point(-1, -1), 1, BORDER_DEFAULT);
      //      imshow("Skeleton Digit", outmat2);

      descriptor des;
      calculDesPointDeBout(outmat2, des);
      //      cout << "No_endingpoint:" << label << ":" << des.nb_endingpoint << endl;
      //      cout << "nb_intersection:" << label << ":" << des.nb_intersection << endl;
      //      for(int i=0; i<4 ; i++) cout << des.nb_endingpoint_quartier[i];


      ofdata << des.nb_endingpoint << "\t" << des.nb_intersection << "\t";
      for (int i = 0; i < 4; i++) ofdata << des.nb_endingpoint_quartier[i] << "\t";
      ofdata << label << endl;

      //      erode(outmat, outmat, convolution);
      //      imshow("Erosion digit", outmat);
      //      waitKey();
      mat.release();
      outmat2.release();
    }
  }
  ofdata.close();
}

void learningPoint(const char* trainingimage, const char* traininglabel, const char* outputDirPath) {
  cout << trainingimage << endl;
  char filepath[255];
  sprintf(filepath, "%s/learning.arff", outputDirPath);
  cout << filepath;
  //  getchar();
  extractData(trainingimage, traininglabel, filepath);
  //  // read training image
  //  ifstream ifimage;
  //  ifimage.open(trainingimage, ios_base::in);
  //  // read training label
  //  ifstream iflabel;
  //  iflabel.open(traininglabel, ios_base::in);
  //
  //  string imageline;
  //  string labelvalue;
  //
  ////  // histogram array
  ////  float*** histos = new float**[NUM_DIGIT];
  ////
  ////  for (int i = 0; i < NUM_DIGIT; i++) {
  ////    histos[i] = new float*[IMAGE_HEIGHT];
  ////    for (int j = 0; j < IMAGE_HEIGHT; j++) {
  ////      histos[i][j] = new float[IMAGE_WIDTH]; //{0};
  ////      for (int k = 0; k < IMAGE_WIDTH; k++) {
  ////        histos[i][j][k] = 0;
  ////      }
  ////    }
  ////  }
  ////  int learningImageNumber[NUM_DIGIT] = {0};
  ////
  ////  descriptor destotal[NUM_DIGIT];
  //
  //  while (iflabel.good()) {
  //    getline(iflabel, labelvalue);
  //    int label = atoi(labelvalue.c_str());
  //    cout << label;
  //    if (label >= 0 && label < NUM_DIGIT) {
  //      Mat mat;
  //      mat.create(IMAGE_WIDTH, IMAGE_HEIGHT, CV_8U);
  //
  //      float** img;
  ////      img = histos[label];
  ////
  ////      learningImageNumber[label]++;
  //      // Read image
  //      for (int i = 0; i < IMAGE_HEIGHT; i++) {
  //        getline(ifimage, imageline);
  //        //cout<<imageline<<endl;
  //        for (int j = 0; j < IMAGE_WIDTH; j++) {
  //          img[i][j] += (imageline[j] == ' ') ? 0 : 1;
  //          //cout<<img[i][j];
  //          //Scalar_<double> a=Scalar::all((imageline[j] == ' ') ? 255 : 0);
  //          mat.at<uchar > (i, j) = (imageline[j] == ' ') ? 255 : 0;
  //          //mat.setTo(a);
  //        }
  //        //cout<<endl;
  //      }
  //
  ////      Mat convolution;
  ////      convolution.create(3, 3, CV_8U);
  ////
  ////      uchar cvArrayClose[9] = {
  ////        1, 1, 1,
  ////        0, 1, 0,
  ////        0, 0, 0
  ////      };
  ////      convolution.data = cvArrayClose;
  ////      uchar cvArrayClose2[9] = {
  ////        0, 0, 0,
  ////        0, 1, 0,
  ////        0, 0, 0
  ////      };
  ////      Mat convolution2;
  ////      convolution2.create(3, 3, CV_8U);
  ////      convolution2.data = cvArrayClose2;
  //
  //
  //      // show image
  //      imshow("Learning Digit", mat);
  //
  //
  //      Mat outmat2 = mat.clone();
  //      Mat mat2 = mat.clone();
  //      //erode(outmat2, mat, convolution, Point(-1, -1), 1, BORDER_REPLICATE);
  //      //dilate(mat2, mat, convolution, Point(-1, -1), 1, BORDER_CONSTANT);
  //
  //      //      imshow("Dilation digit", outmat2);
  //
  //
  //
  //      Mat outmat = mat.clone();
  //      //      cvRasterSkeleton(outmat);
  //      //Skeleton8bits(outmat, outmat2);
  //      threshold(outmat2, outmat2, 0, 255, CV_THRESH_BINARY_INV);
  //      ThinImage(outmat2);
  //      bitwise_not(outmat2, outmat2);
  //      threshold(outmat2, outmat2, 0, 255, CV_THRESH_BINARY);
  //      //dilate(mat2, outmat2, convolution2, Point(-1, -1), 1, BORDER_DEFAULT);
  //      imshow("Skeleton Digit", outmat2);
  //
  //      descriptor des;
  //      calculDesPointDeBout(outmat2, des);
  //      cout << "No_endingpoint:" << label << ":" << des.nb_endingpoint << endl;
  //      cout << "nb_intersection:" << label << ":" << des.nb_intersection << endl;
  //      for(int i=0; i<4 ; i++) cout << des.nb_endingpoint_quartier[i];
  //
  //
  //
  //
  //      //      erode(outmat, outmat, convolution);
  //      //      imshow("Erosion digit", outmat);
  //      waitKey();
  //      mat.release();
  //      outmat2.release();
  //    }
  //  }
  //
  //
  ////  // normalize histogram
  ////  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
  ////    for (int i = 0; i < IMAGE_HEIGHT; i++) {
  ////      for (int j = 0; j < IMAGE_WIDTH; j++) {
  ////        histos[histIndex][i][j] /= (float) learningImageNumber[histIndex];
  ////      }
  ////    }
  ////  }
  ////  // save histogram
  ////  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
  ////    char fileout[255];
  ////    sprintf(fileout, "%s/%d.histo", outputDirPath, histIndex);
  ////    //string a = string(outputDirPath) + "/" + histIndex + ".histo";
  ////    SavePoint(histos[histIndex], fileout);
  ////  }
  ////  // show histogram
  ////  for (int histIndex = 0; histIndex < NUM_DIGIT; histIndex++) {
  ////    char name[255];
  ////    sprintf(name, "histo%d", histIndex);
  ////    showPoint(histos[histIndex], name, outputDirPath);
  ////  }
  //
  //  //ifs.read()
}

int c255to0(int a) {
  return a == 0 ? 1 : 0;
}

void calculDesPointDeBout(const Mat& img, descriptor& des) {

  des.nb_endingpoint = 0;
  des.nb_intersection = 0;
  des.nb_endingpoint_quartier[0] = des.nb_endingpoint_quartier[1] = des.nb_endingpoint_quartier[2] = des.nb_endingpoint_quartier[3] = 0;
  //  for (int i = 1; i < img.rows - 1; i++) {
  //    for (int j = 1; j <= img.cols - 1; j++) {
  //      cout << c255to0(img.at<uchar > (i, j));
  //    }
  //    cout << endl;
  //  }
  int xmean = img.cols / 2;
  int ymean = img.rows / 2;
  for (int i = 1; i < img.rows - 1; i++) {
    for (int j = 1; j <= img.cols - 1; j++) {
      if (img.at<uchar > (i, j) == 0) {
        int a =
          abs(c255to0(img.at<uchar > (i, j + 1)) - c255to0(img.at<uchar > (i - 1, j + 1)))
          + abs(c255to0(img.at<uchar > (i - 1, j + 1)) - c255to0(img.at<uchar > (i - 1, j)))
          + abs(c255to0(img.at<uchar > (i - 1, j)) - c255to0(img.at<uchar > (i - 1, j - 1)))
          + abs(c255to0(img.at<uchar > (i - 1, j - 1)) - c255to0(img.at<uchar > (i, j - 1)))
          + abs(c255to0(img.at<uchar > (i, j - 1)) - c255to0(img.at<uchar > (i + 1, j - 1)))
          + abs(c255to0(img.at<uchar > (i + 1, j - 1)) - c255to0(img.at<uchar > (i + 1, j)))
          + abs(c255to0(img.at<uchar > (i + 1, j)) - c255to0(img.at<uchar > (i + 1, j + 1)))
          + abs(c255to0(img.at<uchar > (i + 1, j + 1)) - c255to0(img.at<uchar > (i, j + 1)))
          ;
        int b = c255to0(img.at<uchar > (i - 1, j + 1))
          + c255to0(img.at<uchar > (i - 1, j))
          + c255to0(img.at<uchar > (i - 1, j - 1))
          + c255to0(img.at<uchar > (i, j - 1))
          + c255to0(img.at<uchar > (i + 1, j - 1))
          + c255to0(img.at<uchar > (i + 1, j))
          + c255to0(img.at<uchar > (i + 1, j + 1))
          + c255to0(img.at<uchar > (i, j + 1))
          ;
        //        cout << a << ":" << b << "(" << i << "," << j << ") ";
        if (a == 2) {
          des.nb_endingpoint++;
          if (i < ymean && j < xmean) {
            des.nb_endingpoint_quartier[1]++;
          } else if (i < ymean && j > xmean) {
            des.nb_endingpoint_quartier[0]++;
          } else if (i > ymean && j < xmean) {
            des.nb_endingpoint_quartier[2]++;
          } else if (i > ymean && j > xmean) {
            des.nb_endingpoint_quartier[3]++;
          }
        }
        if (a== 6) {
          des.nb_intersection++;
        }
      }
    }
  }

}

void showPoint(float** hist, const char* histoname, const char* outputDirPath) {
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

void SavePoint(float** hist, char* name) {
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

void LoadPoint(float** hist, const char* name) {
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

void LoadPoint(const char* file, vector<descriptor*>& desarray) {
  ifstream datafile;
  datafile.open(file);
  //  vector<descriptor*> desarray;
  while (datafile.good() && !datafile.eof()) {
    descriptor* des = new descriptor();
    datafile >> des->nb_endingpoint;
    datafile >> des->nb_intersection;
    for (int i = 0; i < 4; i++)
      datafile >> des->nb_endingpoint_quartier[i];
    datafile >> des->label;
    desarray.push_back(des);
    //    cout<<desarray.size()<<" ";
  }
  datafile.close();
  //  cout<<"end"<<endl;

  //  return desarray;
  //  delete desarray;
}

void deleteVector(vector<descriptor*>& delvector) {
  for (int i = 0; i < delvector.size(); i++) {
    delete delvector[i];
  }
}

void Vector2Mat(const vector<descriptor*>& desarray, Mat& mat, Mat& result) {
  for (int i = 0; i < desarray.size(); i++) {
    mat.at<float>(i, 0) = (float) desarray[i]->nb_endingpoint;
    mat.at<float>(i, 1) = (float) desarray[i]->nb_intersection;
    for (int j = 0; j < 4; j++)
      mat.at<float>(i, 2 + j) = (float) desarray[i]->nb_endingpoint_quartier[j];
    result.at<float>(i, 6) = (float) desarray[i]->label;
    //    cout<<i<<" ";
  }
}

void Vector2Mat(const vector<descriptor>& desarray, Mat& mat, Mat& result) {
  for (int i = 0; i < desarray.size(); i++) {
    mat.at<float>(i, 0) = (float) desarray[i].nb_endingpoint;
    mat.at<float>(i, 1) = (float) desarray[i].nb_intersection;
    for (int j = 0; j < 4; j++)
      mat.at<float>(i, 2 + j) = (float) desarray[i].nb_endingpoint_quartier[j];
    result.at<float>(i, 6) = (float) desarray[i].label;
  }
}

void Vector2IplImage(const vector<descriptor>& desarray, IplImage* mat, IplImage* result) {
  for (int i = 0; i < desarray.size(); i++) {
    CV_IMAGE_ELEM(mat, float, i, 0) = (float) desarray[i].nb_endingpoint;
    CV_IMAGE_ELEM(mat, float, i, 1) = (float) desarray[i].nb_intersection;
    for (int j = 0; j < 4; j++)
      CV_IMAGE_ELEM(mat, float, i, 2 + j) = (float) desarray[i].nb_endingpoint_quartier[j];
    CV_IMAGE_ELEM(mat, float, i, 6) = (float) desarray[i].label;
  }
}

void TestingPoint(char* outputDirPath, char* testImagePath, const char* testLabelPath) {
  char filepath[255];
  sprintf(filepath, "%s/testing.arff", outputDirPath);
  extractData(testImagePath, testLabelPath, filepath);

  //  char trainingpath[255];
  //  sprintf(trainingpath,"%s/learning.data",outputDirPath);
  //  Mat training;
  //  Mat response;
  //  vector<descriptor*> desarray;
  //  LoadPoint(trainingpath,desarray);
  //  training.create(desarray.size(),7,CV_32FC1);
  //  response.create(desarray.size(),1,CV_32FC1);
  ////  training.flags=training.MAGIC_VAL;
  ////  IplImage* training=cvCreateImage(cvSize(desarray.size(),7),1,CV_32FC1);
  ////  IplImage* response=cvCreateImage(cvSize(desarray.size(),1),1,CV_32FC1);
  ////  ((CvMat*)(training))->cols=7;
  ////  ((CvMat*)(training))->rows=desarray.size();
  ////  CvMat mat;
  //
  ////  response.create(desarray.size(),1,CV_32FC1);
  ////  getchar();
  //  Vector2Mat(desarray,training,response);
  ////Vector2IplImage(desarray,training,response);
  ////  cout<<"e"<<endl;
  ////  getchar();
  //
  ////  deleteVector(desarray);
  ////  for(int i=0; i<desarray.size(); i++){
  ////    delete desarray[i];
  ////  }
  //
  //  Mat testing;
  //  Mat trueresult;
  ////  LoadPoint(testImagePath,testing,trueresult);
  //
  //
  //
  //  CvMat cvtraining=cvMat(training.rows,training.cols,CV_32FC1,training.data);
  ////  cvtraining=training;
  ////  cvtraining.data.fl=(float*)training.data;
  ////
  //  CvMat cvresponse=cvMat(response.rows,response.cols,CV_32FC1,response.data);
  ////  cvresponse=response;
  ////  cvresponse.data.fl=(float*)response.data;
  //  CvNormalBayesClassifier nbc;
  //
  //  //nbc.train(cvtraining,cvresponse);
  ////  cout<<CV_IS_MAT_HDR(&cvtraining)<<endl;
  ////  cout<<((((const CvMat*)(&cvtraining))->type & CV_MAGIC_MASK) == CV_MAT_MAGIC_VAL)<<endl;
  ////  cout<<(((const CvMat*)(&cvtraining))->cols)<< ((const CvMat*)(&cvtraining))->rows ;
  ////  cout<<((const CvMat*)(&cvtraining))->type<<endl;
  ////  getchar();
  //  nbc.train(&cvtraining,&cvresponse);
  ////  training.release();
  ////  response.release();
  ////  getchar();
  //
  //  cout<<"begin loading file 2";
  //  getchar();
  //  vector<descriptor*> desarray2;
  //  LoadPoint(filepath,desarray2);
  //
  //  cout<<"end loading file 2";
  //  getchar();
  //
  //  testing.create(desarray2.size(),7,CV_32FC1);
  //  trueresult.create(desarray2.size(),1,CV_32FC1);
  ////  cvMat
  //  Vector2Mat(desarray2,testing,trueresult);
  //  deleteVector(desarray2);
  //
  //  Mat result;
  //  nbc.predict(testing,&result);


}

float calculPointProb(int** img, float** hist) {
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


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "skindetector.h"
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <fstream>

using namespace std;
using namespace cv;
double totalpixel=0;

/*
 * Apprendtissage a partir d'une serie d'image de peau
 */
void Learning(const char* dirPath, const char* histoName) {

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

  //return;
  //  for (int q = 0; q < files.size(); q++) {
  //    string filePath = files.at(q);
  //    cout << filePath << endl;
  //  }
  MatND histSkin; // histogram
  MatND histNoSkin; // histogram noskin
  int HistSize = 32; // size of histogram
  // let's quantize the a to 32 levels
  // and the b to 32 levels
  int abins = HistSize, bbins = HistSize;
  int histSize[] = {abins, bbins};
  // hue varies from 0 to 179, see cvtColor
  float aranges[] = {-256, 256};
  float branges[] = {-256, 256};
  const float* ranges[] = {aranges, branges};

  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {1, 2};
  for (int i = 0; i < files.size(); i++) {
    string filename = files.at(i);
    int indexName = filename.rfind("noskin.");
    if (indexName < 0)
      continue;



    string filePathNoSkin = string(dirPath);
    filePathNoSkin += "/" + filename;
    //    cout<<filePathNoSkin<<endl;

    string filenameSrc = filename.replace(indexName, 7, "");
    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filenameSrc;
    //    cout<<filePathSrc<<endl;
    Mat src;
    if (!(src = imread(filePathSrc, 1)).data)
      continue;

    Mat noskin;
    if (!(noskin = imread(filePathNoSkin, 1)).data)
      continue;
    Mat maskSkin;
    Mat maskNoSkin;

    Mat noskinGray;
    cvtColor(noskin, noskinGray, CV_RGB2GRAY);
    imshow("image noskin gray", noskinGray);
    Mat maskGray;

    threshold(noskinGray, maskGray, 0, 255, THRESH_BINARY);
    imshow("mask image noskin gray", maskGray);
    {
      for (int i = 0; i < noskinGray.rows; i++) {
        for (int j = 0; j < noskinGray.cols; j++) {
          maskGray.at<uchar > (i, j) = noskinGray.at<uchar > (i, j) > 0 ? 255 : 0;
        }
      }
    }
    imshow("mask image noskin gray manual", maskGray);

    Mat noskinGray3;
    cvtColor(noskinGray, noskinGray3, CV_GRAY2RGB);
    imshow("image noskin gray 3 channels", noskinGray3);

    threshold(noskinGray3, maskSkin, 0, 255, THRESH_BINARY);
    threshold(noskinGray3, maskNoSkin, 0, 255, THRESH_BINARY_INV);
    imshow("noskin", noskin);
    imshow("image skin", maskSkin);
    imshow("image noskin", maskNoSkin);


    Mat srcSkin;
    Mat srcNoSkin;
    bitwise_and(src, maskSkin, srcSkin);
    bitwise_and(src, maskNoSkin, srcNoSkin);
    //namedWindow("H-S Histogram", 1);
    imshow("source image skin", srcSkin);
    imshow("source image noskin", srcNoSkin);
    //    waitKey();
    //convertScaleAbs(src,src,32/255,0);
    cout << i << "rows:" << src.rows << " cols:" << src.cols << " numofPoint:" << src.cols * src.rows << endl;
    totalpixel += src.cols * src.rows;
    Mat labSkin;
    Mat labNoSkin;

    MatND histSkinEle;
    MatND histNoSkinEle;
    histSkinEle.create(2,histSize,CV_32F);
    histNoSkinEle.create(2,histSize,CV_32F);
    histSkinEle.setTo(Scalar::all(0));
    histNoSkinEle.setTo(Scalar::all(0));

    cvtColor(srcSkin, labSkin, CV_BGR2Lab);
    cvtColor(srcNoSkin, labNoSkin, CV_BGR2Lab);

    calcHist(&labNoSkin, 1, channels, Mat(), // do not use mask
      histNoSkinEle, 2, histSize, ranges,
      true, // the histogram is uniform
      false);


    calcHist(&labSkin, 1, channels, Mat(), // do not use mask
      histSkinEle, 2, histSize, ranges,
      true, // the histogram is uniform
      false);

    displayHistogram(histNoSkinEle, NULL);
    displayHistogram(histSkinEle, NULL);

    add(histSkinEle, histSkin, histSkin);
    add(histNoSkinEle, histNoSkin, histNoSkin);

    double maxVal = 0;
    minMaxLoc(histSkin, 0, &maxVal, 0, 0);
    cout << "max:" << maxVal << endl;

    minMaxLoc(histNoSkin, 0, &maxVal, 0, 0);
    cout << "max:" << maxVal << endl;

//    waitKey();
    //    break;
  }

  normalizeHistogram(histSkin);
  normalizeHistogram(histNoSkin);

  displayHistogram(histSkin, "histogram skin");
  displayHistogram(histNoSkin, "histogram noskin");

  char filename[255];
  sprintf(filename, "%s.skin", histoName);
  SaveHistograme(histSkin, filename);
  sprintf(filename, "%s.noskin", histoName);
  SaveHistograme(histNoSkin, filename);

  waitKey();
}

void normalizeHistogram(MatND& hist) {
  // normalize histogram
  //  double maxVal = 0;
  //  minMaxLoc(hist, 0, &maxVal, 0, 0);
  //  //  normalize(hist,1);
  //  for (int a = 0; a < hist.size[0]; a++)
  //    for (int b = 0; b < hist.size[1]; b++) {
  //      hist.at<double>(a, b) /= maxVal;
  //    }

  //norm(hist);

  //totalpixel += src.cols * src.rows;

  cout<<totalpixel;
  for (int a = 0; a < hist.size[0]; a++)
    for (int b = 0; b < hist.size[1]; b++) {
      hist.at<double>(a, b) /= totalpixel;
    }
}

void displayHistogram(const MatND& hist, const char* name) {
  // display histogram
  int scale = 10;
  Mat histImg = Mat::zeros(hist.size[0] * scale, hist.size[1] * scale, CV_8UC3);
  //  int total = 0;
  cout << endl;
  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      double binVal = hist.at<double>(a, b);
      cout << binVal << " ";
      //      total += binVal;
      int intensity = cvRound(binVal * 255);
      rectangle(histImg, Point(a*scale, b * scale),
        Point((a + 1) * scale - 1, (b + 1) * scale - 1),
        Scalar::all(intensity),
        CV_FILLED);
    }
    cout << endl;
  }
  cout << endl;
  //cout << "total point" << total << endl;
  //  namedWindow("Source", 1);
  //  imshow("Source", src);
  if (name != NULL) {
    namedWindow(name, 1);
    imshow(name, histImg);
  }
}

void SaveHistograme(const MatND& hist, const char* name) {
  ofstream ofs;
  ofs.open(name);
  ofs << hist.size[0] << endl;
  ofs << hist.size[1] << endl;
  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      ofs << hist.at<float>(a, b) << " ";
    }
    ofs << endl;
  }
  ofs.close();
}

void LoadHistograme(MatND& hist, string name) {
  LoadHistograme(hist, name.c_str());
}

void LoadHistograme(MatND& hist, const char* name) {
  ifstream ifs;
  ifs.open(name);
  int asize = 0;
  ifs >> asize;
  int bsize = 0;
  ifs >> bsize;
  //  cout << asize << " " << bsize << endl;
  int sizes[] = {asize, bsize};
  hist.create(2, sizes, CV_32F);
  hist = Scalar(0);


  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      float value = 0;
      ifs >> hist.at<float>(a, b);
      //      cout << hist.at<float>(a, b);
    }
  }
  ifs.close();

}

/*
 * Chercher le peau dans l'image
 */
void Testing(const char* testPath, const char* histoName) {

  MatND histNoSkin;
  MatND histSkin;

  LoadHistograme(histNoSkin, string(histoName) + ".noskin");
  LoadHistograme(histSkin, string(histoName) + ".skin");

  displayHistogram(histNoSkin, "hist noskin");
  displayHistogram(histSkin, "hist skin");

  Mat src;
  if (!(src = imread(testPath, 1)).data)
    return;
  //    Mat maskSkin;
  //    Mat maskNoSkin;
  //    threshold(noskin, maskSkin, 0, 255, THRESH_BINARY);
  //    threshold(noskin, maskNoSkin, 0, 255, THRESH_BINARY_INV);
  //    //    imshow("noskin",noskin);
  //    //    imshow("image skin", maskSkin);
  //    //    imshow("image noskin",maskNoSkin);

  //    Mat srcSkin;
  //    Mat srcNoSkin;
  //    bitwise_and(src, maskSkin, srcSkin);
  //    bitwise_and(src, maskNoSkin, srcNoSkin);
  //    //namedWindow("H-S Histogram", 1);
  //    imshow("image skin", srcSkin);
  //    imshow("image noskin", srcNoSkin);
  //    waitKey();
  //    //convertScaleAbs(src,src,32/255,0);
  imshow("image source", src);
  cout << "rows:" << src.rows << " cols:" << src.cols << " numofPoint:" << src.cols * src.rows << endl;
  Mat labSkin;

  cvtColor(src, labSkin, CV_BGR2Lab);

  typedef Vec<uchar, 3 > VT;
  typedef Vec<int, 3 > VTLab;
  MatIterator_<VT> it = src.begin<VT > (),
    it_end = src.end<VT > ();
  MatIterator_<VT> itlab = labSkin.begin<VT > (),
    itlab_end = labSkin.end<VT > ();
  for (; it != it_end; ++it, ++itlab) {
    VTLab labval = *itlab;
    VT srcval = *it;
    int a = labval[1];
    int b = labval[2];
    float noskin = histNoSkin.at<float>(a, b);
    float skin = histSkin.at<float>(a, b);
    if (noskin >= skin) {
      *it = VT(0, 0, 0);
    }
  }


  //  for (int i = 0; i < labSkin.rows; i++) {
  //    for (int j = 0; j < labSkin.cols; j++) {
  //      int l = labSkin.at<int>(j * labSkin.channels(), i);
  //      int a = labSkin.at<int>(j * labSkin.channels() + 1, i);
  //      int b = labSkin.at<int>(j * labSkin.channels() + 2, i);
  //      cout << l << " " << a << " " << b << " ";
  //      float noskin = histNoSkin.at<float>(a, b);
  //      float skin = histSkin.at<float>(a, b);
  //      if (noskin >= skin) {
  //        src.at<int>(j * src.channels(), i) = 0;
  //        src.at<int>(j * src.channels() + 1, i) = 0;
  //        src.at<int>(j * src.channels() + 2, i) = 0;
  //      }
  //    }
  //  }
  imshow("image skin", src);
  waitKey();
}

void HistogrammePeau(int a, int b) {

}

void HistogrammeNonPeau(int a, int b) {

}

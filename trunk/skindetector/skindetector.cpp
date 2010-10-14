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
long totalpixel = 0;

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
  int HistSize = num_color; // size of histogram
  // let's quantize the a to 32 levels
  // and the b to 32 levels
  int abins = HistSize, bbins = HistSize;
  int histSize[] = {abins, bbins};

  float aranges[] = {0, 256};
  float branges[] = {0, 256};
  const float* ranges[] = {aranges, branges};

  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {1, 2};
  for (int i = 0; i < files.size(); i++) {
    string filename = files.at(i);
    string filenamecp = string(filename);
    int indexName = filename.rfind("noskin.");
    if (indexName < 0)
      continue;



    string filePathNoSkin = string(dirPath);
    filePathNoSkin += "/" + filename;
    //    cout<<filePathNoSkin<<endl;

    string filenameSrc = filename.replace(indexName, 7, "");
    string filenameSkin = filenamecp.replace(indexName, 7, "skin.");

    cout << filenameSkin << endl;
    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filenameSrc;
    string filePathSkin = string(dirPath);
    filePathSkin += "/" + filenameSkin;
    cout << filePathSkin << endl;
    cout << filePathSrc << endl;
    Mat src;
    if (!(src = imread(filePathSrc, 1)).data)
      continue;

    Mat noskin;
    if (!(noskin = imread(filePathNoSkin, 1)).data)
      continue;

    Mat skin;
    if (!(skin = imread(filePathSkin, 1)).data)
      continue;

    Mat maskSkin;
    Mat maskNoSkin;

    Mat noskinGray;
    Mat skinGray;
    cvtColor(noskin, noskinGray, CV_RGB2GRAY);
    cvtColor(skin, skinGray, CV_RGB2GRAY);
    //    imshow("image noskin gray", noskinGray);
    //    imshow("image skin gray", skinGray);

    Mat maskGrayNoSkin;
    Mat maskGraySkin;
    threshold(noskinGray, maskGrayNoSkin, 0, 255, THRESH_BINARY);
    threshold(skinGray, maskGraySkin, 0, 255, THRESH_BINARY);
    //bitwise_not(maskGrayNoSkin, maskGraySkin);
    //    imshow("mask image noskin gray", maskGrayNoSkin);
    //    imshow("mask image skin gray", maskGraySkin);
    //    waitKey();
    //    {
    //      for (int i = 0; i < noskinGray.rows; i++) {
    //        for (int j = 0; j < noskinGray.cols; j++) {
    //          maskGray.at<uchar > (i, j) = noskinGray.at<uchar > (i, j) > 0 ? 255 : 0;
    //        }
    //      }
    //    }
    //    imshow("mask image noskin gray manual", maskGray);

    //    Mat noskinGray3;
    //    cvtColor(noskinGray, noskinGray3, CV_GRAY2RGB);
    //    imshow("image noskin gray 3 channels", noskinGray3);
    //
    //    threshold(noskinGray3, maskSkin, 0, 255, THRESH_BINARY);
    //    threshold(noskinGray3, maskNoSkin, 0, 255, THRESH_BINARY_INV);
    //    imshow("noskin", noskin);
    //    imshow("image skin", maskSkin);
    //    imshow("image noskin", maskNoSkin);
    //
    //
    //    Mat srcSkin;
    //    Mat srcNoSkin;
    //    bitwise_and(src, maskSkin, srcSkin);
    //    bitwise_and(src, maskNoSkin, srcNoSkin);
    //    //namedWindow("H-S Histogram", 1);
    //    imshow("source image skin", srcSkin);
    //    imshow("source image noskin", srcNoSkin);
    //    //    waitKey();
    //    //convertScaleAbs(src,src,32/255,0);
    //    cout << i << "rows:" << src.rows << " cols:" << src.cols << " numofPoint:" << src.cols * src.rows << endl;
    //    totalpixel += src.cols * src.rows;
    Mat labSkin;
    Mat labNoSkin;

    MatND histSkinEle;
    MatND histNoSkinEle;
    //    histSkinEle.create(2, histSize, CV_32S);
    //    histNoSkinEle.create(2, histSize, CV_32S);
    //    histSkinEle.setTo(Scalar::all(0));
    //    histNoSkinEle.setTo(Scalar::all(0));

    cvtColor(skin, labSkin, CV_BGR2Lab);
    cvtColor(noskin, labNoSkin, CV_BGR2Lab);

    //from 256 to 32
    Mat dst = src.clone();
    img256To32(dst, src);
    //    convertScaleAbs(src,src,1/8,0);
    //    convertScaleAbs(src,src,8,0);
    imshow("image 32 bit", src);

    Mat srclab;
    cvtColor(src, srclab, CV_BGR2Lab);

    calcHist(&srclab, 1, channels, maskGrayNoSkin, histNoSkinEle, 2, histSize, ranges, true, false);
    calcHist(&srclab, 1, channels, maskGraySkin, histSkinEle, 2, histSize, ranges, true, false);

    //    calcHist(&labNoSkin, 1, channels, Mat(), // do not use mask
    //      histNoSkinEle, 2, histSize, ranges,
    //      true, // the histogram is uniform
    //      false);
    //    calcHist(&labSkin, 1, channels, Mat(), // do not use mask
    //      histSkinEle, 2, histSize, ranges,
    //      true, // the histogram is uniform
    //      false);

    int nbhistnoskin = histNo(histNoSkinEle);
    int nbhistskin = histNo(histSkinEle);

    cout << "Hist point number:" << nbhistnoskin << "+" << nbhistskin << "=" << nbhistnoskin + nbhistskin << endl;
    totalpixel += nbhistnoskin + nbhistskin;

    //    displayHistogram(histNoSkinEle, NULL);
    //    displayHistogram(histSkinEle, NULL);

    add(histSkinEle, histSkin, histSkin);
    add(histNoSkinEle, histNoSkin, histNoSkin);

    displayHistogram(histNoSkinEle, NULL);
    displayHistogram(histSkinEle, NULL);

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

  //#define GNUPLOT_MATRIX "splot '%s' matrix using (($1-32)*255/32):(($2-32)*255/32):3 title '%s' with pm3d"
#define GNUPLOT_MATRIX "splot '%s' matrix using 1:2:3 title '%s' with pm3d"
  //    gnuplot("set palette gray");
  gnuplot("set pm3d explicit");


  sprintf(filename, "%s.skin.splot", histoName);
  SaveGnuPlotMatrix(histSkin, filename);
  char plotcmd[255];
  sprintf(plotcmd, GNUPLOT_MATRIX, filename, "Skin Histogram");
  gnuplot(plotcmd);

  sprintf(filename, "%s.noskin.splot", histoName);
  SaveGnuPlotMatrix(histNoSkin, filename);
  //  char plotcmd[255];
  sprintf(plotcmd, GNUPLOT_MATRIX, filename, "No Skin Histogram");
  gnuplot(plotcmd);

  //  waitKey();
}

int histNo(const MatND& hist) {
  int histno = 0;
  for (int a = 0; a < hist.size[0]; a++)
    for (int b = 0; b < hist.size[1]; b++) {
      histno += hist.at<float> (a, b);
    }
  return histno;
}
long pixtotal = 0;

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

  cout << totalpixel;

  //hist.type();
  for (int a = 0; a < hist.size[0]; a++)
    for (int b = 0; b < hist.size[1]; b++) {
      pixtotal += hist.at<float>(a, b);
      hist.at<float>(a, b) /= totalpixel;
    }
  cout << " compare: " << pixtotal << "-" << totalpixel << "=" << pixtotal - totalpixel << endl;
}

void displayHistogram(const MatND& hist, const char* name) {
  // display histogram
  int scale = 10;
  Mat histImg = Mat::zeros(hist.size[0] * scale, hist.size[1] * scale, CV_8UC3);
  //  int total = 0;
  cout << endl;
  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      float binVal = hist.at<float>(a, b);
      //      if(binVal<0)
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
  //  if (name != NULL) {
  //    namedWindow(name, 1);
  //    imshow(name, histImg);
  //  }
}

void SaveGnuPlotMatrix(const MatND& hist, const char* name) {
  ofstream ofs;
  ofs.open(name);
  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      ofs << hist.at<float>(a, b) << " ";
    }
    ofs << endl;
  }
  ofs.close();
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
      //      float value = 0;
      ifs >> hist.at<float>(a, b);
      //      cout << hist.at<float>(a, b);
    }
  }
  ifs.close();

}

float SumHistogram(const MatND& hist) {
  float sum = 0;
  for (int a = 0; a < hist.size[0]; a++) {
    for (int b = 0; b < hist.size[1]; b++) {
      sum += hist.at<float>(a, b);
    }
  }
  return sum;
}

/*
 * Chercher le peau dans l'image
 */
void Testing(const char* testPath, const char* histoName, const char* fileOut, float thres = 0.5, const char* refRefFilePath = NULL) {

  MatND histNoSkin;
  MatND histSkin;

  LoadHistograme(histNoSkin, string(histoName) + ".noskin");
  LoadHistograme(histSkin, string(histoName) + ".skin");

  //  cout<<"negatif:"<<histNoSkin.at<float>(135, 141)<<endl;
  //
  //  displayHistogram(histNoSkin, "hist noskin");
  //  displayHistogram(histSkin, "hist skin");
  //
  //  cout<<"negatif:"<<histNoSkin.at<float>(135, 141)<<endl;

  Scalar p_skin = sum(histSkin);
  cout << "p_skin:" << p_skin[0] << endl;
  Scalar p_noskin = sum(histNoSkin);
  cout << "p_noskin:" << p_noskin[0] << endl;
  //  displayHistogram(histNoSkin, "hist noskin");

  Mat ref;

  int peauPixTotal = 0;
  int peauPixCorrect = 0;
  bool refcompare=false;
  if (refRefFilePath != NULL) {

    if ((!(ref = imread(refRefFilePath, 1)).data)) {
      refcompare=false;
    }else{
      refcompare=true;
    }
  }

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
  MatIterator_<VT> itref;
  if (refcompare) {
    itref = ref.begin<VT > ();
  }
  MatIterator_<VT> it = src.begin<VT > (),
    it_end = src.end<VT > ();
  MatIterator_<VT> itlab = labSkin.begin<VT > (),
    itlab_end = labSkin.end<VT > ();
  //  cout<<"negatif:"<<histNoSkin.at<float>(135, 141)<<endl;
  float K = thres;
  for (; it != it_end; ++it, ++itlab) {
    VTLab labval = *itlab;
    VT srcval = *it;
    int a = labval[1];
    int b = labval[2];
    a = a * histNoSkin.size[0] / 256;
    b = b * histNoSkin.size[0] / 256;

    float p_c_noskin = histNoSkin.at<float>(a, b);
    float p_c_skin = histSkin.at<float>(a, b);
    float threshold = K * p_noskin[0] / p_skin[0];
    //    if(p_c_noskin<0)
    //      cout<<a<<" "<<b<<endl;
    ////    if(a<15 || a>20 ){
    ////      cout<<a<<"-"<<b<<"end ab";
    ////      cout << threshold << "\t" << threshold * p_c_noskin << "\t" << p_c_skin << "\t" << p_c_noskin <<"\t"<<(p_c_skin < threshold * p_c_noskin)<< endl;
    ////    }

    if (p_c_skin <= threshold * p_c_noskin) {
      *it = VT(0, 0, 0);
    }

    if (refcompare) {
      if ((*itref)[0] != 0 || (*itref)[1] != 0 || (*itref)[2] != 0) {
        peauPixTotal++;
        if ((*it)[0] == (*itref)[0] && (*it)[1] == (*itref)[1] && (*it)[2] == (*itref)[2]) {
          peauPixCorrect++;
        }
      }
      itref++;
    }
  }
  if (refcompare) {
    cout << "Percent correct:" << peauPixCorrect << "/" << peauPixTotal << "=" << ((float) peauPixCorrect) / peauPixTotal << endl;
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
  if (is_win_mode) {
    imshow("image skin", src);
    waitKey();
  }
  if (fileOut != NULL) {

    imwrite(fileOut, src);
  }
  src.release();
}

void gnuplot(const char *gnucommand) {

  char syscommand[1024];
  sprintf(syscommand, "echo \"%s\" | gnuplot -persist", gnucommand);
  system(syscommand);
}

void img256To32(const Mat& src, Mat& dst) {
  typedef Vec<uchar, 3 > VT;
  MatConstIterator_<VT> it = src.begin<VT > (),
    it_end = src.end<VT > ();
  MatIterator_<VT> itdest = dst.begin<VT > ();
  for (; it != it_end; ++it, ++itdest) {

    VT srcval = *it;
    *itdest = VT(srcval[0] * num_color / 256, srcval[1] * num_color / 256, srcval[2] * num_color / 256);
  }
}

void img32To256(const Mat& src, Mat& dst) {
  typedef Vec<uchar, 3 > VT;
  MatConstIterator_<VT> it = src.begin<VT > (),
    it_end = src.end<VT > ();
  MatIterator_<VT> itdest = dst.begin<VT > ();
  for (; it != it_end; ++it, ++itdest) {
    VT srcval = *it;
    *itdest = VT(srcval[0]*256 / num_color, srcval[1]*256 / num_color, srcval[2]*256 / num_color);
  }
}
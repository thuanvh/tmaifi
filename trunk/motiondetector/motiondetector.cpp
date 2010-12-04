#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <queue>
#include <iostream>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "motiondetector.h"
using namespace std;
using namespace cv;

void MotionDetection(char* videoFile, int fps, int queuesize) {
  cout << "begin" << endl;
  VideoCapture cap;
  string vf(videoFile);
  cout << "err" << endl;
  cap.open(vf); // open the default camera
  if (!cap.isOpened()) // check if we succeeded
    return;
  cout << "end open" << endl;
  //  cap.set(CV_CAP_PROP_FPS, fps);
  //  cap.set(CV_CAP_PROP_CONVERT_RGB, 1);
  Mat backImage;
  namedWindow("edges", 1);
  vector<Mat*> matqueue;
  int* pixelArray = new int[queuesize];
  int height;
  int width;
  //Mat firstframe;
  cap >> backImage;
  height = backImage.rows;
  width = backImage.cols;

  int time = -1;
  Mat* frame = NULL;
  Mat imgsubtract;

  Mat predictImage = Mat::zeros(height, width, CV_8UC3);
  Mat measureImage = Mat::zeros(height, width, CV_8UC3);
  Mat correctImage = Mat::zeros(height, width, CV_8UC3);

  vector<PatchItem*> trackingItemList;
  int itemLabel = 0;
  int defaultVisited = 5;
  while (true) {
    {
      //      cap.set(CV_CAP_PROP_POS_MSEC, time);
      //      cap.set(CV_CAP_PROP_POS_FRAMES, time);
      //      cout << "need convert to RGB " << cap.get(CV_CAP_PROP_CONVERT_RGB) << endl;
      //time += 1000;
      time++;
      frame = new Mat();
      Mat a;
      cap >> a; // get a new frame from camera
      a.copyTo(*frame);
      if (time % fps != 0) {
        frame->release();
        delete frame;
        continue;
      }
      matqueue.push_back(frame);
      if (matqueue.size() > 1) {
        Mat diff;
        diff.create(height, width, CV_8UC1);
        Mat img1;
        Mat img2;
        cvtColor(*frame, img1, CV_RGB2GRAY);
        cvtColor(backImage, img2, CV_RGB2GRAY);
        compare(img1, img2, diff, (int) CMP_EQ);
        //        compare(*frame, backImage, diff, (int) CMP_EQ);
        //        imshow("diff", diff);
      }
      if (matqueue.size() > queuesize) {
        matqueue.front()->release();
        delete matqueue.front();
        matqueue.erase(matqueue.begin());
      }
      //      imshow("video", *frame);
    }


    //    for (int row = 0; row < height; row++) {
    //      for (int col = 0; col < width; col++) {
    int cap = backImage.dataend - backImage.datastart;
    //    cout << "size:" << cap << endl;
    for (int ptrIndex = 0; ptrIndex < cap; ptrIndex++) {
      //        for (int k = 0; k < 3; k++) {
      for (int i = 0; i < matqueue.size(); i++) {
        Mat* frmImage = matqueue[i];
        uchar frmImagePtr = *(frmImage->data + ptrIndex);
        pixelArray[i] = frmImagePtr;
      }
      sort(pixelArray, pixelArray + matqueue.size());
      //        for (int i = 0; i < matqueue.size(); i++) {
      //          cout << pixelArray[i] << " ";
      //        }
      //        cout << endl;
      //        cout << pixelArray[matqueue.size() / 2] << endl;
      uchar* backImagePtr = (backImage.data + ptrIndex);
      *backImagePtr = pixelArray[matqueue.size() / 2];
      //        }
      //      }
    }



    //    for (int i = 0; i < matqueue.size(); i++) {
    //      char frameNo[255];
    //      sprintf(frameNo, "frame%d", i);
    //      imshow(frameNo, *matqueue[i]);
    //      cout << matqueue[i] << "-";
    //    }
    //    imshow("background", backImage);
    Mat currentFrame = (*frame).clone();
    Mat currentFrameLab;
    cvtColor(currentFrame, currentFrameLab, CV_RGB2Lab);
    Mat backImageLab;
    cvtColor(backImage, backImageLab, CV_RGB2Lab);
    Mat imgsubtractlab;
    absdiff(currentFrameLab, backImageLab, imgsubtractlab);
    int i = 0;
    for (uchar* ptr = currentFrameLab.datastart; ptr < currentFrameLab.dataend; i++, ptr++) {
      if (i % 3 == 0) *ptr = 0;
    }
    //bitwise_and(imgsubtractlab, Scalar(0, 255, 255), imgsubtractlab);
    cvtColor(imgsubtractlab, imgsubtract, CV_Lab2RGB);
    //absdiff(*frame, backImage, imgsubtract);
    //      subtract(*frame, backImage, imgsubtract);
    Mat imgerode;
    uchar a[9] = {0, 1, 0,
      1, 1, 1,
      0, 1, 0};
    Mat mat4c(3, 3, CV_8UC1, a);


    Mat subtractGray;
    subtractGray.create(imgsubtract.rows, imgsubtract.cols, CV_8UC1);
    cvtColor(imgsubtract, subtractGray, CV_RGB2GRAY);


    //    imshow("diff", subtractGray);
    threshold(subtractGray, imgerode, 30, 255, THRESH_BINARY);

    dilate(imgerode, imgerode, mat4c, Point(-1, -1), 1);
    erode(imgerode, imgerode, mat4c, Point(-1, -1), 4);
    dilate(imgerode, imgerode, mat4c, Point(-1, -1), 8);
    //    imshow("morpho", imgerode);
    //
    //        printImage(imgerode);
    //        waitKey();
    Mat labeledImage = Mat::zeros(imgerode.rows, imgerode.cols, CV_32SC1);
    int maxLabel;
    LabelingConnectedComponent(imgerode, labeledImage, maxLabel);
    int* maxX = new int[maxLabel];
    int* maxY = new int[maxLabel];
    int* minX = new int[maxLabel];
    int* minY = new int[maxLabel];
    FrameMotionDetect(labeledImage, maxLabel, maxX, maxY, minX, minY);
    cout << "end motion detect" << endl;
    FrameMotionFilter(maxLabel, maxX, maxY, minX, minY);
    FrameMotionMarking(imgerode, maxLabel, maxX, maxY, minX, minY);
    FrameMotionMarking(currentFrame, maxLabel, maxX, maxY, minX, minY);

    cout << "end motion marking" << endl;
    //    printImageLabel(labeledImage);
    //    for (int i = 0; i < maxLabel; i++) {
    //      if (minX[i] == MAXINT) continue;
    //      //rectangle(image, Point(minY[i], minX[i]), Point(maxY[i], maxX[i]), Scalar(255), 1, 8, 0);
    //      //rectangle(image, Point(minX[i], minY[i]), Point(maxX[i], maxY[i]), Scalar(255), 1, 8, 0);
    //      for (int k = minY[i]; k <= maxY[i]; k++) {
    //        for (int l = minX[i]; l <= maxX[i]; l++) {
    //          cout << labeledImage.at<int>(k, l) << " ";
    //        }
    //        cout << endl;
    //      }
    //      cout << endl;
    //    }
    //    Mat labeledColorImage;
    //    Mat b = Mat::zeros(labeledImage.rows, labeledImage.cols, CV_8UC1);
    //    //scaleAdd(labeledImage, 255 / (double) maxLabel, b, labeledColorImage);
    //    //    convertScaleAbs(labeledImage, labeledColorImage, 255 / (double) maxLabel, 0);

    imshow("differode", imgerode);
    imshow("currentFrame", currentFrame);
    //    imshow("labelImage", labeledColorImage);
    vector<PatchItem*> currentListImage;
    // extraire une liste d'objets
    FrameMotionExtraire(currentFrame, maxLabel, maxX, maxY, minX, minY, currentListImage);
    cout << "end frame motion extraire" << endl;
    // create image de points
    FrameMotionMatching(currentListImage, trackingItemList, itemLabel, defaultVisited);
    cout << "end frame motion matching" << endl;
    // trackingItemList
    Mat itemPointImage = Mat::zeros(imgerode.rows, imgerode.cols, CV_32SC1);
    markItem(itemPointImage, trackingItemList);
    cout << "end marking centre" << endl;
    Mat currentFrameMarkLabel = (*frame).clone();
    FrameMotionMarkingLabeled(currentFrameMarkLabel, trackingItemList);
    imshow("currentFrameLabel", currentFrameMarkLabel);
    cout << "end marking label" << endl;
    cout << "predict" << endl;
    //    //Predict
    for (int i = 0; i < trackingItemList.size(); i++) {
      trackingItemList[i]->predict();
    }
    DotMotionPredictMarking(predictImage, trackingItemList);
    imshow("predict Image", predictImage);
    cout << "measure" << endl;
    DotMotionMesureMarking(measureImage, trackingItemList);
    imshow("measure Image", measureImage);

    //    //Analyse Kalman
    cout << "correct" << endl;
    for (int i = 0; i < trackingItemList.size(); i++) {
      trackingItemList[i]->correct();
    }
    DotMotionCorrectMarking(correctImage, trackingItemList);
    imshow("correct image", correctImage);

    //Recalculate points
    //Draw result

    //    waitKey();
    //    getchar();
    delete []maxX;
    delete []maxY;
    delete []minX;
    delete []minY;
    if (waitKey(30) >= 0) break;
  }
}
//#define CARACTERISTIC_SIZE


// Normalize histogramme

void normalizeHistogram(MatND& hist, int totalpixel, int degree) {
  if (degree == 2) {
    // 2 dimensions
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++) {
        hist.at<float>(a, b) /= totalpixel;
      }
  } else if (degree == 3) {
    // 3 dimensions
    for (int a = 0; a < hist.size[0]; a++)
      for (int b = 0; b < hist.size[1]; b++) {
        for (int c = 0; c < hist.size[2]; c++) {
          hist.at<float>(a, b, c) /= totalpixel;
        }
      }
  }
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
// calculer le nombre total de valeur dans histogramme

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
// Sauver la matrice

void SaveMatrix(const MatND& hist, vector<double>& caracter, int degree) {
  if (degree == 2) {
    for (int a = 0; a < hist.size[0]; a++) {
      for (int b = 0; b < hist.size[1]; b++) {
        //        cout << hist.at<float>(a, b) << " ";
        caracter.push_back(hist.at<float>(a, b));
      }
    }
  } else if (degree == 3) {
    for (int a = 0; a < hist.size[0]; a++) {
      for (int b = 0; b < hist.size[1]; b++) {
        for (int c = 0; c < hist.size[2]; c++) {
          //          cout << hist.at<float>(a, b, c) << " ";
          caracter.push_back(hist.at<float>(a, b, c));
        }
      }
    }
  }
  //  getchar();
}

void extractHistoColorRGB(const Mat & src, int colorSize, vector<double>& caracter) {
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

  calcHist(&dst, 1, channels, Mat(), hist, 3, histSize, ranges, true, false);

  int nbhist = histNo(hist, 3);
  normalizeHistogram(hist, nbhist, 3);
  SaveMatrix(hist, caracter, 3);

}

void GetCaracteristic(const Mat& image, vector<double>& caracteristic) {
  extractHistoColorRGB(image, 32, caracteristic);
}
// Calculer la distance entre Couleur

double getColorHistoDistance(const vector<double>& learningVector, const vector<double>& testingVector) {
  double distance;
  //  for (int i=0; i < colorSize; i++) {
  //    distance += abs(learningVector[i] - testingVector[i]);
  //  }

  for (int i = 0; i < learningVector.size(); i++) {
    if (learningVector[i] + testingVector[i] != 0) {
      distance += pow(learningVector[i] - testingVector[i], 2) / (learningVector[i] + testingVector[i]);
    }
  }
  return distance;
}

//double GetImageDistance(const PatchItem& image1, const PatchItem& image2) {
//  //return 0;
//  double distance = getColorHistoDistance(image1.caracteristic, image2.caracteristic);
//  return distance;
//}

void FrameMotionMatching(vector<PatchItem*>& listImage, vector<PatchItem*>& mapImage, int& totalLabel, int defaultVisited) {
  cout << "list for compare" << mapImage.size() << endl;
  int measureFrameSize = 70;
  for (int i = 0; i < listImage.size(); i++) {
    PatchItem* patch = listImage[i];
    //    imshow("patch", * (patch->image));
    double minDistance = 1e20;
    int matchingIndex = -1;
    //    cout << "distance:";
    int frameMinX = patch->centerX - measureFrameSize / 2;
    int frameMaxX = patch->centerX + measureFrameSize / 2;
    int frameMinY = patch->centerY - measureFrameSize / 2;
    int frameMaxY = patch->centerY + measureFrameSize / 2;
    for (int j = 0; j < mapImage.size(); j++) {
      //      double distance = GetImageDistance(*patch, *mapImage[j]);
      //      if (minDistance > distance) {
      //        minDistance = distance;
      //        matchingIndex = j;
      //        cout << distance << " ";
      //      }
      PatchItem* oldpatch = mapImage[j];
      //      cout <<"frame compare "<< oldpatch->centerX << " " << oldpatch->centerY << " " \
//        << frameMinX << " " << frameMaxX << " " << frameMinY << " " << frameMaxY << endl;
      if (oldpatch->centerX > frameMinX && oldpatch->centerY > frameMinY &&
        oldpatch->centerX < frameMaxX && oldpatch->centerY < frameMaxY) {
        double distance = sqrt(pow((double) oldpatch->centerX - patch->centerX, 2.) + pow((double) oldpatch->centerY - patch->centerY, 2));
        if (minDistance > distance) {
          minDistance = distance;
          matchingIndex = j;
        }
      }
    }
    cout << endl << matchingIndex << "-" << minDistance << endl;
    //    waitKey();
    // seuil
    if (matchingIndex >= 0) {
      //      patch->label = mapImage[matchingIndex]->label;
      //      patch->visited = mapImage[matchingIndex]->visited;
      //      patch->color = mapImage[matchingIndex]->color;
      //      patch->vx = mapImage[matchingIndex]->centerX - patch->centerX;
      //      patch->vy = mapImage[matchingIndex]->centerY - patch->centerY;
      mapImage[matchingIndex]->vx = patch->centerX - mapImage[matchingIndex]->centerX;
      mapImage[matchingIndex]->vy = patch->centerY - mapImage[matchingIndex]->centerY;
      mapImage[matchingIndex]->centerY = patch->centerX;
      mapImage[matchingIndex]->centerY = patch->centerY;
      //mapImage[i] = patch;
//      delete patch;
    } else {
      patch->label = totalLabel++;
      patch->visited = defaultVisited;
      int r = rand() % 255;
      int g = rand() % 255;
      int b = rand() % 255;
      patch->color = Scalar(r, g, b);
      //init kalman
      patch->initKalman();
      mapImage.push_back(patch);
    }
  }
  vector<PatchItem*>::iterator iter;
  for (iter = mapImage.begin(); iter < mapImage.end(); iter++) {
    PatchItem* a = *iter;
    a->visited--;
    if (a->visited <= 0) {
      cout << "delete expire" << a->label << endl;
      mapImage.erase(iter);
//      delete a;
      
    }
  }
  //mapImage.clear();
  //  for (int i = 0; i < listImage.size(); i++) {
  //    PatchItem* patch = listImage[i];
  //    // assign matching
  //    int j;
  //    for (j = 0; j < mapImage.size(); j++) {
  //      PatchItem* matchingPatch = mapImage[j];
  //      if (matchingPatch->label == patch->label) {
  //        cout << "change old" << matchingPatch->label << endl;
  //        //        delete matchingPatch;
  //        mapImage[j] = patch;
  //        break;
  //      }
  //    }
  //    //add nouvel
  //    if (j == mapImage.size()) {
  //      mapImage.push_back(patch);
  //    }
  //  }
}

void FrameMotionExtraire(const Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY, vector<PatchItem*>& listImage) {
  for (int i = 0; i < maxLabel; i++) {
    //    cout << i << endl;
    //    cout << minY[i] << " " << minX[i] << maxY[i] << " " << maxX[i];
    if (minX[i] == MAXINT || minX[i] == maxX[i] || minY[i] == maxY[i]) {
      //      cout << "continue";
      continue;
    }

    //rectangle(image, Point(minY[i], minX[i]), Point(maxY[i], maxX[i]), Scalar(255), 1, 8, 0);
    //rectangle(image, Point(minX[i], minY[i]), Point(maxX[i], maxY[i]), Scalar(255), 1, 8, 0);
    int width = abs(maxX[i] - minX[i]);
    int height = abs(maxY[i] - minY[i]);
    int centerX = (maxX[i] + minX[i]) / 2;
    int centerY = (maxY[i] + minY[i]) / 2;

    //    cout<<"end cut image"<<endl;
    PatchItem* patch = new PatchItem();
    //    imshow("region", *region);
    //    waitKey();
    //    Mat* region = new Mat();
    //    getRectSubPix(image, Size(width, height), Point(centerX, centerY), *region);
    //    patch->image = region;
    patch->label = -1;
    //    GetCaracteristic(*patch->image, patch->caracteristic);
    patch->centerX = centerX;
    patch->centerY = centerY;
    patch->maxX = maxX[i];
    patch->minX = minX[i];
    patch->maxY = maxY[i];
    patch->minY = minY[i];
    listImage.push_back(patch);
  }
}

void markItem(Mat& markedImage, vector<PatchItem*> trackingList) {
  for (int i = 0; i < trackingList.size(); i++) {
    markedImage.at<int>(trackingList[i]->centerY, trackingList[i]->centerX) = trackingList[i]->label;
  }
}

void FrameMotionMarkingLabeled(Mat& image, vector<PatchItem*> trackingList) {
  for (int i = 0; i < trackingList.size(); i++) {
    PatchItem* item = trackingList[i];
    //if (item->minX[i] == MAXINT) continue;
    rectangle(image, Point(item->minX, item->minY), Point(item->maxX, item->maxY), item->color, 1, 8, 0);
    char labelstr[10];
    sprintf(labelstr, "%d", item->label);
    putText(image, string(labelstr), Point(item->minX, item->minY), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255));
  }
}

void DotMotionMesureMarking(Mat& image, vector<PatchItem*> trackingList) {
  for (int i = 0; i < trackingList.size(); i++) {
    PatchItem* item = trackingList[i];
    circle(image, Point(item->centerX, item->centerY), 1, item->color, 1, 8, 0);
  }
}

void DotMotionPredictMarking(Mat& image, vector<PatchItem*> trackingList) {
  for (int i = 0; i < trackingList.size(); i++) {
    PatchItem* item = trackingList[i];
    circle(image, Point(item->predictX, item->predictY), 1, item->color, 1, 8, 0);
  }
}

void DotMotionCorrectMarking(Mat& image, vector<PatchItem*> trackingList) {
  for (int i = 0; i < trackingList.size(); i++) {
    PatchItem* item = trackingList[i];
    circle(image, Point(item->correctX, item->correctY), 1, item->color, 1, 8, 0);
  }
}

void FrameMotionMarking(Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY) {
  for (int i = 0; i < maxLabel; i++) {
    if (minX[i] == MAXINT) continue;
    //rectangle(image, Point(minY[i], minX[i]), Point(maxY[i], maxX[i]), Scalar(255), 1, 8, 0);
    rectangle(image, Point(minX[i], minY[i]), Point(maxX[i], maxY[i]), Scalar(255), 1, 8, 0);
  }
}

void FrameMotionFilter(int maxLabel, int* maxX, int* maxY, int* minX, int* minY) {
  for (int i = 0; i < maxLabel; i++) {
    if (minX[i] == MAXINT) continue;
    if (abs(maxX[i] - minX[i]) + abs(maxY[i] - minY[i]) < 40) {
      minX[i] = maxX[i] = minY[i] = maxY[i] = MAXINT;
    }
    //rectangle(image, Point(minX[i], minY[i]), Point(maxX[i], maxY[i]), Scalar(255), 1, 8, 0);
  }
}

void FrameMotionDetect(const Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY) {
  for (int i = 0; i < maxLabel; i++) {
    minX[i] = minY[i] = MAXINT;
    maxX[i] = maxY[i] = MININT;
  }
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      int label = image.at<int>(i, j);
      if (label != 0) {
        int labelIndex = label - 1;
        maxX[labelIndex] = maxX[labelIndex] < j ? j : maxX[labelIndex];
        maxY[labelIndex] = maxY[labelIndex] < i ? i : maxY[labelIndex];
        minX[labelIndex] = minX[labelIndex] < j ? minX[labelIndex] : j;
        minY[labelIndex] = minY[labelIndex] < i ? minY[labelIndex] : i;
      }
    }
  }
}

void printImageLabel(const Mat& img) {
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      cout << (int) img.at<int > (i, j) << " ";
    }
    cout << endl;
  }
}

void printImage(const Mat& img) {
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      cout << (int) img.at<uchar > (i, j) << " ";
    }
    cout << endl;
  }
}

void LabelingConnectedComponent(const Mat& img, Mat& dst, int& label) {
  label = 0;
  //dst.setTo(Scalar(0), Mat());
  for (int i = 0; i < dst.rows; i++) {
    for (int j = 0; j < dst.cols; j++) {
      dst.at<int>(i, j) = img.at<char>(i, j);
    }
  }
  //  img.copyTo(dst);
  const int max = 1000;
  int left = max;
  int top = max;
  int right = max;
  int bottom = max;
  int min = 0;
  cout << "contour loop 1" << dst.rows << "*" << dst.cols << endl;
  for (int i = 0; i < dst.rows; i++) {
    for (int j = 0; j < dst.cols; j++) {
      if (dst.at<int > (i, j) != 0) {
        if (j == 0)
          left = max;
        else
          left = (dst.at<int > (i, j - 1) == 0) ? max : dst.at<int > (i, j - 1);
        if (i == 0)
          top = max;
        else
          top = (dst.at<int > (i - 1, j) == 0) ? max : dst.at<int > (i - 1, j);
        min = left < top ? left : top;
        //cout << i << "-" << j << "-" << left << "-" << top << "-" << min << "-" << label << endl;
        if (min == max) {
          label++;

          dst.at<int > (i, j) = label;
        } else {
          dst.at<int > (i, j) = min;
        }
        //        if (dst.at<int > (i, j) == 39) {
        //          int a = 1;
        //          cout << "test 39 " << dst.at<int > (i, j - 1) << " " << dst.at<int > (i - 1, j) << " " << left << " " << top << " " << dst.at<int > (i, j) << endl;
        //        }
        //        cout << "label:" << label << endl;
      }
    }
  }
  //  printImageLabel(dst);
  cout << "contour loop 2" << endl;
  for (int i = dst.rows - 1; i >= 0; i--) {
    for (int j = dst.cols - 1; j >= 0; j--) {
      if (dst.at<float > (i, j) != 0) {
        if (j == dst.cols - 1) right = max;
        else
          right = (dst.at<int > (i, j + 1) == 0) ? max : dst.at<int > (i, j + 1);
        if (i == dst.rows - 1) bottom = max;
        else
          bottom = (dst.at<int > (i + 1, j) == 0) ? max : dst.at<int > (i + 1, j);

        //cout << i << "-" << j << "-" << right << "-" << bottom << "-" << min  << endl;
        min = right < bottom ? right : bottom;
        dst.at<int > (i, j) = dst.at<int > (i, j) < min ? dst.at<int > (i, j) : min;
      }
    }
  }

  //  printImageLabel(dst);
  cout << "end contour loop" << endl;
}

int main1(int argc, char** argv) {
  //  KalmanMotionDetection(argc, argv);
  uchar a[78] = {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 0, 0, 3, 1, 3, 5, 3, 7, 0,
    0, 1, 2, 1, 0, 0, 0, 0, 4, 7, 5, 7, 6,
    1, 1, 1, 1, 0, 0, 1, 1, 3, 2, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1,
    0, 0, 1, 0, 0, 0, 0, 0, 2, 3, 0, 1, 1
  };

  Mat mat4c(6, 13, CV_8UC1, a);
  Mat labeledImage = Mat::zeros(mat4c.rows, mat4c.cols, CV_32SC1);
  int maxLabel;
  printImage(mat4c);
  LabelingConnectedComponent(mat4c, labeledImage, maxLabel);
  printImageLabel(labeledImage);
  int* maxX = new int[maxLabel];
  int* maxY = new int[maxLabel];
  int* minX = new int[maxLabel];
  int* minY = new int[maxLabel];
  FrameMotionDetect(labeledImage, maxLabel, maxX, maxY, minX, minY);
  cout << "end motion detect" << endl;
  for (int i = 0; i < maxLabel; i++) {
    //    if (minX[i] == MAXINT) continue;
    //rectangle(image, Point(minY[i], minX[i]), Point(maxY[i], maxX[i]), Scalar(255), 1, 8, 0);
    cout << minY[i] << "-" << minX[i] << "-" << maxY[i] << "-" << maxX[i] << endl;
  }
  FrameMotionMarking(mat4c, maxLabel, maxX, maxY, minX, minY);
  cout << "end motion marking" << endl;
  imshow("frame", mat4c);
  waitKey();
  delete []maxX;
  delete []maxY;
  delete []minX;
  delete []minY;
}

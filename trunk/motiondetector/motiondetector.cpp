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
        imshow("diff", diff);
      }
      if (matqueue.size() > queuesize) {
        matqueue.front()->release();
        delete matqueue.front();
        matqueue.erase(matqueue.begin());
      }
      imshow("video", *frame);
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
    imshow("background", backImage);
    Mat currentFrame=(*frame).clone();
    absdiff(*frame, backImage, imgsubtract);
    //      subtract(*frame, backImage, imgsubtract);
    Mat imgerode;
    uchar a[9] = {0, 1, 0,
      1, 1, 1,
      0, 1, 0};
    Mat mat4c(3, 3, CV_8UC1, a);


    Mat subtractGray;
    subtractGray.create(imgsubtract.rows, imgsubtract.cols, CV_8UC1);
    cvtColor(imgsubtract, subtractGray, CV_RGB2GRAY);

    imshow("diff", subtractGray);
    threshold(subtractGray, imgerode, 30, 255, THRESH_BINARY);

    dilate(imgerode, imgerode, mat4c, Point(-1, -1), 1);
    erode(imgerode, imgerode, mat4c, Point(-1, -1), 4);
    dilate(imgerode, imgerode, mat4c, Point(-1, -1), 8);
    //
    //    printImage(imgerode);
    //    waitKey();
    Mat labeledImage = Mat::zeros(imgerode.rows, imgerode.cols, CV_32SC1);
    int maxLabel;
    LabelingConnectedComponent(imgerode, labeledImage, maxLabel);
    int* maxX = new int[maxLabel];
    int* maxY = new int[maxLabel];
    int* minX = new int[maxLabel];
    int* minY = new int[maxLabel];
    FrameMotionDetect(labeledImage, maxLabel, maxX, maxY, minX, minY);
    cout << "end motion detect" << endl;
    FrameMotionMarking(imgerode, maxLabel, maxX, maxY, minX, minY);
    FrameMotionMarking(currentFrame, maxLabel, maxX, maxY, minX, minY);

    cout << "end motion marking" << endl;
    //    printImage(labeledImage);
    //    Mat labeledColorImage;
    //    Mat b = Mat::zeros(labeledImage.rows, labeledImage.cols, CV_8UC1);
    //    //scaleAdd(labeledImage, 255 / (double) maxLabel, b, labeledColorImage);
    //    //    convertScaleAbs(labeledImage, labeledColorImage, 255 / (double) maxLabel, 0);

    imshow("differode", imgerode);
    imshow("currentFrame", currentFrame);
    //    imshow("labelImage", labeledColorImage);

//    waitKey();
//    getchar();
    delete []maxX;
    delete []maxY;
    delete []minX;
    delete []minY;
    if (waitKey(30) >= 0) break;
  }
}

void FrameMotionMarking(Mat& image, int maxLabel, int* maxX, int* maxY, int* minX, int* minY) {
  for (int i = 0; i < maxLabel; i++) {
    if (minX[i] == MAXINT) continue;
    //rectangle(image, Point(minY[i], minX[i]), Point(maxY[i], maxX[i]), Scalar(255), 1, 8, 0);
    rectangle(image, Point(minX[i], minY[i]), Point(maxX[i], maxY[i]), Scalar(255), 1, 8, 0);
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

int KalmanMotionDetection(int argc, char** argv) {
  const float A[] = {1, 1, 0, 1};

  IplImage* img = cvCreateImage(cvSize(500, 500), 8, 3);
  CvKalman* kalman = cvCreateKalman(2, 1, 0);
  CvMat* state = cvCreateMat(2, 1, CV_32FC1); /* (phi, delta_phi) */
  CvMat* process_noise = cvCreateMat(2, 1, CV_32FC1);
  CvMat* measurement = cvCreateMat(1, 1, CV_32FC1);
  CvRNG rng = cvRNG(-1);
  char code = -1;

  cvZero(measurement);
  cvNamedWindow("Kalman", 1);

  for (;;) {
    cvRandArr(&rng, state, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1));

    memcpy(kalman->transition_matrix->data.fl, A, sizeof (A));
    cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));
    cvSetIdentity(kalman->process_noise_cov, cvRealScalar(1e-5));
    cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(1e-1));
    cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));
    cvRandArr(&rng, kalman->state_post, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1));

    for (;;) {
#define calc_point(angle)                                      \
                cvPoint( cvRound(img->width/2 + img->width/3*cos(angle)),  \
                         cvRound(img->height/2 - img->width/3*sin(angle)))

      float state_angle = state->data.fl[0];
      CvPoint state_pt = calc_point(state_angle);

      const CvMat* prediction = cvKalmanPredict(kalman, 0);
      float predict_angle = prediction->data.fl[0];
      CvPoint predict_pt = calc_point(predict_angle);
      float measurement_angle;
      CvPoint measurement_pt;

      cvRandArr(&rng, measurement, CV_RAND_NORMAL, cvRealScalar(0),
        cvRealScalar(sqrt(kalman->measurement_noise_cov->data.fl[0])));

      /* generate measurement */
      cvMatMulAdd(kalman->measurement_matrix, state, measurement, measurement);

      measurement_angle = measurement->data.fl[0];
      measurement_pt = calc_point(measurement_angle);

      /* plot points */
#define draw_cross( center, color, d )                                 \
                cvLine( img, cvPoint( center.x - d, center.y - d ),                \
                             cvPoint( center.x + d, center.y + d ), color, 1, CV_AA, 0); \
                cvLine( img, cvPoint( center.x + d, center.y - d ),                \
                             cvPoint( center.x - d, center.y + d ), color, 1, CV_AA, 0 )

      cvZero(img);
      draw_cross(state_pt, CV_RGB(255, 255, 255), 3);
      draw_cross(measurement_pt, CV_RGB(255, 0, 0), 3);
      draw_cross(predict_pt, CV_RGB(0, 255, 0), 3);
      cvLine(img, state_pt, measurement_pt, CV_RGB(255, 0, 0), 3, CV_AA, 0);
      cvLine(img, state_pt, predict_pt, CV_RGB(255, 255, 0), 3, CV_AA, 0);

      cvKalmanCorrect(kalman, measurement);

      cvRandArr(&rng, process_noise, CV_RAND_NORMAL, cvRealScalar(0),
        cvRealScalar(sqrt(kalman->process_noise_cov->data.fl[0])));
      cvMatMulAdd(kalman->transition_matrix, state, process_noise, state);

      cvShowImage("Kalman", img);
      code = (char) cvWaitKey(100);

      if (code > 0)
        break;
    }
    if (code == 27 || code == 'q' || code == 'Q')
      break;
  }

  cvDestroyWindow("Kalman");

  return 0;
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
  uchar a[78] = {
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 0, 0, 0, 1, 3, 5, 0, 0, 0,
    0, 1, 2, 1, 0, 0, 0, 0, 4, 7, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 0, 0, 3, 2, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1
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

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
      cout << "need convert to RGB " << cap.get(CV_CAP_PROP_CONVERT_RGB) << endl;
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
    absdiff(*frame, backImage, imgsubtract);
    //      subtract(*frame, backImage, imgsubtract);
    imshow("diff", imgsubtract);
//    waitKey();
    if (waitKey(30) >= 0) break;
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

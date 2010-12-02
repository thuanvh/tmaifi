#include "patchItem.h"
using namespace std;
void PatchItem::initKalman() {

  kalman = new KalmanFilter();
  kalman->init(2, 2, 0);
  //kalman->init(4,2,0);
  //const float A[] = {1, 1, 0, 1};
  kalman->transitionMatrix.at<float>(0,0)=1;
  kalman->transitionMatrix.at<float>(1,0)=0;
  kalman->transitionMatrix.at<float>(0,1)=0;
  kalman->transitionMatrix.at<float>(1,1)=1;

  //kalman->measurementMatrix.diag()
  setIdentity(kalman->measurementMatrix,Scalar(1,1,1));
  setIdentity(kalman->processNoiseCov,Scalar(1e-5));
  setIdentity(kalman->measurementNoiseCov,Scalar(1e-1));
  setIdentity(kalman->errorCovPost,Scalar(1));
  kalman->statePost.at<float>(0,0)=centerX;
  kalman->statePost.at<float>(1,0)=centerY;

  //memcpy(kalman->transitionMatrix->, A, sizeof (A));
  //    cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));
  //    cvSetIdentity(kalman->process_noise_cov, cvRealScalar(1e-5));
  //    cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(1e-1));
  //    cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));
  //cvRandArr(&rng, kalman->state_post, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1));

  //  memcpy(kalman->transition_matrix->data.fl, A, sizeof (A));
  //    cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));
  //    cvSetIdentity(kalman->process_noise_cov, cvRealScalar(1e-5));
  //    cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(1e-1));
  //    cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));
  //    cvRandArr(&rng, kalman->state_post, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(0.1));
}

void PatchItem::predict() {
  getchar();
  kalman->predict();
  predictX = kalman->statePre.at<float>(0, 0);
  predictY = kalman->statePre.at<float>(1, 0);
  cout<<"predict result "<<predictX<<"*"<<predictY<<endl;
}

void PatchItem::correct() {
  Mat mesure = Mat::ones(2, 1, CV_32FC1);
  mesure.at<float>(0, 0) = centerX;
  mesure.at<float>(1, 0) = centerY;
  kalman->correct(mesure);
  correctX = kalman->statePost.at<float>(0, 0);
  correctY = kalman->statePost.at<float>(1, 0);
  cout<<"correct result "<<correctX<<"*"<<correctY<<endl;
}

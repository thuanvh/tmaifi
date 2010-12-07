#include "patchItem.h"
using namespace std;

PatchItem::PatchItem() {
  centerX = 0;
  centerY = 0;
  vx = 0;
  vy = 0;
  kalman=NULL;
  imgcombine=imgcorrect=imgmeasure=imgpredict=NULL;
}

PatchItem::PatchItem(int x, int y, int vx, int vy, int typeOfTracking) {
  centerX = x;
  centerY = y;
  this->vx = vx;
  this->vy = vy;
  this->typeOfTracking = typeOfTracking;
}

PatchItem::~PatchItem() {
  cout << "~~~~" << endl;
  //  if (image != NULL)
  //    delete image;
  if (kalman != NULL)
    delete kalman;
  if (imgcombine!=NULL)
      delete imgcombine;
  if(imgcorrect!=NULL)
      delete imgcorrect;
  if(imgpredict!=NULL)
      delete imgpredict;
  if(imgmeasure!=NULL)
      delete imgmeasure;
}

void PatchItem::initKalman() {

  kalman = new KalmanFilter();
  if (typeOfTracking == TRACKING_POSITION_VELOCITY) {
    kalman->init(4, 4, 0);
    //kalman->init(4,2,0);
    const float A[4][4] = {
      1, 0, 1, 0,
      0, 1, 0, 1,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    for (int i = 0; i <= 4; i++) {
      for (int j = 0; j <= 4; j++) {
        kalman->transitionMatrix.at<float>(i, j) = A[i][j];
      }
    }
  } else if (typeOfTracking == TRACKING_POSITION) {
    kalman->init(2, 2, 0);
    //kalman->init(4,2,0);
    const float A[2][2] = {
      1, 0,
      0, 1,
    };
    for (int i = 0; i <= 2; i++) {
      for (int j = 0; j <= 2; j++) {
        kalman->transitionMatrix.at<float>(i, j) = A[i][j];
      }
    }
  }

  //kalman->measurementMatrix.diag()
  setIdentity(kalman->measurementMatrix, Scalar(1, 1, 1));
  setIdentity(kalman->processNoiseCov, Scalar(1e-5));
  setIdentity(kalman->measurementNoiseCov, Scalar(1e-1));
  setIdentity(kalman->errorCovPost, Scalar(1));

  if (typeOfTracking == TRACKING_POSITION_VELOCITY) {
    kalman->statePost.at<float>(0, 0) = centerX;
    kalman->statePost.at<float>(1, 0) = centerY;
    kalman->statePost.at<float>(2, 0) = vx;
    kalman->statePost.at<float>(3, 0) = vy;
  } else if (typeOfTracking == TRACKING_POSITION) {
    kalman->statePost.at<float>(0, 0) = centerX;
    kalman->statePost.at<float>(1, 0) = centerY;
  }
  //predict();
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
  //  getchar();
  const Mat* predictMat = &kalman->predict();
  //kalman->predict();
  if (typeOfTracking == TRACKING_POSITION_VELOCITY) {
    predictX = predictMat->at<float>(0, 0);
    predictY = predictMat->at<float>(1, 0);
    predictVx = predictMat->at<float>(2, 0);
    predictVy = predictMat->at<float>(3, 0);
  } else if (typeOfTracking == TRACKING_POSITION) {
    predictX = predictMat->at<float>(0, 0);
    predictY = predictMat->at<float>(1, 0);
  }

//  cout << "predict result "<<label<<" : " << predictX << "*" << predictY << " " << predictVx << " " << predictVy << endl;
}

void PatchItem::correct() {

  if (typeOfTracking == TRACKING_POSITION_VELOCITY) {
    Mat mesure = Mat::ones(4, 1, CV_32FC1);
    mesure.at<float>(0, 0) = centerX;
    mesure.at<float>(1, 0) = centerY;
    mesure.at<float>(2, 0) = vx;
    mesure.at<float>(3, 0) = vy;
    Mat correctMat = kalman->correct(mesure);
    correctX = correctMat.at<float>(0, 0);
    correctY = correctMat.at<float>(1, 0);
    correctVx = correctMat.at<float>(2, 0);
    correctVy = correctMat.at<float>(3, 0);

  } else if (typeOfTracking == TRACKING_POSITION) {
    Mat mesure = Mat::ones(2, 1, CV_32FC1);
    mesure.at<float>(0, 0) = centerX;
    mesure.at<float>(1, 0) = centerY;
    Mat correctMat = kalman->correct(mesure);
    correctX = correctMat.at<float>(0, 0);
    correctY = correctMat.at<float>(1, 0);
  }

  cout << "correct result " <<label<<" : "<< correctX << "*" << correctY << " " << correctVx << " " << correctVy << endl;
}

void PatchItem::refresh() {

}

void PatchItem::draw() {
//  cout << "draw image" << endl;
//  getchar();
  // correct red
    circle(*imgcorrect, Point(correctX, correctY), 2, Scalar(0, 0, 255), 0, 8, 0);
    circle(*imgcombine, Point(correctX, correctY), 2, Scalar(0, 0, 255), 0, 8, 0);
    // predire bleu
    circle(*imgpredict, Point(predictX, predictY), 2, Scalar(255, 0, 0), 0, 8, 0);
    circle(*imgcombine, Point(predictX, predictY), 2, Scalar(255, 0, 0), 0, 8, 0);
    // measure vert
    circle(*imgmeasure, Point(centerX, centerY), 2, Scalar(0, 255,0 ), 0, 8, 0);
    circle(*imgcombine, Point(centerX, centerY), 2, Scalar(0, 255,0 ), 0, 8, 0);
//  cout << "draw image" << endl;
//  getchar();
}

void PatchItem::initImage(int width, int height) {
//  cout << "init image" << endl;
  //(int _rows, int _cols, int _type, const Scalar& _s)
  imgcombine = new Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
  imgcorrect = new Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
  imgpredict = new Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
  imgmeasure = new Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
//  cout << "init image" << endl;
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

void patchItemTest() {
  int x = 1, y = 1, vx = 0, vy = 0;
  int type = TRACKING_POSITION;
  PatchItem patch(x, y, vx, vy, type);
  patch.initKalman();
  while (true) {
    patch.predict();
    cout << "Predict: " << patch.predictX << " " << patch.predictY << " " \
      << patch.predictVx << " " << patch.predictVy << endl;
    int oldX = patch.centerX;
    patch.centerX += rand() % 10;
    int oldY = patch.centerY;
    patch.centerY += rand() % 10;
    patch.vx = patch.centerX - oldX;
    patch.vy = patch.centerY - oldY;
    cout << "Mesure: " << patch.centerX << " " << patch.centerY << " "\
      << patch.vx << " " << patch.vy << endl;

    patch.correct();
    cout << "Correct: " << patch.correctX << " " << patch.correctY << " "\
      << patch.correctVx << " " << patch.correctVy << endl;
    getchar();
  }
}

int maintest(int argc, char** argv) {
  patchItemTest();
  return 0;
}


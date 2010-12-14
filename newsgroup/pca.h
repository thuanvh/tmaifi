#ifndef PCA_H
#define PCA_H
#include "tf-idf.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;
class PCAConverter {
      Mat eigenVectors;
      Mat mean;
//     IplImage* eigenVectors;
//     IplImage* mean;
    int maxComponent;
    int width;
    int height;

    // Global variables
    //IplImage ** faceImgArr = 0; // array of face images
//     CvMat * personNumTruthMat; // array of person numbers
    //#define	MAX_NAME_LENGTH 256		// Give each name a fixed size for easier code.
    //char **personNames = 0;			// array of person names (indexed by the person number). Added by Shervin.
//     vector<string> personNames; // array of person names (indexed by the person number). Added by Shervin.
//     int faceWidth ; // Default dimensions for faces in the face recognition database. Added by Shervin.
//     int faceHeight ; //
//     int nPersons ; // the number of people in the training set. Added by Shervin.
    int ndocs ; // the number of training images
    int nEigens ; // the number of eigenvalues
    IplImage * pAvgTrainImg ; // the average image
    IplImage ** eigenVectArr ; // eigenvectors
    CvMat * eigenValMat ; // eigenvalues
    //CvMat * projectedTrainFaceMat = 0; // projected training faces
//     CvCapture* camera ; // The camera device.

public:
  void import(vector<category>& categorylist,vector<string>& dict,Mat& data);
  void calculatePCA(Mat& data);
  void saveSimple();
  void loadSimple();
  void getProjection(Mat& data,Mat& dataproject);
    void import(vector<category>& categorylist,vector<string>& dict,IplImage**& data);
    void calculatePCA(IplImage** data);
    void save();
    void load();
    void getProjection(IplImage** data,CvMat*& dataproject);
    
    PCAConverter(){
      // Global variables
      //faceImgArr = 0; // array of face images
//       personNumTruthMat = 0; // array of person numbers
      //#define	MAX_NAME_LENGTH 256		// Give each name a fixed size for easier code.
      //char **personNames = 0;			// array of person names (indexed by the person number). Added by Shervin.
      //vector<string> personNames; // array of person names (indexed by the person number). Added by Shervin.
//       faceWidth = 120; // Default dimensions for faces in the face recognition database. Added by Shervin.
//       faceHeight = 90; //
//       nPersons = 0; // the number of people in the training set. Added by Shervin.
      ndocs = 0; // the number of training images
      nEigens = 0; // the number of eigenvalues
      pAvgTrainImg = 0; // the average image
      eigenVectArr = 0; // eigenvectors
      eigenValMat = 0; // eigenvalues
      //projectedTrainFaceMat = 0; // projected training faces
//       camera = 0; // The camera device.
    }
};

#endif

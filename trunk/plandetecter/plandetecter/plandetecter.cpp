#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <algorithm>
#include "plandetecter.h"
using namespace std;
using namespace cv;

void plandetecter::detectPlan(char* folder)
{
  // lire la liste de fichier
  DIR *dp;
  struct dirent *ep;
  vector<string> files;
  
  dp = opendir(folder);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      if(strcmp(ep->d_name,".")==0 || strcmp(ep->d_name,"..")==0 || strcmp(ep->d_name,"Thumbs.db")==0)
	continue;
      string pathname=(string)folder+"/"+(string) ep->d_name;
      files.push_back(pathname);
    }
    
    (void) closedir(dp);
  } else
    cout<< ERR_DIR_OPEN<<endl;
  sort(files.begin(),files.end());
  
  
  // chercher le plan
  vector<int> planList;
  planList.push_back(0);
  for(int i=1; i<files.size(); i++){
    cout<<files[i-1]<<" vs "<<files[i]<<endl;
    Mat image1=imread(files[i-1],1);
    Mat image2=imread(files[i],1);
    bool isPlan=algo->isDifferentPlan(image1,image2);
    if(isPlan){
      planList.push_back(i);
    }
  }
  // print out the result
  cout<<"Number of plans:"<<planList.size()<<endl;
  cout<<"List of plan:"<<endl;
  for(int i=0; i<planList.size(); i++){
    cout<<planList[i]<<", ";
  }
  cout <<endl;
  delete algo;
}

bool likelihoodRatioMethod::isDifferentPlan(Mat& image1, Mat& image2)
{
  //threshold(dcut,dcut,seuil_h,1,THRESH_BINARY);
  double percent=calculateDistance(image1,image2);
  if(percent>seuil_p)
    return true;
  else
    return false;
}
double likelihoodRatioMethod::calculateDistance(Mat& image1, Mat& image2)
{
  int sizew=8;
  int nrow=image1.rows/sizew;
  int ncol=image1.cols/sizew;
  Mat dcut=Mat::zeros(nrow,ncol,CV_8UC1);
  for (int irow=0; irow<nrow; irow++) {
    for (int icol=0; icol<ncol; icol++) {
      Mat subimage1;
      Mat subimage2;
      Size size(sizew,sizew);
      Point centre(irow*sizew+sizew/2,icol*sizew+sizew/2);
      getRectSubPix(image1,size,centre,subimage1);
      getRectSubPix(image2,size,centre,subimage2);
      Scalar mean1,sd1,mean2,sd2;
      //Scalar mean2=mean(subimage2);
      meanStdDev(subimage1,mean1,sd1);
      meanStdDev(subimage2,mean2,sd2);
      double dcutr = pow((sd1[0] + sd2[0]) / 2 + pow(( mean1[0] - mean2[0] )/2,2),2) / (sd1[0]*sd2[0]);
      double dcutg = pow((sd1[1] + sd2[1]) / 2 + pow(( mean1[1] - mean2[1] )/2,2),2) / (sd1[1]*sd2[1]);
      double dcutb = pow((sd1[2] + sd2[2]) / 2 + pow(( mean1[2] - mean2[2] )/2,2),2) / (sd1[2]*sd2[2]);
      dcut.at<uchar>(irow,icol)= (dcutb+dcutg+dcutr)/3>seuil_h?1:0;
    }
  }
  //threshold(dcut,dcut,seuil_h,1,THRESH_BINARY);
  double percent=countNonZero(dcut)/(double)(nrow*ncol);
  return percent;
}
float likelihoodRatioMethod::getDecisionThreshold()
{
  return seuil_p;
}
void likelihoodRatioMethod::setDecisionThreshold(float thres)
{
  seuil_p=thres;
}

bool pixelToPixelMethod::isDifferentPlan(Mat& image1, Mat& image2)
{
    double distance=calculateDistance(image1,image2);
    if (distance>seuil_pixel)
        return true;
    else
        return false;
}

double pixelToPixelMethod::calculateDistance(Mat& image1, Mat& image2)
{
  Mat subtraction;
  Mat img1,img2;
  cvtColor(image1,img1,CV_RGB2GRAY);
  cvtColor(image2,img2,CV_RGB2GRAY);
  absdiff(img1,img2,subtraction);
  imshow("subtraction",subtraction);
  waitKey();
  threshold(subtraction,subtraction,seuil_delta,1,THRESH_BINARY);
  int nb = countNonZero(subtraction);
  int nbtotal=image1.rows * image1.cols;
  cout<<nb<<"/"<<nbtotal<<"="<<nb/(float)nbtotal<<endl;
  return nb/(float)nbtotal;
}

float pixelToPixelMethod::getDecisionThreshold()
{
  return seuil_pixel;
}

void pixelToPixelMethod::setDecisionThreshold(float thres)
{
  seuil_pixel=thres;
}


bool histogramMethod::isDifferentPlan(Mat& image1, Mat& image2)
{
    double dissimilarity=calculateDistance(image1,image2);
    
    if(dissimilarity>seuil_histo)
      return true;
    else
      return false;
}

double histogramMethod::calculateDistance(Mat& image1, Mat& image2)
{
  int histoBin = 30;
  int histSize[] = {histoBin};
  
  float grisranges[] = { 0, 256 };
  
  const float* ranges[] = { grisranges };
  MatND histo1;
  MatND histo2;
  // une seuil channel
  int channels[] = {0};
  
  calcHist( &image1, 1, channels, Mat(),histo1, 1, histSize, ranges,true, false );
  calcHist( &image1, 1, channels, Mat(),histo2, 1, histSize, ranges,true, false );
  double dissimilarity=0;
  for(int i=0; i<histoBin; i++){
    dissimilarity+=(abs(histo1.at<int>(0,i)-histo2.at<int>(0,i))/histo1.at<int>(0,i));
  }
  return dissimilarity;
}

float histogramMethod::getDecisionThreshold()
{
  return seuil_histo;
}

void histogramMethod::setDecisionThreshold(float thres)
{
  seuil_histo=thres;
}


bool contourMethod::isDifferentPlan(Mat& image1, Mat& image2)
{
  double nbContourMax=calculateDistance(image1,image2);
  
  if(nbContourMax>contourThreshold)
    return true;
  else
    return false;
}

double contourMethod::calculateDistance(Mat& image1, Mat& image2)
{
  int nbContour[2]={0};// int nbContourIn; int nbContourOut;
  Mat contourImage[2];
  Mat img1,img2;
  cvtColor(image1,img1,CV_RGB2GRAY);
  cvtColor(image2,img2,CV_RGB2GRAY);
  Canny(img1,contourImage[0],canny_threshold1,canny_threshold2,apertureSize);
  Canny(img2,contourImage[1],canny_threshold1,canny_threshold2,apertureSize);
  imshow("contour1",contourImage[0]);
  imshow("contour2",contourImage[1]);
  waitKey();
  for(int contourIndex=0; contourIndex<2; contourIndex++){
    Mat* contourImage1=&contourImage[contourIndex];
    Mat* contourImage2=&contourImage[(contourIndex+1)%2];
    for(int irow=0;irow< contourImage1->rows; irow++){
      for(int icol=0; icol< contourImage1->cols; icol++){
	if(contourImage1->at<uchar>(irow,icol)>0){
	  int irowmin2=irow-contourDistance<0?0:irow-contourDistance;
	  int irowmax2=irow+contourDistance>=contourImage1->rows?contourImage1->rows:irow+contourDistance;
	  int icolmin2=icol-contourDistance<0?0:icol-contourDistance;
	  int icolmax2=icol+contourDistance>=contourImage1->cols?contourImage1->cols:icol+contourDistance;
	  for(int irow2=irowmin2;irow2<irowmax2; irow2++){
	    for(int icol2=icolmin2; icol2 < icolmax2; icol2++){
	      if(contourImage2->at<uchar>(irow2,icol2)>0){
		nbContour[contourIndex]++;
	      }
	    }
	  }
	}
      }
    }
  }
  int nbContourMax=nbContour[0]<nbContour[1]?nbContour[1]:nbContour[0];
  cout<<nbContourMax<<endl;
  return nbContourMax;
}

float contourMethod::getDecisionThreshold()
{
  return contourThreshold;
}

void contourMethod::setDecisionThreshold(float thres)
{
  contourThreshold=thres;
}

void thresholdAdaptifAlgo::detectPlan(char* folder)
{
  
  // lire la liste de fichier
  DIR *dp;
  struct dirent *ep;
  vector<string> files;
  
  dp = opendir(folder);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      if(strcmp(ep->d_name,".")==0 || strcmp(ep->d_name,"..")==0 || strcmp(ep->d_name,"Thumbs.db")==0)
	continue;
      string pathname=(string)folder+"/"+(string) ep->d_name;
      files.push_back(pathname);
    }
    
    (void) closedir(dp);
  } else
    cout<< ERR_DIR_OPEN<<endl;
  sort(files.begin(),files.end());
  
  
  // chercher le plan
  vector<int> planList;
  if(files.size()<=1)
    return;
  double* distanceArray=new double[files.size()-1];  
  planList.push_back(0);
  double maxDistance=0;
  for(int i=1; i<files.size(); i++){
    cout<<files[i-1]<<" vs "<<files[i]<<endl;
    Mat image1=imread(files[i-1],1);
    Mat image2=imread(files[i],1);
    double distance=algo->calculateDistance(image1,image2);
    if(maxDistance<distance)
      maxDistance=distance;
    distanceArray[i-1]=distance;    
  }
  //normalise distanceArray
  //double* distanceArrayNormal=new distanceArray[files.size()-1];  
  Mat mat=Mat::zeros(1,files.size()-1,CV_32FC1);
  double mean,std;
//   meanStdDev(mat,mean,std);
  for(int i=0; i<mat.cols; i++){
    mat.at<float>(0,i)=distanceArray[i]/maxDistance;
    normaledDiffNoPlan.push_back(distanceArray[i]/maxDistance);
  }
  int size=15;
  int Tp=5;
  for(int i=size/2; i<files.size()-size/2; i++){
    // calculer distance ici
    Mat slidingwindowdiff=Mat::zeros(0,size,CV_32FC1);
    getRectSubPix(mat,Size(size,1),Point2f(i,0),slidingwindowdiff);
    
    // divise en 2
    Mat matleft=Mat::zeros(0,size/2,CV_32FC1);
    Mat matright=Mat::zeros(0,size/2,CV_32FC1);
    Mat matN=Mat::zeros(0,size-1,CV_32FC1);
    deviceMat2(slidingwindowdiff,matleft,matright,matN);
    
    bool isPlan=false;    
    Scalar meanl,sdl;
    Scalar meanr,sdr;
    
    meanStdDev(matleft,meanl,sdl);
    meanStdDev(matright,meanr,sdr);
    
    
    double vl=meanl[0]+Tp*sqrt(sdl[0]);
    double vr=meanr[0]+Tp*sqrt(sdr[0]);
    double vmax=vl<vr?vr:vl;
    if(mat.at<float>(0,i)>vmax){
      isPlan=true;
      planList.push_back(i);      
      normaledDiffNoPlan.erase(normaledDiffNoPlan.begin()+i);
      
      Scalar meanN,sdN;
      meanStdDev(matN,meanN,sdN);
      
      double newThres=(meanN[0]+Tp*sqrt(sdN[0]))*maxDistance;
      //cout<<newThres
      thresh.push_back(newThres);
    }
  }
  // print out the result
  cout<<"Number of plans:"<<planList.size()<<endl;
  cout<<"List of plan:"<<endl;
  for(int i=0; i<planList.size(); i++){
    cout<<planList[i]<<", ";
  }
  cout <<endl;
  delete algo;
}

void thresholdAdaptifAlgo::deviceMat2(const Mat& mat,Mat& left,Mat& right,Mat& matN){
  int size=mat.cols/2;
  for(int i=0; i<size; i++){
    left.at<float>(0,i)=mat.at<float>(0,i);
    right.at<float>(0,i)=mat.at<float>(0,size+1+i);
    matN.at<float>(0,i)=mat.at<float>(0,i);
    matN.at<float>(0,size+i)=mat.at<float>(0,size+1+i);
  }
}
void matrixConfusion::calculate()
{
  recall[0]=matrix[0][1]/(double)(matrix[0][0]+matrix[0][1]);
  recall[1]=matrix[1][0]/(double)(matrix[1][0]+matrix[1][1]);
  recall[2]=0.5*(recall[0]+recall[1]);
  
  precision[0]=matrix[1][0]/(double)(matrix[0][0]+matrix[1][0]);
  precision[1]=matrix[0][1]/(double)(matrix[0][1]+matrix[1][1]);
  precision[2]=0.5*(precision[0]+precision[1]);
}

matrixConfusion::matrixConfusion()
{
  for(int i=0; i<2; i++)
    for(int j=0; j<2; j++)
      matrix[i][j]=0;  
}
void matrixConfusion::print()
{
  calculate();
  cout<<"Confusion Matrix:"<<endl;
  cout<<"\tPlan\tNoPlan"<<endl;
  for(int i=0; i<2; i++){
    if(i==0) cout<<"Plan:\t";
    else cout<<"NoPlan:\t";
    for(int j=0; j<2; j++){
      cout<<matrix[i][j]<<"\t";
    }
    cout<<endl;
  }
  cout<<"Statistics:"<<endl;
  cout<<"Recall:\t";
  cout<<"Plan:\t"<<recall[0];
  cout<<"NoPlan:\t"<<recall[1];
  cout<<"Average:\t"<<recall[2];
  cout<<endl;
  cout<<"Precision:\t";
  cout<<"Plan:\t"<<precision[0];
  cout<<"NoPlan:\t"<<precision[1];
  cout<<"Average:\t"<<precision[2];
  cout<<endl;
}
void matrixConfusion::setReference(int* arrayRef, int length)
{
  this->ref=new int[length];
  this->reflength=length;
  memcpy(ref,arrayRef,length*sizeof(int));
}
void matrixConfusion::setValue(int frameInputId, bool isPlan)
{
  bool found=false;
  for(int i=0; i<this->reflength; i++){
    if(this->ref[i]<frameInputId)
      break;
    if(this->ref[i]==frameInputId){
      found=true;
    }
  }
  if(found==true){
    if(isPlan==true){
      this->matrix[0][0]++;
    }else
      this->matrix[0][1]++;
  }else{
    if(isPlan==true){
      this->matrix[1][0]++;
    }else
      this->matrix[1][1]++;
  }
}

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
#include <algorithm>
#include "tf-idf.h"
#include "pca.h"
using namespace std;
using namespace cv;
void PCAConverter::import(vector<category>& categorylist,vector<string>& dict)
{
    int width=dict.size();
    int height=0;
    for (int i=0; i<categorylist.size(); i++) {
        height+=categorylist[i].numberOfDoc;
    }
    cout<<"matrix: "<<height<<"*"<<width<<endl;
    Mat data=Mat::zeros(height,width,CV_32FC1);
    // load data to matrix
    int dataRowIndex=-1;
    for (int i=0; i<categorylist.size(); i++)
    {
        for (int j=0; j<categorylist[i].documentList.size(); j++) {
            int itemIndex=0;
            dataRowIndex++;
            document* doc=&categorylist[i].documentList[j];
            for (int colIndex=0; colIndex<dict.size(); colIndex++) {
                if (itemIndex >= doc->word.size()) {
                    //cout<<itemIndex<<"end of doc"<<endl;
                    data.at<float>(dataRowIndex,colIndex)=0;
                    continue;
                }
                //cout<<dict[i]<<" vs "<<this->word[itemIndex] << " at "<<itemIndex << ": "<<tfidf[itemIndex]<<endl;
                while (dict[colIndex].compare(doc->word[itemIndex])>0) {
                    itemIndex++;
                    if (itemIndex >=doc->word.size())
                        break;
                }
                if (itemIndex < doc->word.size()) {
                    int compared=dict[i].compare(doc->word[itemIndex]);
                    if (compared==0) {
                        data.at<float>(dataRowIndex,colIndex) = doc->tfidf[itemIndex];
                        itemIndex++;
                    } else data.at<float>(dataRowIndex,colIndex)=0;
                } else {
                    data.at<float>(dataRowIndex,colIndex)=0;
                }

            }
        }
    }
    // pca to convert
    maxComponent=height-1;
    PCA pca(data,Mat(),CV_PCA_DATA_AS_ROW,maxComponent);
    eigenVectors=pca.eigenvectors.clone();
    mean=pca.mean.clone();
}

void PCAConverter::load()
{
  FileStorage pcafile("pcaoutput.xml",FileStorage::READ);
  cv::read(pcafile,"eigenVectors",eigenVectors);
  cv::read(pcafile,"mean",mean);
  pcafile.release();
}
void PCAConverter::save()
{
  FileStorage pcafile("pcaoutput.xml",FileStorage::WRITE);
  cv::write(pcafile,"eigenVectors",eigenVectors);
  cv::write(pcafile,"mean",mean);
  pcafile.release();
}
void PCAConverter::getProjection(const Mat& data,Mat& dataproject)
{
   Mat substraction=data.clone();
   for(int i=0; i<data.rows; i++){
    for(int j=0; j<data.cols; j++){
      substraction.at<float>(i,j)-=mean.at<float>(0,j);
    }
   }
   multiply(substraction,eigenVectors,dataproject);
}

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
#include <fstream>
#include <algorithm>
#include "tf-idf.h"
#include "pca.h"
using namespace std;
using namespace cv;
void PCAConverter::import(vector<category>& categorylist,vector<string>& dict,Mat& data)
{
    width=dict.size();
    height=0;
    for (int i=0; i<categorylist.size(); i++) {
        height+=categorylist[i].numberOfDoc;
    }
    cout<<"matrix: "<<height<<"*"<<width<<endl;
    data=Mat::zeros(height,width,CV_32FC1);
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
                    int compared=dict[colIndex].compare(doc->word[itemIndex]);
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

}
void PCAConverter::calculatePCA(Mat& data) {
    // pca to convert
    maxComponent=maxComponent<height-1?maxComponent:height-1;
    cout<<"max component:"<<maxComponent<<endl;
    PCA pca(data,Mat(),CV_PCA_DATA_AS_ROW,maxComponent);
    cout<<"end pca"<<endl;
    eigenVectors=pca.eigenvectors.clone();
    mean=pca.mean.clone();
    Scalar sumeigen=sum(pca.eigenvalues);
    cout<<"description:"<<sumeigen[0]<<"="<<sumeigen[0]/4647<<endl;
}
void PCAConverter::loadSimple()
{
    char infilestr[255];
    sprintf(infilestr,"%s/pcaoutput.xml",outdir);
    FileStorage pcafile(infilestr,FileStorage::READ);
    cv::read(pcafile["eigenVectors"],eigenVectors);
    cv::read(pcafile["mean"],mean);
    pcafile.release();
}
void PCAConverter::saveSimple()
{
    char outfilestr[255];
    sprintf(outfilestr,"%s/pcaoutput.xml",outdir);
    FileStorage pcafile(outfilestr,FileStorage::WRITE);
    cv::write(pcafile,"eigenVectors",eigenVectors);
    cv::write(pcafile,"mean",mean);
    pcafile.release();
}
void PCAConverter::getProjection(Mat& data,Mat& dataproject)
{
    Mat substraction=data.clone();
    for (int i=0; i<data.rows; i++) {
        for (int j=0; j<data.cols; j++) {
            substraction.at<float>(i,j)-=mean.at<float>(0,j);
        }
    }
    Mat eigenTrans;
    transpose(eigenVectors,eigenTrans);
    cout<<substraction.rows<<"*"<<substraction.cols<<endl;
    cout<<eigenTrans.rows<<"*"<<eigenTrans.cols<<endl;
    //matMulDeriv();
    //multiply(substraction,eigenTrans,dataproject);
    dataproject=substraction*eigenTrans;
}
#define NUMBER_EIGENS nEigens
void PCAConverter::calculatePCA(IplImage** data)
{
    //doPCA
    int i;
    CvTermCriteria calcLimit;
    CvSize faceImgSize;

    // set the number of eigenvalues to use
    nEigens = 5;
    //  nEigens=2*nTrainFaces;

    // allocate the eigenvector images
    faceImgSize.width = data[0]->width;
    faceImgSize.height = data[0]->height;
    cout<<faceImgSize.width<<"*"<<faceImgSize.height<<endl;
    eigenVectArr = (IplImage**) cvAlloc(sizeof (IplImage*) * nEigens);
    for (i = 0; i < nEigens; i++) {
        cout<<i<<" ";
        eigenVectArr[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);
    }

    // allocate the eigenvalue array
    eigenValMat = cvCreateMat(1, nEigens, CV_32FC1);

    // allocate the averaged image
    pAvgTrainImg = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

    // set the PCA termination criterion
    calcLimit = cvTermCriteria(CV_TERMCRIT_ITER, nEigens, 1);

    // compute average image, eigenvalues, and eigenvectors
    cvCalcEigenObjects(
        ndocs,
        (void*) data,
        (void*) eigenVectArr,
        CV_EIGOBJ_NO_CALLBACK,
        0,
        0,
        &calcLimit,
        pAvgTrainImg,
        eigenValMat->data.fl);

    cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
}
void PCAConverter::getProjection(IplImage** data, CvMat*& dataproject)
{
    // project the training images onto the PCA subspace
    dataproject = cvCreateMat(ndocs, NUMBER_EIGENS, CV_32FC1);
    int offset = dataproject->step / sizeof (float);
    for (int i = 0; i < ndocs; i++) {
        //int offset = i * nEigens;
        cvEigenDecomposite(
            data[i],
            NUMBER_EIGENS,
            eigenVectArr,
            0, 0,
            pAvgTrainImg,
            //projectedTrainFaceMat->data.fl + i*nEigens);
            dataproject->data.fl + i * offset);
    }
}
void PCAConverter::import(vector< category >& categorylist, vector< string >& dict, IplImage**& data)
{
    width=dict.size();
    height=0;
    for (int i=0; i<categorylist.size(); i++) {
        height+=categorylist[i].numberOfDoc;
    }
    ndocs=height;
    cout<<width<<"*"<<height;
    // allocate the face-image array and person number matrix
    data = (IplImage **) cvAlloc(ndocs * sizeof (IplImage *));
//     personNumTruthMat = cvCreateMat(1, ndocs, CV_32SC1);

//     personNames.clear(); // Make sure it starts as empty.
//     nPersons = 0;
    //load data to matrix
    int dataRowIndex=-1;
    for (int i=0; i<categorylist.size(); i++)
    {
        for (int j=0; j<categorylist[i].documentList.size(); j++) {
            int itemIndex=0;
            dataRowIndex++;
            document* doc=&categorylist[i].documentList[j];
            IplImage* docimage = cvCreateImage(cvSize(width, 1), IPL_DEPTH_32F, 1);
            //personNumTruthMat[dataRowIndex]=categorylist[i].categoryid;
            for (int colIndex=0; colIndex<dict.size(); colIndex++) {
                if (itemIndex >= doc->word.size()) {
                    //cout<<itemIndex<<"end of doc"<<endl;
                    //data.at<float>(dataRowIndex,colIndex)=0;
                    docimage->imageData[colIndex]=0;
                    continue;
                }
                //cout<<dict[i]<<" vs "<<this->word[itemIndex] << " at "<<itemIndex << ": "<<tfidf[itemIndex]<<endl;
                while (dict[colIndex].compare(doc->word[itemIndex])>0) {
                    itemIndex++;
                    if (itemIndex >=doc->word.size())
                        break;
                }
                if (itemIndex < doc->word.size()) {
                    int compared=dict[colIndex].compare(doc->word[itemIndex]);
                    if (compared==0) {
                        //data.at<float>(dataRowIndex,colIndex) = doc->tfidf[itemIndex];
                        docimage->imageData[colIndex]=doc->tfidf[itemIndex];
                        itemIndex++;
                    } else {
                        //data.at<float>(dataRowIndex,colIndex)=0;
                        docimage->imageData[colIndex]=0;
                    }
                } else {
                    //data.at<float>(dataRowIndex,colIndex)=0;
                    docimage->imageData[colIndex]=0;
                }
            }
            data[dataRowIndex]=docimage;
        }
    }
    cout<<data[0]->width<<"*"<<data[0]->height<<endl;

}

void PCAConverter::load()
{
    CvFileStorage * fileStorage;
    int i;

    // create a file-storage interface
    fileStorage = cvOpenFileStorage("facedata.xml", 0, CV_STORAGE_READ);
    if (!fileStorage) {
        printf("Can't open training database file 'facedata.xml'.\n");
        return ;
    }

    // Load the person names. Added by Shervin.
//     personNames.clear(); // Make sure it starts as empty.
// //     nPersons = cvReadIntByName(fileStorage, 0, "nPersons", 0);
//     if (nPersons == 0) {
//         printf("No people found in the training database 'facedata.xml'.\n");
//         return 0;
//     }
    // Load each person's name.
//     for (i = 0; i < ndocs; i++) {
//         string sPersonName;
//         char varname[200];
//         sprintf(varname, "personName_%d", (i + 1));
//         sPersonName = cvReadStringByName(fileStorage, 0, varname);
//         personNames.push_back(sPersonName);
//     }

    // Load the data
    nEigens = cvReadIntByName(fileStorage, 0, "nEigens", 0);
//     nTrainFaces = cvReadIntByName(fileStorage, 0, "nTrainFaces", 0);
    //*pTrainPersonNumMat = (CvMat *) cvReadByName(fileStorage, 0, "trainPersonNumMat", 0);
    eigenValMat = (CvMat *) cvReadByName(fileStorage, 0, "eigenValMat", 0);
    //projectedTrainFaceMat = (CvMat *) cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
    pAvgTrainImg = (IplImage *) cvReadByName(fileStorage, 0, "avgTrainImg", 0);
    eigenVectArr = (IplImage **) cvAlloc(ndocs * sizeof (IplImage *));
    for (i = 0; i < nEigens; i++) {
        char varname[200];
        sprintf(varname, "eigenVect_%d", i);
        eigenVectArr[i] = (IplImage *) cvReadByName(fileStorage, 0, varname, 0);
    }

    // release the file-storage interface
    cvReleaseFileStorage(&fileStorage);

//     printf("Training data loaded (%d training images of %d people):\n", nTrainFaces, nPersons);
//     printf("People: ");
//     if (nPersons > 0)
//         printf("<%s>", personNames[0].c_str());
//     for (i = 1; i < nPersons; i++) {
//         printf(", <%s>", personNames[i].c_str());
//     }
//     printf(".\n");

    return ;
}
void PCAConverter::save()
{
    CvFileStorage * fileStorage;
    int i;

    // create a file-storage interface
    fileStorage = cvOpenFileStorage("facedata.xml", 0, CV_STORAGE_WRITE);

//     // Store the person names. Added by Shervin.
//     cvWriteInt(fileStorage, "nPersons", nPersons);
//     for (i = 0; i < nPersons; i++) {
//         char varname[200];
//         sprintf(varname, "personName_%d", (i + 1));
//         cvWriteString(fileStorage, varname, personNames[i].c_str(), 0);
//     }

    // store all the data
    cvWriteInt(fileStorage, "nEigens", nEigens);
    cvWriteInt(fileStorage, "nTrainFaces", ndocs);
//     cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0, 0));
    cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0, 0));
    //cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0, 0));
    cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0, 0));
    for (i = 0; i < nEigens; i++) {
        char varname[200];
        sprintf(varname, "eigenVect_%d", i);
        cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0, 0));
    }
#ifdef LDA
    //  if (ldaeigens != NULL) {
    cvWrite(fileStorage, "ldaeigens", ldaeigens, cvAttrList(0, 0));
    //  }
#endif
    // release the file-storage interface
    cvReleaseFileStorage(&fileStorage);
}



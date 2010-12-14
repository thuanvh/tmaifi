
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

void wordindexer::indexer(char* folder, char* fileout, char* dictFile,bool createDictFile)
{
    //read list of folder
    DIR *dp;
    struct dirent *ep;
    vector<string> files;
    cout<<"read folder "<<folder<<endl;
    dp = opendir(folder);
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            //      puts(ep->d_name);
            files.push_back((string) ep->d_name);
        }

        (void) closedir(dp);
    } else
        perror(ERR_DIR_OPEN);
//     for (int i=0; i<files.size(); i++) {
//         cout<<files[i]<<endl;
//     }
    //create category
    for (int i=0; i<files.size(); i++) {
        string categoryname=files[i];
        if (categoryname.compare(".")==0 || categoryname.compare("..")==0) continue;
        category cat;
        cat.categoryid=atoi(categoryname.c_str());
        cat.path=string(folder)+"/"+categoryname;
        cat.indexer();
        categoryList.push_back(cat);
        cout<<"end indexing category "<<categoryname<<endl;
    }
    if (createDictFile) {
        //read category for dictionary
        cout<<"calculate dictionary"<<endl;
        for (int i=0; i<categoryList.size(); i++) {
            //utils::insert(dict,categoryList);
            //utils::merge(dict,categoryList[i].word);
            mergeToDict(categoryList[i]);
        }
        cout<<"end of merging"<<endl;

        cout<<"filtre dict"<<endl;
        filtreDict();
        cout<<"end filtre"<<endl;
    }
//     for (int i=0; i<dict.size(); i++) {
//         cout<<dict[i]<<" ";
//     }
//     cout<<endl;
    Mat pcaProjection;
    //CvMat* pcaProjection;
    if (createDictFile) {
        // out to dictfile
        ofstream dictFilsStream(dictFile);
        writeDict(dictFilsStream);
        dictFilsStream.close();
        PCAConverter pcaCvt;
        Mat data;
        //IplImage** data;
        cout<<"import"<<endl;
        pcaCvt.import(categoryList,dict,data);
        cout<<"calculate pca"<<endl;
        pcaCvt.calculatePCA(data);
        cout<<"save pca"<<endl;
        pcaCvt.saveSimple();
        cout<<"projection"<<endl;
        pcaCvt.getProjection(data,pcaProjection);
    } else {
        ifstream idictfile(dictFile);
        readDict(idictfile,dict);
        idictfile.close();
        PCAConverter pcaCvt;
        Mat data;
        //IplImage** data;
        pcaCvt.import(categoryList,dict,data);
        pcaCvt.loadSimple();
        pcaCvt.getProjection(data,pcaProjection);
    }
    // out to fileout
    ofstream outfile(fileout);
//     writeARFFHeader(outfile);
    writeARFFHeaderPCA(outfile,pcaProjection.cols);
    //writeARFFHeaderPCA(outfile,pcaProjection->cols);
    int rowIndex=0;
    for (int i=0; i<categoryList.size(); i++) {
//         categoryList[i].write(outfile,dict);
        categoryList[i].writePCA(outfile,pcaProjection,rowIndex);
        rowIndex+=categoryList[i].numberOfDoc;
        cout<<"end write cat "<< i<<endl;
    }
    outfile.close();

}
void wordindexer::filtreDict()
{
#define NUMBER_FREQ_MIN 10
#define NUMBER_DOC_MIN 10
#define NUMBER_DOC_MAX (numberOfDoc/2)
    cout<<"Numofdoc:"<<numberOfDoc<<endl;
    cout<<"Before:"<<dict.size()<<endl;
    vector<string>::iterator dictPtr=dict.begin();
    vector<int>::iterator docPtr=dictDocCount.begin();
    vector<int>::iterator freqPtr=dictFreqCount.begin();
    int index=0;
    cout<<"erase:"<<endl;
    int freqmax=0;
    int docmax=0;
    while (dictPtr<dict.end()) {
        freqmax=(dictFreqCount[index]<freqmax)? freqmax:dictFreqCount[index];
        docmax=(dictDocCount[index]<docmax)? docmax:dictDocCount[index];
        if (dictDocCount[index]<NUMBER_DOC_MIN || dictFreqCount[index]<NUMBER_FREQ_MIN || dictDocCount[index]>NUMBER_DOC_MAX) {
            //cout<<dict[index]<<" ";
            dict.erase(dict.begin()+index);
            dictDocCount.erase(dictDocCount.begin()+index);
            dictFreqCount.erase(dictFreqCount.begin()+index);
            continue;
        }
        dictPtr++;
        index++;
    };
    cout<<"Max freq:"<<freqmax<<endl;
    cout<<"Max doc:"<<docmax<<endl;
    cout<<"End:"<<dict.size()<<endl;
}

void wordindexer::mergeToDict(category& cat) {
    vector<string> a=cat.word;
    int dictIndex=0;
    int aIndex=0;
    numberOfDoc+=cat.numberOfDoc;
    while (true) {
        if (aIndex>=a.size()) {
            break;
        }
        if (dictIndex>=dict.size()) {
            for (;aIndex<a.size(); aIndex++) {
                dict.push_back(a[aIndex]);
                dictDocCount.push_back(cat.docCountInCat[aIndex]);
                dictFreqCount.push_back(cat.freqCountInCat[aIndex]);
            }
            break;
        }
        int compare=dict[dictIndex].compare(a[aIndex]);
        if (compare==0) {
            dictDocCount[dictIndex]+=cat.docCountInCat[aIndex];
            dictFreqCount[dictIndex]+=cat.freqCountInCat[aIndex];
            dictIndex++;
            aIndex++;
        } else if (compare<0) {
            dictIndex++;
        } else if (compare>0) {
            dict.insert(dict.begin()+dictIndex,a[aIndex]);
            dictDocCount.insert(dictDocCount.begin() + dictIndex,cat.docCountInCat[aIndex]);
            dictFreqCount.insert(dictFreqCount.begin() + dictIndex,cat.freqCountInCat[aIndex]);
            dictIndex++;
            aIndex++;
        }
    }
}
void category::writePCA(ostream& os, CvMat* mat, int rowindex)
{
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].writePCA(os,mat,rowindex+i);
        os<<this->categoryid<<endl;
    }

}
void category::writePCA(ostream& os, Mat& mat, int rowindex)
{
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].writePCA(os,mat,rowindex+i);
        os<<this->categoryid<<endl;
    }

}
void document::writePCA(ostream& os, Mat& mat, int rowindex)
{
    for (int i=0; i<mat.cols; i++) {
        os<<mat.at<float>(rowindex,i)<<" ";
    }
}
void document::writePCA(ostream& os, CvMat* mat, int rowindex)
{
    for (int i=0; i<mat->cols; i++) {
        os<<mat->data.fl[rowindex * mat->cols + i]<<" ";
    }
}

void wordindexer::writeARFFHeaderPCA(ostream& outfile, int axesNumber)
{
    outfile << "@RELATION tfidf" << endl;
    for (int i = 0; i < axesNumber; i++) {
        outfile << "@ATTRIBUTE axe_" << i << "			real" << endl;
    }
    outfile << "@ATTRIBUTE class 		{";
    int nbrclass = this->categoryList.size();
    for (int i = 1; i <= nbrclass; i++) {
        outfile << i;
        if (i < nbrclass)
            outfile << ",";
    }
    outfile << "}	" << endl;
    outfile << "@DATA" << endl;

}

void wordindexer::readDict(istream& dictFile,vector<string>& dict) {
    dict.clear();
    while (dictFile.good()) {
        string term;
        dictFile>>term;
        if (term.compare("")==0)
            return;
        dict.push_back(term);
    }
}
void wordindexer::writeDict(ostream& dictFile)
{
    for (int i=0; i<dict.size();i++) {
        dictFile<<dict[i]<<endl;
    }
}

// merge 2 sorted array
void utils::merge(vector<string>& dict,const vector<string>& a) {
    int dictIndex=0;
    int aIndex=0;
    while (true) {
        if (aIndex>=a.size()) {
            break;
        }
        if (dictIndex>=dict.size()) {
            for (;aIndex<a.size(); aIndex++) {
                dict.push_back(a[aIndex]);
            }
            break;
        }
        int compare=dict[dictIndex].compare(a[aIndex]);
        if (compare==0) {
            dictIndex++;
            aIndex++;
        } else if (compare<0) {
            dictIndex++;
        } else if (compare>0) {
            dict.insert(dict.begin()+dictIndex,a[aIndex]);
            dictIndex++;
            aIndex++;
        }
    }
}
// binary search
void utils::insert(vector<string>& dict,string word,int& position, bool& inserted) {
    inserted=false;
    position=-1;
    int max=dict.size()-1;
    int min=0;

    while (min<=max) {
        int mid=(max+min)/2;
        int compared=dict[mid].compare(word);
        if (compared==0) {
            position=mid;
            inserted=false;
            return;
        } else   if (compared<0) {
            min=mid+1;
        } else {
            max=mid-1;
        }
    }
    if (position==-1) {
        position=min;
        if (position<dict.size() && position>=0) {
            vector<string>::iterator iter=dict.begin()+position;
            dict.insert(iter,word);
        } else {
            dict.push_back(word);
        }
        inserted=true;
    }
}
void category::indexer()
{
    //read list of folder
    DIR *dp;
    struct dirent *ep;
    vector<string> files;

    dp = opendir(path.c_str());
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            //      puts(ep->d_name);
            files.push_back((string) ep->d_name);
        }

        (void) closedir(dp);
    } else
        perror(ERR_DIR_OPEN);
    // read document
    numberOfDoc=0;
#define maxdoc files.size()
//#define maxdoc 10
    for (int i=0; i<maxdoc; i++) {
        if (files[i].compare(".")==0 || files[i].compare("..")==0) continue;
        document doc;
        doc.documentPath=path+"/"+files[i];
        doc.indexer();
        documentList.push_back(doc);
        numberOfDoc++;
//         cout<<"begin doc count for "<<doc.word.size()<<" words"<<endl;
        // calculate document count
        for (int j=0; j<doc.word.size(); j++) {
            int position;
            bool inserted;
            //cout<<doc.word[j]<<" ";
            utils::insert(this->word,doc.word[j],position,inserted);
            // count document
            if (!inserted) {
                this->docCountInCat[position]++;
                this->freqCountInCat[position]+=doc.countInDoc[j];
            } else {
                if (position<this->docCountInCat.size()) {
                    this->docCountInCat.insert(this->docCountInCat.begin()+position,1);
                    this->freqCountInCat.insert(this->freqCountInCat.begin()+position,doc.countInDoc[j]);
                } else {
                    this->docCountInCat.push_back(1);
                    this->freqCountInCat.push_back(doc.countInDoc[j]);
                }
            }

        }
    }
//     for (int i=0; i<this->word.size(); i++) {
//         cout<<this->word[i]<<":"<<this->docCount[i]<<" ";
//     }
//     cout<<endl;
    // calculate tf-idf
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].calculateTfIdf(this->word,this->docCountInCat,this->numberOfDoc);
    }
}
void category::write(ostream& os,const vector<string>& dict)
{
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].write(os,dict);
        os<<" "<<categoryid<<endl;
        cout<<"=============end document write "<<i<<endl;
    }
//     if (documentList.size()>1)
//         os<<" "<<categoryid<<endl;
}


// calculate tf
void document::indexer()
{
    ifstream ifile(this->documentPath.c_str());
//     cout<<"*********read file "<<this->documentPath<<endl;
    while (ifile.good()) {
        string item;
        ifile>>item;
        //cout<<item<<endl;
        vector<string> itemlist;
        utils::normalizeitem(item,itemlist);
        for (int i=0; i<itemlist.size(); i++) {
            item=itemlist[i];
            //cout<<item<<endl;
            if (!utils::isValid(item))
                continue;
            int position;
            bool inserted;
            utils::insert(word,item,position,inserted);
            if (inserted) {
                if (position>countInDoc.size()) {
                    countInDoc.push_back(1);
                } else {
                    countInDoc.insert(countInDoc.begin()+position,1);
                }
            } else {
                countInDoc[position]++;
            }
        }
    }
    int itemcount=word.size();
    for (int i=0; i<itemcount; i++) {

        tf.push_back(countInDoc[i]/(double)itemcount);
//         cout<<word[i]<<":"<<count[i]<<" "<<tf[i]<<endl;
    }
    ifile.close();
}
void document::write(ostream& os,const vector<string>& dict)
{
    int itemIndex=0;
    for (int i=0; i<dict.size(); i++) {
        if (itemIndex >= this->word.size()) {
            //cout<<itemIndex<<"end of doc"<<endl;
            os<<0<<" ";
            continue;
        }
        //cout<<dict[i]<<" vs "<<this->word[itemIndex] << " at "<<itemIndex << ": "<<tfidf[itemIndex]<<endl;
        while (dict[i].compare(this->word[itemIndex])>0) {
            itemIndex++;
            if (itemIndex >=this->word.size())
                break;
        }
        if (itemIndex < this->word.size()) {
            int compared=dict[i].compare(this->word[itemIndex]);
            if (compared==0) {
                os << this->tfidf[itemIndex];
                itemIndex++;
            } else os<<0;
        } else {
            os<<0;
        }
        os<<" ";
    }
}
void document::calculateTfIdf(const vector<string>& catword,const vector<int>& catDocCount,int catNumberOfDoc)
{
    int docwordIndex=0;
    int catwordIndex=0;
    while (true) {
        if (catwordIndex >= catword.size()) {
            break;
        }
        if (docwordIndex >= word.size()) {
            break;
        }
        //cout<<"compare:"<<catword[catwordIndex]<<" vs "<<word[docwordIndex]<<endl;
        int compared=catword[catwordIndex].compare(word[docwordIndex]);
        if (compared==0) {
            this->tfidf.push_back(this->tf[docwordIndex]* log( catNumberOfDoc / (double)catDocCount[catwordIndex]));
            //cout<<catword[catwordIndex]<<":"<<this->tf[docwordIndex]<<" "<<catNumberOfDoc<<" "<<catDocCount[catwordIndex]<<endl;
            docwordIndex++;
            catwordIndex++;
        } else {
            catwordIndex++;
        }
    }
//     for (int i=0; i<word.size(); i++) {
//         cout<<word[i]<<": tf="<<tf[i]<<" : tfidf="<<this->tfidf[i]<<endl;
//     }
}
bool utils::isValid(const std::string& item)
{
#define MINSIZE 3
#define MAXSIZE 20
    if (!(item.length()>MINSIZE && item.length()<MAXSIZE)) {
        return false;
    }
    if (isdigit(item[0]))
        return false;
    // remove digit
    int i=0;
    for (i=0; i<item.length(); i++) {
        if (!isdigit(item[i]) && item[i]!='e')
            break;
    }
    if (i==item.length())
        return false;

    return true;
}
void utils::normalizeitem(string& item,vector<string>& list)
{
    std::transform(item.begin(), item.end(), item.begin(), ::tolower);
    int start=-1;
    int stop=-1;

    for (int i=0; i<item.length(); i++) {
        if (!isalnum(item[i])) {
            //item.erase(i,1);
            if (start!=-1) {
                stop=i;
                list.push_back(item.substr(start,stop-start));
                stop=start=-1;
            }
        } else {
            if (start==-1)
                start=i;
        }
    }
    if (start>=0 && stop==-1)
        list.push_back(item.substr(start,item.length()-start));
}
void wordindexer::writeARFFHeader(ostream& outfile)
{
    outfile << "@RELATION tfidf" << endl;
    for (int i = 0; i < dict.size(); i++) {
        outfile << "@ATTRIBUTE " << dict[i] << "			real" << endl;
    }
    outfile << "@ATTRIBUTE class 		{";
    int nbrclass = this->categoryList.size();
    for (int i = 1; i <= nbrclass; i++) {
        outfile << i;
        if (i < nbrclass)
            outfile << ",";
    }
    outfile << "}	" << endl;
    outfile << "@DATA" << endl;


}




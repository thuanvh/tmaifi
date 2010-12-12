
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
    for (int i=0; i<files.size(); i++) {
        cout<<files[i]<<endl;
    }
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

    //read category for dictionary
    cout<<"calculate dictionary"<<endl;
    for (int i=0; i<categoryList.size(); i++) {
        //utils::insert(dict,categoryList);
        utils::merge(dict,categoryList[i].word);
    }
    cout<<"end of merging"<<endl;
//     for (int i=0; i<dict.size(); i++) {
//         cout<<dict[i]<<" ";
//     }
//     cout<<endl;
    if (createDictFile) {
        // out to dictfile
        ofstream dictFilsStream(dictFile);
        writeDict(dictFilsStream);
        dictFilsStream.close();
    }else{
	ifstream idictfile(dictFile);
	readDict(idictfile,dict);
	idictfile.close();
    }
    // out to fileout
    ofstream outfile(fileout);
    writeARFFHeader(outfile);
    for (int i=0; i<categoryList.size(); i++) {
        categoryList[i].write(outfile,dict);
        cout<<"end write cat "<< i<<endl;
    }
    outfile.close();

}
void wordindexer::readDict(istream& dictFile,vector<string>& dict){
    dict.clear();
    while(dictFile.good()){
      string term;
      dictFile>>term;
      if(term.compare("")==0)
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
    for (int i=0; i<files.size(); i++) {
        if (files[i].compare(".")==0 || files[i].compare("..")==0) continue;
        document doc;
        doc.documentPath=path+"/"+files[i];
        doc.indexer();
        documentList.push_back(doc);
        numberOfDoc++;
        cout<<"begin doc count for "<<doc.word.size()<<" words"<<endl;
        // calculate document count
        for (int j=0; j<doc.word.size(); j++) {
            int position;
            bool inserted;
            //cout<<doc.word[j]<<" ";
            utils::insert(this->word,doc.word[j],position,inserted);
            // count document
            if (!inserted)
                this->docCount[position]++;
            else {
                if (position<this->docCount.size())
                    this->docCount.insert(this->docCount.begin()+position,1);
                else
                    this->docCount.push_back(1);
            }

        }
    }
//     for (int i=0; i<this->word.size(); i++) {
//         cout<<this->word[i]<<":"<<this->docCount[i]<<" ";
//     }
//     cout<<endl;
    // calculate tf-idf
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].calculateTfIdf(this->word,this->docCount,this->numberOfDoc);
    }
}
void category::write(ostream& os,const vector<string>& dict)
{
    for (int i=0; i<documentList.size(); i++) {
        documentList[i].write(os,dict);
        cout<<"=============end document write "<<i<<endl;
    }
    if (documentList.size()>1)
        os<<" "<<categoryid<<endl;
}
// calculate tf
void document::indexer()
{
    ifstream ifile(this->documentPath.c_str());
    cout<<"*********read file "<<this->documentPath<<endl;
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
                if (position>count.size()) {
                    count.push_back(1);
                } else {
                    count.insert(count.begin()+position,1);
                }
            } else {
                count[position]++;
            }
        }
    }
    int itemcount=word.size();
    for (int i=0; i<itemcount; i++) {

        tf.push_back(count[i]/(double)itemcount);
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

    // remove digit
    int i=0;
    for (i=0; i<item.length(); i++) {
        if (!isdigit(item[i]))
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




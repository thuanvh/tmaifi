#ifndef TF_IDF
#define TF_IDF
#include <iostream>
#include <vector>
using namespace std;

#define ERR_DIR_MISSING "Error in command line"
#define ERR_DIR_OPEN "Error in open dir"

#define TYPE_TRAIN_STR "train"
#define TYPE_TEST_STR "test"

#define TYPE_TRAIN 0
#define TYPE_TEST 1


class document{
public:
  int documentId;
  string documentName;
  string documentPath;
  vector<string> word;
  vector<int> count;
  vector<double> tf;
  vector<double> tfidf;
  int wordtotal;
  void indexer();
  void write(ostream& os,const vector<string>& dict);
  void calculateTfIdf(const vector<string>& catword,const vector<int>& catDocCount,int catNumberOfDoc);
};

class category{
public:
  int categoryid;
  vector<string> word;
  vector<int> docCount;
  vector<document> documentList;
  int numberOfDoc;
  string path;
  void indexer();
  void write(ostream& os,const vector<string>& dict);
};
class dictionary{
public:
  vector<string> list;
};
class wordindexer{
public:
  //dictionary dict;
  vector<string> dict;
  vector<category> categoryList;
  void indexer(char* folder,char* fileout,char* dictFile,bool createDictFile);
  void writeARFFHeader(ostream& outfile);
  void writeDict(ostream& dictFile);
  void readDict(istream& dictFile,vector<string>& dict);
};
class utils{
public:
  static void insert(vector<string>& dict,string word,int& position, bool& success);
  static void merge(vector<string>& dict,const vector<string>& a);
  static void normalizeitem(string& item,vector<string>& list);
  static bool isValid(const string& item);
};
#endif

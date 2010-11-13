
#include <vector>
#include <map>
#include "texture.h"
#include <opencv/cvaux.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


using namespace cv;
using namespace std;

/*
 * Apprendtissage a partir d'une serie d'image de peau
 */
void extraitFeatureVector(const char* dirPath, const char* name, int graySize) {

  //  // load list of file in dir
  //  DIR *dp;
  //  struct dirent *ep;
  //  vector<string> files;
  //  vector<string> classes;
  //
  //  dp = opendir(dirPath);
  //  if (dp != NULL) {
  //    while (ep = readdir(dp)) {
  //      //      puts(ep->d_name);
  //      files.push_back((string) ep->d_name);
  //    }
  //    (void) closedir(dp);
  //  } else
  //    perror(ERR_DIR_OPEN);

  // read list of file and its class
  ifstream listFileStream;
  char listFileStr[255];
  sprintf(listFileStr, "%s/listfile.txt", dirPath);
  listFileStream.open(listFileStr);

  char outFileName[255];
  sprintf(outFileName, "%s.texture.arff", name);
  ofstream outfile;
  outfile.open(outFileName);

  //  #define NUMBER_EIGENS 100
  const int nombre_para = 6 * 48;
  outfile << "@RELATION face" << endl;
  for (int i = 0; i < nombre_para; i++) {
    outfile << "@ATTRIBUTE axis_" << i << "			real" << endl;
  }
  outfile << "@ATTRIBUTE class 		{";
  int nbrclass = 40;
  for (int i = 1; i <= nbrclass; i++) {
    outfile << "s" << i;
    if (i < nbrclass)
      outfile << ",";
  }
  outfile << "}	" << endl;
  outfile << "@DATA" << endl;

  //int size = 32;
  // initialize list of matrix
  double*** concurrenceArray = new double**[NUM_MATRIX];
  for (int i = 0; i < NUM_MATRIX; i++) {
    concurrenceArray[i] = new double*[graySize];
    for (int j = 0; j < graySize; j++) {
      concurrenceArray[i][j] = new double[graySize];
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }
  while (listFileStream.good()) {
    string filename;
    string classname;
    listFileStream >> filename;
    listFileStream >> classname;
    //    files.push_back(filename);
    //    classes.push_back(classname);

    Mat src;
    // calculer list of feature vector
    //    for (int fileindex = 0; fileindex < files.size(); fileindex++) {
    //      string filename = files.at(fileindex);
    if (filename == "")
      continue;
    bool debug = false;
    //    if (classname.compare("weave")==0 || classname.compare("wood")==0)
    //      continue;
    //    if (filename.compare("weave_000_06.pgm") == 0) {
    //      debug = true;
    //    }
    string filePathSrc = string(dirPath);
    filePathSrc += "/" + filename;
    cout << filePathSrc << endl;

    if (!(src = imread(filePathSrc, 0)).data)
      continue;

    cout << "size=" << graySize << endl;
    reduireNiveauDeGris(src, graySize);
    if (debug) {
      cout << "end niveau de gris" << endl;
      getchar();
    }

    int deltax = src.cols / 2;
    int deltay = src.rows / 3;
    for (int x = 0; x < src.cols; x += deltax) {
      for (int y = 0; y < src.rows-1; y += deltay) {
        Mat region;
        getRectSubPix(src, Size(deltax, deltay), Point2f(x + deltax / 2, y + deltay / 2), region);
        setZero(concurrenceArray, graySize);


        if (debug) {
          cout << "begin calcul matrix" << endl;
          for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
            printMatrix(concurrenceArray[matrixIndex], graySize);
          }
          getchar();
        }
        try {
          //    double*** matrices = calculerMatrixCooccurence(src, size);
          calculerMatrixCooccurence(region, concurrenceArray, graySize);
        } catch (...) {
          continue;
        }
        if (debug) {
          cout << "end calcul matrix" << endl;
          getchar();
        }
        //    cout << "begin extract" << endl;
        cout << filename << " " << classname << " ";
        //    outfile << filename << " " << classname << " ";


        for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
          //      printMatrix(concurrenceArray[matrixIndex], size);
          vector<double> vvalue;
          extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, &outfile, vvalue);
          //      getchar();
        }

        //    cout << "end extract" << endl;
        //    }
      }
    }
    outfile << " " << classname;
    outfile << endl;
  }
  listFileStream.close();
  cout << "free memory" << endl;
  freeMatrix(concurrenceArray, graySize);
  outfile.close();
}

void printVector(const vector<double>& vector) {
  for (int i = 0; i < vector.size(); i++) {
    cout << vector[i] << " ";
  }
  cout << endl;
}

void printMatrix(double** dirPath, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      cout << dirPath[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void learning(const char* dirPath, const char* name, int graySize) {
  extraitFeatureVector(dirPath, name, graySize);
}

void separateCrossTesting(const char* filename, const char* fileLearn, const char* fileTest, int percent) {

  ofstream ofFileLearn;
  ofFileLearn.open(fileLearn);
  ofstream ofFileTest;
  ofFileTest.open(fileTest);
  ifstream ifFile;
  ifFile.open(filename);
  srand(time(NULL));
  while (ifFile.good()) {
    string line;
    getline(ifFile, line);
    if (rand() % 100 < percent) {
      ofFileLearn << line << endl;
    } else {
      ofFileTest << line << endl;
    }
  }
  ofFileTest.close();
  ofFileLearn.close();
  ifFile.close();
}

void reduireNiveauDeGris(Mat& image, int size) {
  double a = (double) size / 256;
  //  cout<<"a="<<a;
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      image.at<uchar > (i, j) = (uchar) (a * image.at<uchar > (i, j));
      //      cout<<(int)image.at<uchar > (i, j)<<" ";
    }
    //    cout<<endl;
  }
  //  getchar();
}

void setZero(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < graySize; j++) {
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }
}

void freeVector(vector<int>** vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
  delete[] vector;
}

void freeMatrix(double** matrix, int size) {
  for (int i = 0; i < size; i++) {
    delete[] matrix[i];
  }
  delete matrix;
}

void freeMatrix(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {

    for (int j = 0; j < graySize; j++) {
      //      cout<<i<<","<<j<<" ";
      delete[] concurrenceArray[i][j]; // = new double[graySize];
    }
    delete[] concurrenceArray[i]; // = new double*[graySize];
  }
  //  delete[] concurrenceArray;
}
// calculer 8 matrices de co-occurences

void calculerMatrixCooccurence(const Mat& image, double*** concurrenceArray, int graySize) {

  // convertir image a partir de 256 a size
  int deltaX[NUM_MATRIX] = {1, 1, 0, -1, 2, 2, 0, -2};
  int deltaY[NUM_MATRIX] = {0, 1, 1, 1, 0, 2, 2, 2};
  int numberOfPair[NUM_MATRIX] = {0};

  //  // initialize list of matrix
  //  double*** concurrenceArray = new double**[NUM_MATRIX];
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    concurrenceArray[i] = new double*[graySize];
  //    for (int j = 0; j < graySize; j++) {
  //      concurrenceArray[i][j] = new double[graySize];
  //      for (int k = 0; k < graySize; k++) {
  //        concurrenceArray[i][j][k] = 0;
  //      }
  //    }
  //  }

  //  cout << "begin start" << endl;
  // calculer matrix of concurrence
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      for (int k = 0; k < NUM_MATRIX; k++) {
        int ia = i + deltaY[k];
        int ja = j + deltaX[k];
        if (ia < image.rows && ja < image.cols) {
          int g1 = image.at<uchar > (i, j);
          int g2 = image.at<uchar > (ia, ja);
          //          cout<<g1<<"-"<<g2<<" ";
          concurrenceArray[k][g1][g2]++;
          //          cout<<concurrenceArray[k][g1][g2]<<" ";
          numberOfPair[k]++;
        }
      }
    }
  }

  //  cout << "begin divise" << endl;
  //  for (int i = 0; i < NUM_MATRIX; i++) {
  //    cout << numberOfPair[i] << " ";
  //  }
  //  cout << endl;
  // normalize matrix of concurrence
  for (int i = 0; i < NUM_MATRIX; i++) {
    //    cout<<i<<endl;
    for (int j = 0; j < graySize; j++) {
      //      cout<<j<<endl;
      for (int k = 0; k < graySize; k++) {
        //        cout << concurrenceArray[i][j][k] << " ";
        concurrenceArray[i][j][k] /= numberOfPair[i];

      }
      //      cout << endl;
    }
    //    cout<<endl;
  }
  //  cout << "end divise" << endl;

  //  return concurrenceArray;
}

// calculer 14 parametres
// sauver le vecteur de caracteristique

void extraitCaracteristicVector(double** mat, int size, ostream* ofs, vector<double>& vvalue) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);
  double mean = para_mean(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);

  double sum_avg = para_sum_average(mat, size);

  vvalue.push_back(para_angular_second_moment(mat, size));
//  vvalue.push_back(para_constrast(mat, size));
  vvalue.push_back(para_entropy(mat, size));
  vvalue.push_back(para_correlation(mat, size, meani, meanj, vari, varj));
  vvalue.push_back(para_dissimilarity(mat, size));
  vvalue.push_back(para_energy(mat, size));
  vvalue.push_back(para_homogenety(mat, size));

  //  vvalue.push_back(sum_avg);
  //  vvalue.push_back(para_sum_entropy(mat, size));
  //  vvalue.push_back(para_sum_of_squares__variance(mat, size, mean));
  //  vvalue.push_back(para_sum_variance(mat, size, sum_avg));
  //  vvalue.push_back(para_difference_entropy(mat, size));
  //  vvalue.push_back(para_difference_variance(mat, size));

  //  vvalue.push_back(para_shade(mat, size, meani, meanj));
  //  vvalue.push_back(para_prominence(mat, size, meani, meanj));

  // normalize value
  double vmean = 0;
  for (int i = 0; i < vvalue.size(); i++) {
    vmean += vvalue[i];
  }
  vmean /= vvalue.size();

  double sd = 0;
  for (int i = 0; i < vvalue.size(); i++) {
    sd += pow(vvalue[i] - vmean, 2);
  }
  sd = sqrt(sd / vvalue.size());


  for (int i = 0; i < vvalue.size(); i++) {
    vvalue[i] = (vvalue[i] - vmean) / sd;
  }

  // output to file
  if (ofs != NULL) {
    for (int i = 0; i < vvalue.size(); i++) {
      (*ofs) << vvalue[i] << " ";
    }
  }
}
//

void loadVector() {

}
//

double para_angular_second_moment(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }
  return value;
}

double para_constrast(double** mat, int size) {
  double value = 0;
  int number_of_gray = size;
  for (int n = 0; n < number_of_gray; n++) {
    float sub = 0;
    for (int i = 0; i < number_of_gray; i++) {
      for (int j = 0; j < number_of_gray; j++) {
        if (abs(i - j) == n) {
          sub += mat[i][j];
        }
      }
    }
    value += n * n*sub;
  }
  return value;
}

double para_mean(double** mat, int size) {
  double mean = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      mean += i * j * mat[i][j];

    }
  }
  return mean;
}

double para_meani(double** mat, int size) {
  // mean
  double meani = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      meani += i * mat[i][j];

    }
  }
  return meani;
}

double para_meanj(double** mat, int size) {
  // mean

  double meanj = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      meanj += j * mat[i][j];
    }
  }
  return meanj;
}

double para_vari(double** mat, int size, double meani) {
  // variance
  double vari = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      vari += mat[i][j] * pow(i - meani, 2);

    }
  }
  return vari;
}

double para_varj(double** mat, int size, double meanj) {
  // variance

  double varj = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      varj += mat[i][j] * pow(j - meanj, 2);
    }
  }
  return varj;
}

double para_correlation(double** mat, int size, double meani, double meanj, double vari, double varj) {

  // correlation
  double corr = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      corr += mat[i][j]*(i - meani)*(j - meanj) / sqrt(vari * varj);
    }
  }
  return corr;
}

double para_correlation(double** mat, int size) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);


  // correlation
  double corr = para_correlation(mat, size, meani, meanj, vari, varj);
  return corr;

}

double para_sum_of_squares__variance(double** mat, int size, double mean) {
  double var = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      var += pow(i - mean, 2) * mat[i][j];

    }
  }
  return var;
}

void para_inverse_difference_moment(double** mat, int size) {

}

double para_sum_average(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    value += k * p_i_add_j;
  }
  return value;
}

double para_sum_variance(double** mat, int size, double sum_e) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    value += pow(k - sum_e, 2) * p_i_add_j;
  }
  return value;
}

double para_sum_entropy(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;
  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }
    if (p_i_add_j != 0)
      value += p_i_add_j * log(p_i_add_j);
  }
  return -value;
}

double para_entropy(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (mat[i][j] != 0)
        value += mat[i][j] * log(mat[i][j]);
    }
  }
  return -value;
}

double para_difference_variance(double** mat, int size) {
  double value = 0;
  double p_i_sub_j = 0;
  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }
    value += pow(k, 2) * p_i_sub_j;
  }
  return value;
}

double para_difference_entropy(double** mat, int size) {
  //int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_sub_j = 0;
  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }
    if (p_i_sub_j != 0)
      value += p_i_sub_j * log(p_i_sub_j);
  }
  return -value;
}

void para_info_measure_of_correlation_1(double** mat, int size) {

}

void para_info_measure_of_correlation_2(double** mat, int size) {

}

void para_max_correlation_coeff(double** mat, int size) {

}

double para_dissimilarity(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += mat[i][j] * abs(i - j);
    }
  }
  return value;
}

double para_energy(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }
  return value;
}

double para_homogenety(double** mat, int size) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += (1 / (double) (1 + pow((i - j), 2))) * mat[i][j];
    }
  }
  return value;
}

double para_shade(double** mat, int size, int meani, int meanj) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 3) * mat[i][j];
    }
  }
  return value;

}

double para_prominence(double** mat, int size, int meani, int meanj) {
  double value = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 4) * mat[i][j];
    }
  }
  return value;

}

////double test_hung() {
////  while (1) {
////    hungarian_t prob;
////
////    /*
////    int r[4][4] =  {{  363, 626, 376,  46  },
////                    {  802, 993, 784, 953  },
////                    {  673,  23, 823, 207  },
////                    {  380, 451, 520, 646  }};
////     */
////    double* r;
////    int m, n;
////    m = n = 4;
////
////    //  parse_args(argc,argv);
////
////    //  if(!strlen(input_fname))
////    r = make_random_r(m, n);
////    //  else
////    //    r = make_r_from_ORlib(input_fname,&m,&n);
////
////    if (!r) {
////      puts("got NULL input");
////      exit(-1);
////    }
////
////    hungarian_init(&prob, (double*) r, m, n, HUNGARIAN_MIN);
////    hungarian_print_rating(&prob);
////    hungarian_solve(&prob);
////    hungarian_print_assignment(&prob);
////
////    printf("\nfeasible? %s\n",
////      (hungarian_check_feasibility(&prob)) ? "yes" : "no");
////    printf("benefit: %f\n\n", hungarian_benefit(&prob));
////
////    hungarian_fini(&prob);
////    free(r);
////  }
////}
//
//double*
//make_random_r(size_t m, size_t n) {
//  int i, j;
//  double* r;
//  time_t curr;
//  assert(r = (double*) malloc(sizeof (double) *m * n));
//  curr = time(NULL);
//  srand(curr);
//  for (i = 0; i < m; i++) {
//    for (j = 0; j < n; j++) {
//      r[i * n + j] = 1 + rand() / (double) 1000;
//    }
//  }
//  return (r);
//}

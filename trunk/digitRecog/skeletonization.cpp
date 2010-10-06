#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <string>
#include <cstring>
#include "skeletonization.h"
using namespace cv;
//void ExpansionByDilatation8bits(Mat& BitMap) {
//  unsigned char *LineAux, *Line, *LineAbove, *LineBelow;
//  int x, y, i, j;
//  //Step (1) - it creates an image with domain size (2m + 1) x (2n + 1)
//  Mat BitMapAux;
//  //BitMap.copyTo(BitMapAux);
//  BitMapAux.create(2*)
//  BitMap->Width = 2 * BitMap->Width + 1;
//  BitMap->Height = 2 * BitMap->Height + 1;
//  //Step (2) - it fills out the image with the background color
//  for (y = 0; y < BitMap->Height; y++) {
//    Line = (unsigned char *) BitMap->ScanLine[y];
//    for (x = 0; x < BitMap->Width; x++)
//      Line[x] = 0x0;
//  }
//  //Step (3) - it puts the objects pixels in the expanded image
//  for (y = 0; y < BitMapAux->Height; y++) {
//    j = 2 * y + 1;
//    LineAbove = (unsigned char *) BitMap->ScanLine[j - 1];
//    Line = (unsigned char *) BitMap->ScanLine[j];
//    LineBelow = (unsigned char *) BitMap->ScanLine[j + 1];
//    LineAux = (unsigned char *) BitMapAux->ScanLine[y];
//    for (x = 0; x < BitMapAux->Width; x++)
//      if (LineAux[x] != 0x0) {
//        i = 2 * x + 1;
//        LineAbove[i - 1] = LineAux[x];
//        LineAbove[i] = LineAux[x];
//        LineAbove[i + 1] = LineAux[x];
//        Line[i - 1] = LineAux[x];
//        Line[i] = LineAux[x];
//        Line[i + 1] = LineAux[x];
//        LineBelow[i - 1] = LineAux[x];
//        LineBelow[i] = LineAux[x];
//        LineBelow[i + 1] = LineAux[x];
//      }
//  }
//  delete BitMapAux;
//}

void Skeleton8bits(const Mat& BitMap, Mat& outBitMap) {
  try {
    int min, x, y, X = BitMap.cols - 1, Y = BitMap.rows - 1;
    unsigned char *Line;
    //it creates the matrix M
    int **M = new int * [BitMap.rows]; //to allocate the lines
    for (int y = 0; y < BitMap.rows; y++)
      M[y] = new int [BitMap.cols]; //to allocate the columns
    //it puts the image in a matrix
    for (y = 0; y < BitMap.rows; y++) {
      //Line = (unsigned char *) BitMap->ScanLine[y];
      for (x = 0; x < BitMap.cols; x++)
        M[y][x] = (BitMap.at<uchar > (y, x) == 0x0) ? 1 : 0;
    }
    //Pseudo-code B - it implements the Chessboard distance transformation
    for (y = 1; y < Y; y++) //process in raster mode
      for (x = 1; x < X; x++)
        if (M[y][x] == 1) {
          min = M[y - 1][x - 1];
          if (M[y - 1][x] < min) min = M[y - 1][x];
          if (M[y - 1][x + 1] < min) min = M[y - 1][x + 1];
          if (M[y][x - 1] < min) min = M[y][x - 1];
          M[y][x] = min + 1;
        }
    for (y = Y - 1; y > 0; y--) //process in anti-raster mode
      for (x = X - 1; x > 0; x--)
        if (M[y][x] > 1) {
          min = M[y][x + 1];
          if (M[y + 1][x - 1] < min) min = M[y + 1][x - 1];
          if (M[y + 1][x] < min) min = M[y + 1][x];
          if (M[y + 1][x + 1] < min) min = M[y + 1][x + 1];
          if (min + 1 < M[y][x]) M[y][x] = min + 1;
        }
    //Pseudo-code C - it implements the skeletonization algorithm
    for (y = 1; y < Y; y++) //process in raster mode
      for (x = 1; x < X; x++)
        if (M[y][x] > 0 && abs(M[y - 1][x]) <= M[y][x] && abs(M[y][x - 1]) <= M[y][x] &&
          (M[y + 1][x] <= M[y][x] || M[y - 1][x] < 0) && (M[y][x + 1] <= M[y][x] || M[y][x - 1] < 0))
          M[y][x] *= -1;
    for (y = Y - 1; y > 0; y--) //process in anti-raster mode
      for (x = X - 1; x > 0; x--)
        if (M[y][x] > 0 && ((M[y + 1][x] < 0 && abs(M[y - 1][x]) > M[y][x]) ||
          (M[y][x + 1] < 0 && abs(M[y][x - 1]) > M[y][x])))
          M[y][x] *= -1;
    //Pseudo-code D - it implements the skeleton filter
    int transitions, greater;
    for (y = 1; y < Y; y++) //process in raster mode
      for (x = 1; x < X; x++)
        if (M[y][x] < 0) {
          greater = 0;
          transitions = 0;
          if (M[y - 1][x - 1] < M[y][x]) greater = 1;
          if (M[y - 1][x - 1] < 0 && M[y - 1][x] >= 0) transitions++;
          if (M[y - 1][x] < M[y][x]) greater = 1;
          if (M[y - 1][x] < 0 && M[y - 1][x + 1] >= 0) transitions++;
          if (M[y - 1][x + 1] < M[y][x]) greater = 1;
          if (M[y - 1][x + 1] < 0 && M[y][x + 1] >= 0) transitions++;
          if (M[y][x - 1] < M[y][x]) greater = 1;
          if (M[y][x + 1] < 0 && M[y + 1][x + 1] >= 0) transitions++;
          if (M[y][x + 1] < M[y][x]) greater = 1;
          if (M[y + 1][x + 1] < 0 && M[y + 1][x] >= 0) transitions++;
          if (M[y + 1][x - 1] < M[y][x]) greater = 1;
          if (M[y + 1][x] < 0 && M[y + 1][x - 1] >= 0) transitions++;
          if (M[y + 1][x] < M[y][x]) greater = 1;
          if (M[y + 1][x - 1] < 0 && M[y][x - 1] >= 0) transitions++;
          if (M[y + 1][x + 1] < M[y][x]) greater = 1;
          if (M[y][x - 1] < 0 && M[y - 1][x - 1] >= 0) transitions++;
          if (greater == 1 && transitions < 2) M[y][x] *= -1;
        }
    for (y = Y - 1; y > 0; y--) //process in anti-raster mode
      for (x = X - 1; x > 0; x--)
        if (M[y][x] < 0) {
          greater = 0;
          transitions = 0;
          if (M[y - 1][x - 1] < M[y][x]) greater = 1;
          if (M[y - 1][x - 1] < 0 && M[y - 1][x] >= 0) transitions++;
          if (M[y - 1][x] < M[y][x]) greater = 1;
          if (M[y - 1][x] < 0 && M[y - 1][x + 1] >= 0) transitions++;
          if (M[y - 1][x + 1] < M[y][x]) greater = 1;
          if (M[y - 1][x + 1] < 0 && M[y][x + 1] >= 0) transitions++;
          if (M[y][x - 1] < M[y][x]) greater = 1;
          if (M[y][x + 1] < 0 && M[y + 1][x + 1] >= 0) transitions++;
          if (M[y][x + 1] < M[y][x]) greater = 1;
          if (M[y + 1][x + 1] < 0 && M[y + 1][x] >= 0) transitions++;
          if (M[y + 1][x - 1] < M[y][x]) greater = 1;
          if (M[y + 1][x] < 0 && M[y + 1][x - 1] >= 0) transitions++;
          if (M[y + 1][x] < M[y][x]) greater = 1;
          if (M[y + 1][x - 1] < 0 && M[y][x - 1] >= 0) transitions++;
          if (M[y + 1][x + 1] < M[y][x]) greater = 1;
          if (M[y][x - 1] < 0 && M[y - 1][x - 1] >= 0) transitions++;
          if (greater == 1 && transitions < 2) M[y][x] *= -1;
        }
    //it puts the skeleton points in the image

    for (y = 1; y < Y; y++) {
      //Line = (unsigned char *) BitMap->ScanLine[y];
      for (x = 1; x < X; x++) {
        outBitMap.at<uchar > (y, x) = 255;
        if (M[y][x] < 0)
          outBitMap.at<uchar > (y, x) = 0;
        //BitMap.at<uchar>(y,x)=0xbb;
        //Line[x] = 0xbb;
      }
    }
    //it deletes the matrix M
    for (int y = 0; y < BitMap.rows; y++)
      delete[] M[y]; //to delete the columns
    delete[] M; //to delete the lines
  } catch (...) {
  }
}

void cvRasterSkeleton(Mat& image, int method) {
  //  static const uchar bit_masks[] = "\x01\x02\x04\x08\x10\x20\x40\x80";

  //    CV_FUNCNAME("cvRasterSkeleton");
  //
  //    __BEGIN__;

  Size size = image.size();
  int step = image.step;
  uchar *img = image.data;

  //    CV_CHECK_MASK_IMAGE( image );

  //cvGetImageRawData( image, &img, &step, &size );

  //  switch (method) {
  //    case CV_SKEL_PAVLIDIS:
  icvPavlidis(img, step, size);

  //    default:
  //      return;
  //      //CV_ERROR( CV_StsBadArg, "Incorrect skeleton retrieval method" );
  //  }

  //    __END__;
}

CvStatus icvPavlidis(uchar* img, int step, Size size) {
  uchar table[256];
  uchar nval = 2;
  int width = size.width - 2;
  uchar* img_start = img + step + 1;
  uchar* img_end = img + (size.height - 1) * step + 1;
  int change_flag;
  int deltas[4];
  int skeleton_points = 0;
  int i0, iters = 0;

  deltas[0] = 1;
  deltas[1] = -step;
  deltas[2] = -1;
  deltas[3] = step;

  memset(table, -1, sizeof (table));

  // build table
  for (i0 = 0; i0 < 256; i0++) {
    int i90 = ((i0 >> 2) | (i0 << 6)) & 0xff;
    int i180 = ((i0 >> 4) | (i0 << 4)) & 0xff;
    int i270 = ((i0 >> 6) | (i0 << 2)) & 0xff;

    if ((i0 & 0x11) == 0 && (i0 & 0xe) != 0 && (i0 & 0xe0) != 0) {
      /* 9.3(a) first configuration: 0 degree */
      table[i0] = nval;
    } else if ((i90 & 0x11) == 0 && (i90 & 0xe) != 0 && (i90 & 0xe0) != 0) {
      /* 9.3(a) first configuration: 90 degree */
      table[i0] = nval;
    } else if ((i0 & 0x41) == 0 && (i0 & 0xe3) != 0 && (i0 & 0x80) != 0) {
      /* 9.3(a) second configuration: 0 degree */
      table[i0] = nval;
    } else if ((i90 & 0x41) == 0 && (i90 & 0xe3) != 0 && (i90 & 0x80) != 0) {
      /* 9.3(a) second configuration: 90 degree */
      table[i0] = nval;
    } else if ((i180 & 0x41) == 0 && (i180 & 0xe3) != 0 && (i180 & 0x80) != 0) {
      /* 9.3(a) second configuration: 180 degree */
      table[i0] = nval;
    } else if ((i270 & 0x41) == 0 && (i270 & 0xe3) != 0 && (i270 & 0x80) != 0) {
      /* 9.3(a) second configuration: 270 degree */
      table[i0] = nval;
    }
  }

  // iterative thinning
  do {
    int j; // neighboor index
    change_flag = 0;
    for (j = 0; j < 4; j++) {
      int delta = deltas[j];
      uchar* p;
      for (p = img_start; p < img_end; p += step) {
        int x;
        for (x = 0; x < width; x++) {
          if (p[x] == 1 && p[delta + x] == 0) {
            int val = (p[x + 1]&1) + (p[x - step + 1]&1)*2 +
              (p[x - step]&1)*4 + (p[x - step - 1]&1)*8 +
              ((p[x - 1]&1) + (p[x + step - 1]&1)*2 +
              (p[x + step]&1)*4 + (p[x + step + 1]&1)*8)*16;
            val = table[val];
            change_flag |= val;
            p[x] = (uchar) val;
          }
        }
      }

      if (change_flag) // remove all 255-pixels
      {
        for (p = img_start; p < img_end; p += step) {
          int x;
          for (x = 0; x < width; x++) {
            int val = p[x];
            val &= (val == 255) - 1;
            p[x] = (uchar) val;
          }
        }
      }
    }
    change_flag = change_flag == 255;
  } while (change_flag != 0);

  // set non-zero pixels to 255
  uchar* p;
  for (p = img_start; p < img_end; p += step) {
    int x;
    for (x = 0; x < width; x++) {
      p[x] = (uchar) (p[x] ? 255 : 0);
    }
  }

  return CV_OK;
}


/*
 * C code from the article
 * "Efficient Binary Image Thinning using Neighborhood Maps"
 * by Joseph M. Cychosz, 3ksnn64@ecn.purdue.edu
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include <stdio.h>

//typedef unsigned char Pixel; /* Pixel data type		*/
//
//typedef struct { /* Image control structure	*/
//  short Hres; /*   Horizontal resolution (x)	*/
//  short Vres; /*   Vertical	resolution (y)	*/
//  int Size; /*   Image size (bytes)		*/
//  Pixel *i; /*   Image array		*/
//  Pixel * p[1]; /*   Scanline pointer array	*/
//  /*   Pixel (x,y) is given by	*/
//  /*   image->p[y][x]		*/
//} Image;
//

/* ---- ThinImage - Thin binary image. -------------------------------- */
/*									*/
/*	Description:							*/
/*	    Thins the supplied binary image using Rosenfeld's parallel	*/
/*	    thinning algorithm.						*/
/*									*/
/*	On Entry:							*/
/*	    image = Image to thin.					*/
/*									*/
/* -------------------------------------------------------------------- */


/* Direction masks:			*/
/*   N	   S	 W     E		*/
static int masks[] = {0200, 0002, 0040, 0010};

/*	True if pixel neighbor map indicates the pixel is 8-simple and	*/
/*	not an end point and thus can be deleted.  The neighborhood	*/
/*	map is defined as an integer of bits abcdefghi with a non-zero	*/
/*	bit representing a non-zero pixel.  The bit assignment for the	*/
/*	neighborhood is:						*/
/*									*/
/*				a b c					*/
/*				d e f					*/
/*				g h i					*/

uchar deleteArray[512] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

void ThinImage(Mat& image) {
  int xsize, ysize; /* Image resolution		*/
  int x, y; /* Pixel location		*/
  int i; /* Pass index			*/
  int pc = 0; /* Pass count			*/
  int count = 1; /* Deleted pixel count		*/
  int p, q; /* Neighborhood maps of adjacent*/
  /* cells			*/
  uchar *qb; /* Neighborhood maps of previous*/
  /* scanline			*/
  int m; /* Deletion direction mask	*/

  xsize = image.cols;
  ysize = image.rows;
  qb = (uchar *) malloc(xsize * sizeof (uchar));
  qb[xsize - 1] = 0; /* Used for lower-right pixel	*/

  while (count) { /* Scan image while deletions	*/
    pc++;
    count = 0;

    for (i = 0; i < 4; i++) {

      m = masks[i];

      /* Build initial previous scan buffer.			*/

      p = image.at<uchar > (0, 0) != 0;
      for (x = 0; x < xsize - 1; x++)
        qb[x] = p = ((p << 1)&0006) | (image.at<uchar>(0,x + 1) != 0);

      /* Scan image for pixel deletion candidates.		*/

      for (y = 0; y < ysize - 1; y++) {

        q = qb[0];
        p = ((q << 3)&0110) | (image.at<uchar>(y + 1,0) != 0);

        for (x = 0; x < xsize - 1; x++) {
          q = qb[x];
          p = ((p << 1)&0666) | ((q << 3)&0110) |
            (image.at<uchar>(y + 1,x + 1) != 0);
          qb[x] = p;
          if (((p & m) == 0) && deleteArray[p]) {
            count++;
            image.at<uchar>(y,x) = 0;
          }
        }

        /* Process right edge pixel.			*/

        p = (p << 1)&0666;
        if ((p & m) == 0 && deleteArray[p]) {
          count++;
          image.at<uchar > (y, xsize - 1) = 0;
        }
      }

      /* Process bottom scan line.				*/

      for (x = 0; x < xsize; x++) {
        q = qb[x];
        p = ((p << 1)&0666) | ((q << 3)&0110);
        if ((p & m) == 0 && deleteArray[p]) {
          count++;
          image.at<uchar > (ysize - 1, x) = 0;
        }
      }
    }

//    printf("ThinImage: pass %d, %d pixels deleted\n", pc, count);
  }

  free(qb);
}
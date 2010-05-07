/* 
 * File:   rothwell.h
 * Author: thuan
 *
 * Created on May 7, 2010, 4:46 PM
 */

#ifndef _ROTHWELL_H
#define	_ROTHWELL_H

#define GAUSS_TAIL 0.015       /* As recommended in the technical report. */
#define FAR 65535
#define DUMMYTHETA 10000.0
#include <opencv/cv.h>
typedef struct{
  int x;
  int y;
  float thin;
}XYFLOAT;

int read_pgm_image(char *infilename, unsigned char **image, int *rows,
   int *cols);
int write_pgm_image(char *outfilename, unsigned char *image, int rows,
    int cols, char *comment, int maxval);

void Sub_pixel_interpolation(float **_grad, float **_dx, float **_dy, int cols,
   int rows, int kwidth, float _low, float ALPHA, float ***_thresh, int ***_dist,
   float ***_theta);
void Thicken_threshold(float **_thresh, int **_dist, int x, int y, float _low,
   int kwidth);
void Compute_gradient(float **dx, float **dy, int cols, int rows, int kwidth,
   float ***grad);
void Compute_x_gradient(float **smoothedimage, int cols, int rows, int kwidth,
   float ***dx);
void Compute_y_gradient(float **smoothedimage, int cols, int rows, int kwidth,
   float ***dy);
void Smooth_image(float **image, int cols, int rows,
   float ***smoothedimage, float sigma, int *kwidth, float gauss_tail);
void Set_kernel(float **kernel, float sigma, int width, int k_size);
int **Make_int_image(int x, int y);
void Set_int_image(int **image, int val, int cols, int rows);
void Copy_int_image(int **image1, int **image2, int cols, int rows);
void Free_int_image(int ***ptr);
float **Make_float_image(int x, int y);
void Set_float_image(float **image, float val, int cols, int rows);
void Copy_float_image(float **image1, float **image2, int cols, int rows);
void Free_float_image(float ***ptr);
void Set_thresholds(int **_dist, float **_grad, float **_thresh, float ***_thin,
   int cols, int rows, float _low);
void Forward_chamfer(int m, int n, int **dist, float **param);
void Backward_chamfer(int m, int n, int **dist, float **param);
void Alt1_chamfer(int m, int n, int **dist, float **param);
void Alt2_chamfer(int m, int n, int **dist, float **param);
int Minimum4(int a, int b, int c, int d);
int Minimum5(int a, int b, int c, int d, int e);
int compare (const void * a, const void * b);
void Thin_edges(float **_thin, float **_thresh, int cols, int rows, int kwidth);
void rothwell(IplImage* imageSource, IplImage* dest, float sigma, float low, float alpha);

#endif	/* _ROTHWELL_H */


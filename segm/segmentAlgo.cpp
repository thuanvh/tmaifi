#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include "segmentAlgo.h"
#include "imageSegment.h"

string cvScalar2String(CvScalar value) {
  string a;
  a += (int) value.val[0];
  a += "-";
  a += (int) value.val[1];
  a += "-";
  a += (int) value.val[2];
  return a;
}

void pyrsegmentation(IplImage* src, IplImage* dst) {
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* comp = NULL;
  cvPyrSegmentation(src, dst, storage, &comp, 4, 200, 50);
  int n_comp = comp->total;
  for (int i = 0; i < n_comp; i++) {
    //CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem( comp, i );
    printf("comp %d\n", i);
    CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem(comp, i);
    CvPoint pt1, pt2;
    int scale = 1;
    CvRect r = cc->rect;
    pt1.x = r.x*scale;
    pt2.x = (r.x + r.width) * scale;
    pt1.y = r.y*scale;
    pt2.y = (r.y + r.height) * scale;
    cvRectangle(dst, pt1, pt2, CV_RGB(255, 0, 0), 3, 8, 0);

    //do_something_with( cc );
  }
  cvReleaseMemStorage(&storage);
}

void pyrmeansegmentation(IplImage* src, IplImage* dst) {
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* comp = NULL;
  cvPyrMeanShiftFiltering(src, dst, 20, 40, 2);
  int n_comp = comp->total;
  for (int i = 0; i < n_comp; i++) {
    //CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem( comp, i );
    printf("comp %d\n", i);
    CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem(comp, i);
    CvPoint pt1, pt2;
    int scale = 1;
    CvRect r = cc->rect;
    pt1.x = r.x*scale;
    pt2.x = (r.x + r.width) * scale;
    pt1.y = r.y*scale;
    pt2.y = (r.y + r.height) * scale;
    cvRectangle(dst, pt1, pt2, CV_RGB(255, 0, 0), 3, 8, 0);

    //do_something_with( cc );
  }
  cvReleaseMemStorage(&storage);
}

void kmeans2(int argc, char** argv) {
#define MAX_CLUSTERS 5
  static const CvScalar color_tab[MAX_CLUSTERS] = {
    CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(100, 100, 255),
    CV_RGB(255, 0, 255), CV_RGB(255, 255, 0)
  };
  IplImage* img = cvCreateImage(cvSize(500, 500), 8, 3);
  CvRandState rng;
  cvRandInit(&rng, 0, 1, -1, CV_RAND_NORMAL);

  cvNamedWindow("clusters", 1);

  for (;;) {
    int k, cluster_count = cvRandNext(&rng) % MAX_CLUSTERS + 1;
    int i, sample_count = cvRandNext(&rng) % 1000 + 1;
    CvMat* points = cvCreateMat(sample_count, 1, CV_32FC2);
    CvMat* clusters = cvCreateMat(sample_count, 1, CV_32SC1);

    /* generate random sample from multigaussian distribution */
    for (k = 0; k < cluster_count; k++) {
      CvPoint center;
      CvMat point_chunk;
      center.x = cvRandNext(&rng) % img->width;
      center.y = cvRandNext(&rng) % img->height;
      cvRandSetRange(&rng, center.x, img->width / 6, 0);
      cvRandSetRange(&rng, center.y, img->height / 6, 1);
      cvGetRows(points, &point_chunk, k * sample_count / cluster_count,
              k == cluster_count - 1 ? sample_count : (k + 1) * sample_count / cluster_count);

      cvRand(&rng, &point_chunk);
    }

    /* shuffle samples */
    for (i = 0; i < sample_count / 2; i++) {
      CvPoint2D32f* pt1 = (CvPoint2D32f*) points->data.fl + cvRandNext(&rng) % sample_count;
      CvPoint2D32f* pt2 = (CvPoint2D32f*) points->data.fl + cvRandNext(&rng) % sample_count;
      CvPoint2D32f temp;
      CV_SWAP(*pt1, *pt2, temp);
    }

    cvKMeans2(points, cluster_count, clusters,
            cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));

    cvZero(img);

    for (i = 0; i < sample_count; i++) {
      CvPoint2D32f pt = ((CvPoint2D32f*) points->data.fl)[i];
      int cluster_idx = clusters->data.i[i];
      cvCircle(img, cvPointFrom32f(pt), 2, color_tab[cluster_idx], CV_FILLED);
    }

    cvReleaseMat(&points);
    cvReleaseMat(&clusters);

    cvShowImage("clusters", img);

    int key = cvWaitKey(0);
    if (key == 27) // 'ESC'
      break;
  }
}

void meanshift(IplImage* image) {
  //  CvHistogram* a;
  //  int hist_size[] = {20, 20, 20};
  //  float r_ranges[] = {0, 255}; /* hue varies from 0 (~0°red) to 180 (~360°red again) */
  //  float g_ranges[] = {0, 255}; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
  //  float b_ranges[] = {0, 255}; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
  //  float* ranges[] = {r_ranges, g_ranges, b_ranges};
  //
  //  a = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);

  //IplImage *image = cvLoadImage("winter.jpg", 1);
  IplImage *image_hsv = cvCreateImage(cvGetSize(image), image->depth, 3);
  //Replacing the origin if necessary
  int flip = 0;
  if (image->origin != IPL_ORIGIN_TL) {
    flip = CV_CVTIMG_FLIP;
  }
  //creating the image in levels of gray
  cvConvertImage(image, image_hsv, CV_BGR2HSV);

  //creating the windows in wich the images will appear
  //  cvNamedWindow("Original image", CV_WINDOW_AUTOSIZE);
  //  cvNamedWindow("HSV image", CV_WINDOW_AUTOSIZE);

  CvHistogram *hist;
  int h_bins = 30, s_bins = 32, v_bins = 30;
  int hist_size[] = {h_bins, s_bins, v_bins};
  float h_ranges[] = {0, 180};
  float s_ranges[] = {0, 255};
  float v_ranges[] = {0, 255};
  float* ranges[] = {h_ranges, s_ranges, v_ranges};
  IplImage* h_plane = cvCreateImage(cvGetSize(image), 8, 1);
  IplImage* s_plane = cvCreateImage(cvGetSize(image), 8, 1);
  IplImage* v_plane = cvCreateImage(cvGetSize(image), 8, 1);
  //IplImage * planes[] = {h_plane, s_plane, v_plane};
  IplImage * planes[] = {s_plane};
  hist = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
  cvCvtPixToPlane(image_hsv, h_plane, s_plane, v_plane, 0);
  cvCalcHist(planes, hist);

  //Show the images
  //  cvShowImage("Original image", image);
  //  cvShowImage("HSV image", image_hsv);

  //cvCalcHist(&image, hist);
  IplImage* imageprob = cvCloneImage(image);
  IplImage* project = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
  cvCalcBackProject(planes, project, hist);
  int x = 0;
  int y = 0;
  int width = image->width;
  int height = image->height;
  int windowsize = 41;
  CvConnectedComp comp;
  CvRect window = cvRect(x, y, windowsize, windowsize);
  CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1000, 0.1);

  cvMeanShift(project, window, criteria, &comp);

  SegImage segimage;
  segimage.width = image->width;
  segimage.height = image->height;
  segimage.image = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
  int segcount = 0;
  CvSeq* contours = comp.contour;



  //cvDrawContours(segimage.image, contours, cvScalarAll(segcount + 1), cvScalarAll(255), -1, -1, 8, cvPoint(0, 0));
  for (; contours != 0; contours = contours->h_next, segcount++)
  {
    cout<<"contour"<<segcount<<endl;
    cvDrawContours(segimage.image, contours, cvScalarAll(segcount + 1),
            cvScalarAll(255), -1, -1, 8, cvPoint(0, 0));
  }

  segimage.segments = segcount;
  cout << "Segment:" << segcount << endl;
  IplImage* imageshow = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
  segimage.colorerSegImage(image, imageshow);
  cvShowImage("image segment", imageshow);
  cvShowImage("image source", image);
  //cvShowImage("image source",image);

  cvWaitKey(0);
  cvReleaseImage(&imageshow);
  cvReleaseImage(&imageprob);
  cvReleaseImage(&project);

}

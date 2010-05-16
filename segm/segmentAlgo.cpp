


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>

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



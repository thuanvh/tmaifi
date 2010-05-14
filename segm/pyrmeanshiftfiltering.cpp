#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>

void pyrmeansegmentation(IplImage* src, IplImage* dst) {
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* comp = NULL;
  cvPyrMeanShiftFiltering(src, dst, 20,40,2);
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



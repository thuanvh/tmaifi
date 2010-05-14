


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>


IplImage* marker_mask = 0;
IplImage* markers = 0;
IplImage* img0 = 0, *img = 0, *img_gray = 0, *wshed = 0;
CvPoint prev_pt = {-1, -1};

void on_mouse(int event, int x, int y, int flags, void* param) {
  if (!img)
    return;

  if (event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON))
    prev_pt = cvPoint(-1, -1);
  else if (event == CV_EVENT_LBUTTONDOWN)
    prev_pt = cvPoint(x, y);
  else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)) {
    CvPoint pt = cvPoint(x, y);
    if (prev_pt.x < 0)
      prev_pt = pt;
    cvLine(marker_mask, prev_pt, pt, cvScalarAll(255), 5, 8, 0);
    cvLine(img, prev_pt, pt, cvScalarAll(255), 5, 8, 0);
    prev_pt = pt;
    cvShowImage("image", img);
  }
}

void watershed(char* filename) {
  //char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
  CvRNG rng = cvRNG(-1);

  if ((img0 = cvLoadImage(filename, 1)) == 0)
    return 0;

  printf("Hot keys: \n"
          "\tESC - quit the program\n"
          "\tr - restore the original image\n"
          "\tw or SPACE - run watershed algorithm\n"
          "\t\t(before running it, roughly mark the areas on the image)\n"
          "\t  (before that, roughly outline several markers on the image)\n");

  cvNamedWindow("image", 1);
  cvNamedWindow("watershed transform", 1);

  img = cvCloneImage(img0);
  img_gray = cvCloneImage(img0);
  wshed = cvCloneImage(img0);
  marker_mask = cvCreateImage(cvGetSize(img), 8, 1);
  markers = cvCreateImage(cvGetSize(img), IPL_DEPTH_32S, 1);
  cvCvtColor(img, marker_mask, CV_BGR2GRAY);
  cvCvtColor(marker_mask, img_gray, CV_GRAY2BGR);

  cvZero(marker_mask);
  cvZero(wshed);
  cvShowImage("image", img);
  cvShowImage("watershed transform", wshed);
  cvSetMouseCallback("image", on_mouse, 0);

  for (;;) {
    int c = cvWaitKey(0);

    if ((char) c == 27)
      break;

    if ((char) c == 'r') {
      cvZero(marker_mask);
      cvCopy(img0, img);
      cvShowImage("image", img);
    }

    if ((char) c == 'w' || (char) c == ' ') {
      CvMemStorage* storage = cvCreateMemStorage(0);
      CvSeq* contours = 0;
      CvMat* color_tab;
      int i, j, comp_count = 0;
      //cvSaveImage( "wshed_mask.png", marker_mask );
      //marker_mask = cvLoadImage( "wshed_mask.png", 0 );
      cvFindContours(marker_mask, storage, &contours, sizeof (CvContour),
              CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
      cvZero(markers);
      for (; contours != 0; contours = contours->h_next, comp_count++) {
        cvDrawContours(markers, contours, cvScalarAll(comp_count + 1),
                cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0, 0));
      }

      color_tab = cvCreateMat(1, comp_count, CV_8UC3);
      for (i = 0; i < comp_count; i++) {
        uchar* ptr = color_tab->data.ptr + i * 3;
        ptr[0] = (uchar) (cvRandInt(&rng) % 180 + 50);
        ptr[1] = (uchar) (cvRandInt(&rng) % 180 + 50);
        ptr[2] = (uchar) (cvRandInt(&rng) % 180 + 50);
      }

      {
        double t = (double) cvGetTickCount();
        cvWatershed(img0, markers);
        t = (double) cvGetTickCount() - t;
        printf("exec time = %gms\n", t / (cvGetTickFrequency()*1000.));
      }

      // paint the watershed image
      for (i = 0; i < markers->height; i++)
        for (j = 0; j < markers->width; j++) {
          int idx = CV_IMAGE_ELEM(markers, int, i, j);
          uchar* dst = &CV_IMAGE_ELEM(wshed, uchar, i, j * 3);
          if (idx == -1)
            dst[0] = dst[1] = dst[2] = (uchar) 255;
          else if (idx <= 0 || idx > comp_count)
            dst[0] = dst[1] = dst[2] = (uchar) 0; // should not get here
          else {
            uchar* ptr = color_tab->data.ptr + (idx - 1)*3;
            dst[0] = ptr[0];
            dst[1] = ptr[1];
            dst[2] = ptr[2];
          }
        }

      cvAddWeighted(wshed, 0.5, img_gray, 0.5, 0, wshed);
      cvShowImage("watershed transform", wshed);
      cvReleaseMemStorage(&storage);
      cvReleaseMat(&color_tab);
    }
  }

  return 1;
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

void kmeans2( int argc, char** argv )
{
    #define MAX_CLUSTERS 5
    static const int color_tab[MAX_CLUSTERS] =
    {
        CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(100,100,255),
        CV_RGB(255,0,255), CV_RGB(255,255,0)
    };
    IplImage* img = cvCreateImage( cvSize( 500, 500 ), 8, 3 );
    CvRandState rng;
    cvRandInit( &rng, 0, 1, -1, CV_RAND_NORMAL );

    cvNamedWindow( "clusters", 1 );

    for(;;)
    {
        int k, cluster_count = cvRandNext(&rng)%MAX_CLUSTERS + 1;
        int i, sample_count = cvRandNext(&rng)%1000 + 1;
        CvMat* points = cvCreateMat( sample_count, 1, CV_32FC2 );
        CvMat* clusters = cvCreateMat( sample_count, 1, CV_32SC1 );

        /* generate random sample from multigaussian distribution */
        for( k = 0; k < cluster_count; k++ )
        {
            CvPoint center;
            CvMat point_chunk;
            center.x = cvRandNext(&rng)%img->width;
            center.y = cvRandNext(&rng)%img->height;
            cvRandSetRange( &rng, center.x, img->width/6, 0 );
            cvRandSetRange( &rng, center.y, img->height/6, 1 );
            cvGetRows( points, &point_chunk, k*sample_count/cluster_count,
                       k == cluster_count - 1 ? sample_count : (k+1)*sample_count/cluster_count );

            cvRand( &rng, &point_chunk );
        }

        /* shuffle samples */
        for( i = 0; i < sample_count/2; i++ )
        {
            CvPoint2D32f* pt1 = (CvPoint2D32f*)points->data.fl + cvRandNext(&rng)%sample_count;
            CvPoint2D32f* pt2 = (CvPoint2D32f*)points->data.fl + cvRandNext(&rng)%sample_count;
            CvPoint2D32f temp;
            CV_SWAP( *pt1, *pt2, temp );
        }

        cvKMeans2( points, cluster_count, clusters,
                   cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ));

        cvZero( img );

        for( i = 0; i < sample_count; i++ )
        {
            CvPoint2D32f pt = ((CvPoint2D32f*)points->data.fl)[i];
            int cluster_idx = clusters->data.i[i];
            cvCircle( img, cvPointFrom32f(pt), 2, color_tab[cluster_idx], CV_FILLED );
        }

        cvReleaseMat( &points );
        cvReleaseMat( &clusters );

        cvShowImage( "clusters", img );

        int key = cvWaitKey(0);
        if( key == 27 ) // 'ESC'
            break;
    }
}


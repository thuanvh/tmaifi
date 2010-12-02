#define OPENCV

#include <libplayerc++/playerc++.h>
#include <stdio.h>
#include <math.h>
#include <stack>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <time.h>

#include <fstream>
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "exploration.h"
using namespace cv;

using namespace PlayerCc;
using namespace std;
#define RAYS 32

//typedef struct {
//  int dist;
//  int degree;
//} laser_range;

double scale = 0.05;
//int taille_laser = 81;
//laser_range info[81][81];


stack<player_pose2d_t> track;

void getMapLocal(int**& localmap, Position2dProxy& pos, LaserProxy& laser, int& localsize) {
  double minAngle = laser.GetMinAngle();
  double maxAngle = laser.GetMaxAngle();
  int maxRange = round(laser.GetMaxRange() / scale);
  localsize = maxRange * 2;

  localmap = new int*[localsize];
  for (int i = 0; i < localsize; i++) { // row
    localmap[i] = new int[localsize];
    for (int j = 0; j < localsize; j++) { // col
      localmap[i][j] = UNKNOWN;
    }
  }
  //  for (int i = 0; i < 360; i++) {
  //    cout << i << "->" << laser.GetRange(i) << " ";
  //  }
  for (int i = 0; i < localsize; i++) { // row
    for (int j = 0; j < localsize; j++) { // col
      int x = j - localsize / 2;
      int y = localsize / 2 - i;
      double length = sqrt(x * x + y * y);
      double sinAngle = 0;
      if (length > 0)
        sinAngle = sin(abs(y) / length);
      int intAngle = round(asin(sinAngle)*180 / M_PI);
      if (abs(x) == abs(y) && abs(x) == localsize / 2) {
        int a = 0;
      }
      if (x >= 0 && y > 0) {
        intAngle = intAngle;
      } else if (x < 0 && y >= 0) {
        intAngle = 180 - intAngle;
      } else if (x <= 0 && y < 0) {
        intAngle = 180 + intAngle;
      } else if (x > 0 && y <= 0) {
        intAngle = 360 - intAngle;
      }
      intAngle = (180 + intAngle) % 360;
      double range = 0;
      int tempAngle = intAngle;
      while ((range = laser.GetRange(tempAngle)) <= 0) {
        tempAngle++;
      }
      range = range / scale;
      int intrange = round(range);
      int intlength = round(length);
      if (intlength > maxRange) {
        localmap[i][j] = UNKNOWN;
      } else {
        if (intrange < maxRange) {
          if (intlength < intrange)
            localmap[i][j] = FREE;
          else if (intlength == intrange)
            localmap[i][j] = OBSTACLE;
          else
            localmap[i][j] = UNKNOWN;
        } else {
          if (intlength <= maxRange)
            localmap[i][j] = FREE;
        }
      }
    }
  }
}

void integrateToMapGlobal(int**& map, int** maplocal, Position2dProxy& pos, int mapwidth, int mapheight, int maplocalsize) {
  double yaw = pos.GetYaw();
  double posx = pos.GetXPos() / scale;
  double posy = pos.GetYPos() / scale;
  double cosAngle = cos(yaw);
  double sinAngle = sin(yaw);
  for (int i = 0; i < maplocalsize; i++) {
    for (int j = 0; j < maplocalsize; j++) {
      int x = j - maplocalsize / 2;
      int y = maplocalsize / 2 - i;
      int newx = posx + x * cosAngle - y*sinAngle;
      int newy = posy + x * sinAngle + y*cosAngle;
      int newj = mapwidth / 2 + newx;
      int newi = mapheight / 2 - newy;
      if (map[newi][newj] == UNKNOWN) {
        map[newi][newj] = maplocal[i][j];
      }
    }
  }

}

//void findBestNextPoint(Mat* map, Mat* img,) {
//
//}

void AStarMapFinder() {

}
//fonction principale

int main(int argc, char **argv) {

  bool comeback;
  // we throw exceptions on creation if we fail
  try {
    PlayerClient robot("localhost", 6665);
    Position2dProxy pos(&robot, 1);
    LaserProxy laser(&robot, 0);

    std::cout << robot << std::endl;
    pos.SetMotorEnable(true);
    robot.Read();
    robot.Read();

    // go into read-think-act loop
    int temps = 0;
    player_pose2d_t target_pose;
    comeback = true;
    bool stop = false;
    cvNamedWindow("carte", 1);
    Mat mapimage(640, 640, CV_8UC1);
    int** map;
    int height = 640;
    int width = 640;
    map = new int*[height];
    for (int i = 0; i < height; i++) {
      map[i] = new int[width];
      for (int j = 0; j < width; j++) {
        map[i][j] = UNKNOWN;
      }
      //cout<<endl;
    }
    while (!stop) {

      robot.Read();

      // get localmap
      int** localmap;
      int localsize;
      getMapLocal(localmap, pos, laser, localsize);
      Mat localimage(localsize, localsize, CV_8UC1);
      for (int i = 0; i < localsize; i++) {
        for (int j = 0; j < localsize; j++) {
          localimage.at<uchar > (i, j) = localmap[i][j]*255 / 2;
        }
      }
      imshow("localmap", localimage);

      integrateToMapGlobal(map, localmap, pos, width, height, localsize);
      for (int i = 0; i < mapimage.rows; i++) {
        for (int j = 0; j < mapimage.cols; j++) {
          mapimage.at<uchar > (i, j) = map[i][j]*255 / 2;
        }
      }
      imshow("carte", mapimage);
      //      waitKey();
      //getchar();
      if (waitKey(30) >= 0) continue;
    }
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return -1;
  }
}
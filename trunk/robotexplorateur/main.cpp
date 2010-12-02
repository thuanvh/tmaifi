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
      double angle = 0;
      if (length > 0)
        angle = asin(abs(y) / length);
      int intAngle = round(angle*180 / M_PI);
      if (abs(x) == abs(y) && abs(x) == localsize / 2) {
        int a = 0;
      }
      if(x==0 || y==0){
        int a=0;
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

void findBestNextPoint(int** map, Position2dProxy& pos, int& x, int& y, int mapwidth, int mapheight) {
  x = y = -1;
  int posx = mapwidth / 2 + pos.GetXPos() / scale;
  int posy = mapheight / 2 - pos.GetYPos() / scale;
  cout << "robot:" << posx << "*" << posy << endl;
#define MAXDISTANCE 1e20
  double mindistance = 1e20;
  for (int i = 0; i < mapheight; i++) {
    for (int j = 0; j < mapwidth; j++) {
      if (map[i][j] == FREE
        && (map[i - 1][j] == UNKNOWN || map[i][j + 1] == UNKNOWN || map[i + 1][j] == UNKNOWN || map[i][j - 1] == UNKNOWN)
        && (map[i - 1][j] != OBSTACLE && map[i][j + 1] != OBSTACLE && map[i + 1][j] != OBSTACLE && map[i][j - 1] != OBSTACLE)
        ) {
        //        cout<<"existe"<<endl;
        double distance = sqrt(pow(posx - j, 2) + pow(posy - i, 2));
        if (mindistance > distance) {
          mindistance = distance;
          x = j;
          y = i;
        }
      }
    }
  }
}
#define CHILD_NUMBER 4

struct node {
  node();

  node(int _x, int _y, node * _parent) {
    x = _x;
    y = _y;
    parent = _parent;
    if (parent != NULL)
      rootdistance = _parent->rootdistance + 1;
    for (int i = 0; i < CHILD_NUMBER; i++) {
      childs[i] = NULL;
    }
  }
  int x;
  int y;
  int rootdistance;
  double estimatedistane;
  node * childs[CHILD_NUMBER];
  node* parent;

  void estimateDistance(node * dist) {
    double d = sqrt(pow(dist->x - x, 2) + pow(dist->y - y, 2));
    estimatedistane = d + rootdistance;
  }

  ~node() {
    for (int i = 0; i < CHILD_NUMBER; i++) {
      if (childs[i] != NULL) {
        delete childs[i];
      }
    }
  }
};

bool isEqual(node* a, node* b) {
  return (a->x == b->x && a->y == b->y);
}

bool isInList(node* a, vector<node*>& visitedList) {
  for (int i = 0; i < visitedList.size(); i++) {
    node* b = visitedList[i];
    if (isEqual(a, b)) {
      return true;
    }
  }
  return false;
}

void insertIntoList(node* a, vector<node*>& nodeList) {
  vector<node*>::iterator iter;
  for (iter = nodeList.begin(); iter < nodeList.end(); iter++) {
    node* item = *iter;
    if (a->estimatedistane < item->estimatedistane) {
      nodeList.insert(iter, a);
      return;
    }
  }
  nodeList.push_back(a);
}

void AStarMapFinder(int** map, int startX, int startY, int destX, int destY, int mapwidth, int mapheight,
  int*& pathX, int*& pathY, int& length) {
  node* root = new node(startX, startY, NULL);
  root->rootdistance = 0;
  node* dest = new node(destX, destY, NULL);
  vector<node*> list;
  vector<node*> visitedList;
  list.push_back(root);
  bool found = false;
  node* current = NULL;
  while (!found && list.size() > 0) {
    cout << list.size() << " ";
    current = list[0];
    list.erase(list.begin());
    if (isInList(current, visitedList)) {
      continue;
    } else {
      visitedList.push_back(current);
    }
    if (isEqual(current, dest)) {
      found = true;
      break;
    }
    current->childs[0] = current->x < 1 ? NULL : new node(current->x - 1, current->y, current);
    current->childs[1] = current->x > mapwidth - 1 ? NULL : new node(current->x + 1, current->y, current);
    current->childs[2] = current->y < 1 ? NULL : new node(current->x, current->y - 1, current);
    current->childs[3] = current->y > mapheight - 1 ? NULL : new node(current->x, current->y + 1, current);
    for (int i = 0; i < CHILD_NUMBER; i++) {
      node* child = current->childs[i];
      if (child != NULL) {
        if (map[child->y][child->x] == FREE && !isInList(child, visitedList)) {
          child->estimateDistance(dest);
          insertIntoList(child, list);
        } else {
          current->childs[i] = NULL;
          delete child;
        }
      }
    }
  }
  if (found) {
    cout << "found" << endl;
    length = current->rootdistance;
    pathX = new int[length];
    pathY = new int[length];
    node* temp = current;
    int tempindex = length;
    while (temp->parent != NULL) {
      pathX[tempindex - 1] = temp->parent->x;
      pathY[tempindex - 1] = temp->parent->y;
      temp = temp->parent;
      tempindex--;
    }
    cout << "path:";
    for (int i = 0; i < length; i++) {
      cout << pathX[i] << "*" << pathY[i] << " ";
    }
    cout << endl;
  } else {
    length = -1;
  }
  delete root;
  delete dest;
}
//fonction principale

int main1(int argc, char** argv) {
  int** A;
  A = new int*[30];
  for (int i = 0; i < 30; i++) {
    A[i] = new int[7];
    for (int j = 0; j < 7; j++) {
      A[i][j] = FREE;
    }
  }
  A[3][4] = OBSTACLE;
  A[4][4] = OBSTACLE;
  int* pathX;
  int* pathY;
  int length;
  AStarMapFinder(A, 3, 3, 5, 5, 7, 30, pathX, pathY, length);
}

int getAngle(double x, double y) {
  double length = sqrt(x * x + y * y);
  double sinAngle = 0;
  if (length > 0)
    sinAngle = sin(abs(y) / length);
  int intAngle = round(asin(sinAngle)*180 / M_PI);

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
  return intAngle;
}

void MoveToTarget(double targetX, double targetY, Position2dProxy& pos, PlayerClient& robot) {
  robot.Read();
  robot.Read();
  double startX = pos.GetXPos();
  double startY = pos.GetYPos();
  double yaw = pos.GetYaw();
  int angleYaw = rtod(yaw);
  int angleRotate = getAngle(targetX - startX, targetY - startY);
  cout << angleYaw << " " << angleRotate << endl;
}

void MoveToPath(int* pathX, int *pathY, int length, Position2dProxy& pos, PlayerClient& robot, int width, int height) {
  pos.SetMotorEnable(true);
  for (int i = 0; i < length; i++) {
    double targetX = (pathX[i] - width / 2) * scale;
    double targetY = (height / 2 - pathY[i]) * scale;
    MoveToTarget(targetX, targetY, pos, robot);

  }
}

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
      int destX;
      int destY;
      findBestNextPoint(map, pos, destX, destY, width, height);
      cout << "target " << destX << "*" << destY << endl;
      int* pathX;
      int* pathY;
      int length;
      int startX = width / 2 + pos.GetXPos() / scale;
      int startY = height / 2 - pos.GetYPos() / scale;
      AStarMapFinder(map, startX, startY, destX, destY, width, height, pathX, pathY, length);

      MoveToTarget(1,1,pos,robot);
      waitKey();

      //getchar();
      if (waitKey(30) >= 0) continue;
    }
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return -1;
  }
}

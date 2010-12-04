/* 
 * File:   exploration.h
 * Author: thuanvh
 *
 * Created on November 25, 2010, 12:00 AM
 */

#ifndef EXPLORATION_H
#define	EXPLORATION_H

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

#define UNKNOWN 1
#define OBSTACLE 0
#define FREE 2

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


int getAngle(double x, double y);
bool isFreePath(node* a, node* b, int** map);
void MoveToTarget(double targetX, double targetY, Position2dProxy& pos, PlayerClient& robot);
void MoveToTarget(int x, int y, Position2dProxy& pos, PlayerClient& robot, int width, int height) ;
#endif	/* EXPLORATION_H */


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
            //      double angle = 0;
            //      if (length > 0)
            //        angle = asin(abs(y) / length);
            //      int intAngle = round(angle * 180 / M_PI);
            ////      if (abs(x) == abs(y) && abs(x) == localsize / 2) {
            ////        int a = 0;
            ////      }
            //      if (x == 0 || y == 0) {
            //        int a = 0;
            //      }
            //      if (x >= 0 && y > 0) {
            //        intAngle = intAngle;
            //      } else if (x < 0 && y >= 0) {
            //        intAngle = 180 - intAngle;
            //      } else if (x <= 0 && y < 0) {
            //        intAngle = 180 + intAngle;
            //      } else if (x > 0 && y <= 0) {
            //        intAngle = 360 - intAngle;
            //      }
            //      intAngle = (180 + intAngle) % 360;
            int intAngle = getAngle(x, y);
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
            } else if (map[newi][newj] == FREE && maplocal[i][j] == OBSTACLE) {
                map[newi][newj] = maplocal[i][j];
            } else if (map[newi][newj] == OBSTACLE && maplocal[i][j] == FREE) {
                //        map[newi][newj] = maplocal[i][j];
            }

        }
    }

}

bool isNearObstacle(int x, int y, int** map, int width, int height) {
    int obstaclerange = 5;
    bool nearobstacle = false;
    for (int k = y - obstaclerange; k < y + obstaclerange; k++) {
        for (int l = x - obstaclerange; l < x + obstaclerange; l++) {
            if (k < 0 || k >= height || l < 0 || l >= width)
                continue;
            if (map[k][l] == OBSTACLE) {
                nearobstacle = true;
                break;
            }
        }
        if (nearobstacle) break;
    }
    return nearobstacle;
}

void findBestNextPoint(int** map, Position2dProxy& pos, int& x, int& y, int mapwidth, int mapheight, int range) {
    x = y = -1;
    int posx = mapwidth / 2 + pos.GetXPos() / scale;
    int posy = mapheight / 2 - pos.GetYPos() / scale;

    cout << "robot:" << posx << "*" << posy << endl;
#define MAXDISTANCE 1e20
    double mindistance = 1e20;
    node src(posx,posy,NULL);
    
    for (int i = 0; i < mapheight; i++) {
        for (int j = 0; j < mapwidth; j++) {
            if (map[i][j] == FREE
                    && (map[i - 1][j] == UNKNOWN || map[i][j + 1] == UNKNOWN || map[i + 1][j] == UNKNOWN || map[i][j - 1] == UNKNOWN)
                    //&& (map[i - 1][j] != OBSTACLE && map[i][j + 1] != OBSTACLE && map[i + 1][j] != OBSTACLE && map[i][j - 1] != OBSTACLE)
               ) {
                bool nearobstacle = isNearObstacle(j, i, map, mapwidth, mapheight);
                if (nearobstacle) continue;
                //        cout<<"existe"<<endl;
		
                int distance = sqrt(pow(posx - j, 2) + pow(posy - i, 2));
		node dist(j,i,NULL);
		bool isFree=isFreePath(&dist,&src,map);
		if(!isFree)
		  distance+=range;
                if (mindistance > distance && distance >= range - 1) {
                    mindistance = distance;
                    x = j;
                    y = i;
                }
            }
        }
    }
}
bool isDest(node* a, int** map, int mapwidth,int mapheight){
  int i=a->y;
  int j=a->x;
  if(map[i][j] == FREE  && (map[i - 1][j] == UNKNOWN || map[i][j + 1] == UNKNOWN || map[i + 1][j] == UNKNOWN || map[i][j - 1] == UNKNOWN))
  {
    bool nearobstacle = isNearObstacle(j, i, map, mapwidth, mapheight);  
    if(nearobstacle)
      return false;
    return true;
  }
  return false;
}
bool isContain(node* a, node* b, int r, int** map) {
    if (a->x - r <= b->x && a->x + r >= b->x && a->y - r <= b->y && a->y + r >= b->y) {
        if (isFreePath(a, b, map))
            return true;
    }
    return false;
}

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

bool isFreePath(node* a, node* b, int** map) {
    int minX = (a->x < b->x) ? a->x : b->x;
    int maxX = (a->x > b->x) ? a->x : b->x;
    int minY = (a->y < b->y) ? a->y : b->y;
    int maxY = (a->y > b->y) ? a->y : b->y;

    for (int i = minY; i < maxY; i++) {
        for (int j = minX; j < maxX; j++) {
            if (map[i][j] != FREE) {
                return false;
            }
        }
    }
    return true;
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
    const int deltaCell = 5;
    while (!found && list.size() > 0) {
        //    cout << list.size() << " ";
        current = list[0];
        list.erase(list.begin());
        if (isInList(current, visitedList)) {
            continue;
        } else {
            visitedList.push_back(current);
        }
        //if (isEqual(current, dest)) {
        if (isContain(current, dest, deltaCell, map)) {
	//if(isDest(current,map,mapwidth,mapheight)){
            found = true;
            break;
        }
        current->childs[0] = current->x < deltaCell ? NULL : new node(current->x - deltaCell, current->y, current);
        current->childs[1] = current->x >= mapwidth - deltaCell ? NULL : new node(current->x + deltaCell, current->y, current);
        current->childs[2] = current->y < deltaCell ? NULL : new node(current->x, current->y - deltaCell, current);
        current->childs[3] = current->y >= mapheight - deltaCell ? NULL : new node(current->x, current->y + deltaCell, current);
	
	current->childs[4] = current->childs[1]==NULL || current->childs[2]==NULL ? NULL : new node(current->childs[1]->x, current->childs[2]->y, current);
	current->childs[5] = current->childs[1]==NULL || current->childs[3]==NULL ? NULL : new node(current->childs[1]->x, current->childs[3]->y, current);
	current->childs[6] = current->childs[0]==NULL || current->childs[3]==NULL ? NULL : new node(current->childs[0]->x, current->childs[3]->y, current);
	current->childs[7] = current->childs[0]==NULL || current->childs[2]==NULL ? NULL : new node(current->childs[0]->x, current->childs[2]->y, current);
        for (int i = 0; i < CHILD_NUMBER; i++) {
            node* child = current->childs[i];
            if (child != NULL) {
                if (map[child->y][child->x] == FREE && isFreePath(current, child, map)
                        && !isNearObstacle(child->x, child->y, map, mapwidth, mapheight) && !isInList(child, visitedList)) {
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
	if(current->parent!=NULL)
	  current=current->parent;
        cout << "found" << endl;
        length = current->rootdistance;
        pathX = new int[length];
        pathY = new int[length];
        node* temp = current;
        int tempindex = length;
        while (temp->parent != NULL) {
            pathX[tempindex - 1] = temp->x;
            pathY[tempindex - 1] = temp->y;
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
        A[i] = new int[10];
        for (int j = 0; j < 10; j++) {
            A[i][j] = FREE;
        }
    }
    A[3][4] = OBSTACLE;
    A[4][4] = OBSTACLE;
    int* pathX;
    int* pathY;
    int length;
    AStarMapFinder(A, 3, 3, 5, 5, 10, 30, pathX, pathY, length);
}

int getAngle(double x, double y) {
    double length = sqrt(x * x + y * y);
    double angle = 0;
    if (length > 0)
        angle = asin(abs(y) / length);
    int intAngle = round(angle * 180 / M_PI);

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

void MoveToTarget(int x, int y, Position2dProxy& pos, PlayerClient& robot, int width, int height) {
    double targetX = (x - width / 2) * scale;
    double targetY = (height / 2 - y) * scale;
    MoveToTarget(targetX, targetY, pos, robot);
}

void MoveToTarget(double targetX, double targetY, Position2dProxy& pos, PlayerClient& robot) {
    robot.Read();
    robot.Read();
    double startX = pos.GetXPos();
    double startY = pos.GetYPos();
    double yaw = pos.GetYaw();
    int angleYaw = rtod(yaw);
    int angleRotate = getAngle(targetX - startX, targetY - startY);
    //  cout << angleYaw << " " << angleRotate << endl;
    int deltaangle=(angleRotate - angleYaw)%360;
    if(deltaangle<0)
      deltaangle=360+deltaangle;
    if(abs(deltaangle)<180)
      pos.SetSpeed(0, dtor(deltaangle));
    else
      pos.SetSpeed(0, -dtor(360-deltaangle));
    sleep(1);
    pos.SetSpeed(0, 0);
    int delta=0;//10 idiot
    double distance = sqrt(pow(targetX - startX, 2) + pow(targetY - startY, 2)) - delta*scale;
    pos.SetSpeed(-distance, -distance, 0);
    //pos.SetSpeed(-abs(targetX - startX), -abs(targetY - startY), 0);
    //pos.SetSpeed((targetX-startX),(targetY-startY),0);
    sleep(1);
    pos.SetSpeed(0, 0);
}

void Wander(Position2dProxy & pos) {
    double oldx = pos.GetXPos();
    double oldy = pos.GetYPos();
    while (true) {
	cout<<"wandering"<<endl;
        pos.GetPlayerClient()->Read();
        int forward = rand() % 2;
        double v = 5 * scale;
        if (forward) {
            pos.SetSpeed(v, v, dtor(rand() % 180));
        } else {
            pos.SetSpeed(-v, -v, dtor(rand() % 180));
        }
        sleep(1);
        pos.SetSpeed(0, 0);
        if (pos.GetXPos() != oldx || pos.GetYPos() != oldy) {
            break;
        }
    }
}

void MoveToPath(int* pathX, int *pathY, int length, Position2dProxy& pos, PlayerClient& robot, int width, int height) {
    pos.SetMotorEnable(true);
    for (int i = 0; i < length; i++) {
        double targetX = (pathX[i] - width / 2) * scale;
        double targetY = (height / 2 - pathY[i]) * scale;
        MoveToTarget(targetX, targetY, pos, robot);
    }
}

void MoveOutObstacle(Position2dProxy& pos, int** map, int mapwidth, int mapheight) {
    pos.GetPlayerClient()->Read();
    int x = mapwidth / 2 + pos.GetXPos() / scale;
    int y = mapheight / 2 - pos.GetYPos() / scale;
    int obstaclerange = 5;
    bool nearobstacle = false;
    int k;
    int l;
    for (k = y - obstaclerange; k < y + obstaclerange; k++) {
        for (l = x - obstaclerange; l < x + obstaclerange; l++) {
            if (k < 0 || k >= mapheight || l < 0 || l >= mapwidth)
                continue;
            if (map[k][l] == OBSTACLE) {
                nearobstacle = true;
                break;
            }
        }
        if (nearobstacle) break;
    }
    if (nearobstacle) {
        int x2[1] = {2 * l - x};
        int y2[1] = {2 * k - y};
        MoveToPath(x2, y2, 1, pos, *pos.GetPlayerClient(), mapwidth, mapheight);
        Wander(pos);
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

        int** map;
//         int height = 600;
//         int width = 1200;
	int height = 640;
	int width = 640;
        Mat mapimage(height, width, CV_8UC1);
	//Mat pathimage(height, width, CV_8UC3);
	
        map = new int*[height];
        for (int i = 0; i < height; i++) {
            map[i] = new int[width];
            for (int j = 0; j < width; j++) {
                map[i][j] = UNKNOWN;
            }
            //cout<<endl;
        }
        vector<int> planPathX;
        vector<int> planPathY;
        while (!stop) {

            int numberRotate = 2;
            if (planPathX.size()==0) {
                while (true) {
                    cout<<"refresh map"<<endl;
                    robot.Read();
                    robot.Read();
                    // get localmap
                    int** localmap;
                    int localsize;
                    getMapLocal(localmap, pos, laser, localsize);
                    //        Mat localimage(localsize, localsize, CV_8UC1);
                    //        for (int i = 0; i < localsize; i++) {
                    //          for (int j = 0; j < localsize; j++) {
                    //            localimage.at<uchar > (i, j) = localmap[i][j]*255 / 2;
                    //          }
                    //        }
                    //        imshow("localmap", localimage);

                    integrateToMapGlobal(map, localmap, pos, width, height, localsize);
                    for (int i = 0; i < mapimage.rows; i++) {
                        for (int j = 0; j < mapimage.cols; j++) {
                            mapimage.at<uchar > (i, j) = map[i][j]*255 / 2;
                        }
                    }
                    imshow("carte", mapimage);
                    if(numberRotate>0){ 
		      pos.SetSpeed(0, 0, dtor(rand() % 360));
		      sleep(1);
		      pos.SetSpeed(0, 0, 0);
		    }
                    if (numberRotate-- <= 0)
                        break;
                    
                }
            }

            int destX;
            int destY;
            int range = laser.GetMaxRange() / scale;
            int* pathX;
            int* pathY;
            int length=-1;
            int startX = width / 2 + pos.GetXPos() / scale;
            int startY = height / 2 - pos.GetYPos() / scale;
            if (planPathX.size()==0) {
                findBestNextPoint(map, pos, destX, destY, width, height, range);
                cout << "target " << destX << "*" << destY << endl;

                AStarMapFinder(map, startX, startY, destX, destY, width, height, pathX, pathY, length);
            }
            //getchar();
            Mat pathimage = mapimage.clone();
            circle(pathimage, Point(startX, startY), 2, Scalar(0));
            circle(pathimage, Point(destX, destY), 2, Scalar(0));
            imshow("image path", pathimage);
            if (waitKey(30) >= 0) continue;
            if (length != -1) {


                //        //        MoveToTarget(1, 1, pos, robot);
                //        planPathX.clear();
                //        planPathY.clear();
                for (int i = 0; i < length; i++) {
                    circle(pathimage, Point(pathX[i], pathY[i]), 2, Scalar(0));

//                     planPathX.insert(planPathX.begin(), pathX[i]);
//                     planPathY.insert(planPathY.begin(), pathY[i]);
                }
                imshow("image path", pathimage);
		if (waitKey(30) >= 0) continue;
                MoveToPath(pathX, pathY, length, pos, robot, width, height);
                //        //getchar();
                //
                //        //      waitKey();
            } 
            else {
                Wander(pos);
            }
//             if (planPathX.size() > 0) {
//                 //MoveToPath(pathX, pathY, length, pos, robot, width, height);
//                 MoveToTarget(planPathX[0], planPathY[0], pos, robot, width, height);
//                 planPathX.erase(planPathX.begin());
//                 planPathY.erase(planPathY.begin());
// 
//             } else {
//                 //Wander(pos);
//                 MoveOutObstacle(pos, map, width, height);
//             }
            
            imshow("image path", pathimage);
            if (waitKey(30) >= 0) continue;
        }
    } catch (PlayerCc::PlayerError e) {
        std::cerr << e << std::endl;
        return -1;
    }
}

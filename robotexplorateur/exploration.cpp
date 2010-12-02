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
using namespace cv;

using namespace PlayerCc;
using namespace std;
#define RAYS 32

typedef struct {
  int dist;
  int degree;
} laser_range;

//double scale = 0.05;
int taille_laser = 81;
laser_range info[81][81];
int carte[640][640];

stack<player_pose2d_t> track;

void print_usage(int argc, char** argv);
std::string gHostname(PlayerCc::PLAYER_HOSTNAME);
uint32_t gPort(PlayerCc::PLAYER_PORTNUM);
uint32_t gIndex(0);
uint32_t gDebug(0);
uint32_t gFrequency(10); // Hz
uint32_t gDataMode(PLAYER_DATAMODE_PUSH);
bool gUseLaser(false);

int parse_args(int argc, char** argv) {
  // set the flags
  const char* optflags = "h:p:i:d:u:lm:";
  int ch;

  // use getopt to parse the flags
  while (-1 != (ch = getopt(argc, argv, optflags))) {
    switch (ch) {
        // case values must match long_options
      case 'h': // hostname
        gHostname = optarg;
        break;
      case 'p': // port
        gPort = atoi(optarg);
        break;
      case 'i': // index
        gIndex = atoi(optarg);
        break;
      case 'd': // debug
        gDebug = atoi(optarg);
        break;
      case 'u': // update rate
        gFrequency = atoi(optarg);
        break;
      case 'm': // datamode
        gDataMode = atoi(optarg);
        break;
      case 'l': // datamode
        gUseLaser = true;
        break;
      case '?': // help
      case ':':
      default: // unknown
        print_usage(argc, argv);
        exit(-1);
    }
  }

  return (0);
} // end parse_args

void print_usage(int argc, char** argv) {
  using namespace std;
  cerr << "USAGE:  " << *argv << " [options]" << endl << endl;
  cerr << "Where [options] can be:" << endl;
  cerr << "  -h <hostname>  : hostname to connect to (default: "
    << PlayerCc::PLAYER_HOSTNAME << ")" << endl;
  cerr << "  -p <port>      : port where Player will listen (default: "
    << PlayerCc::PLAYER_PORTNUM << ")" << endl;
  cerr << "  -i <index>     : device index (default: 0)"
    << endl;
  cerr << "  -d <level>     : debug message level (0 = none -- 9 = all)"
    << endl;
  cerr << "  -u <rate>      : set server update rate to <rate> in Hz"
    << endl;
  cerr << "  -l      : Use laser if applicable"
    << endl;
  cerr << "  -m <datamode>  : set server data delivery mode"
    << endl;
  cerr << "                      PLAYER_DATAMODE_PUSH = "
    << PLAYER_DATAMODE_PUSH << endl;
  cerr << "                      PLAYER_DATAMODE_PULL = "
    << PLAYER_DATAMODE_PULL << endl;
} // end print_usage

void initialisation() {
  // 0: non exploré
  // 1: déjà exploré
  // 2: obstacle
  for (int i = 0; i < 80; i++)
    for (int j = 0; j < 80; j++)
      carte[i][j] = 0;
  // calculer la distance et l'angle d'une position à la racine
  for (int i = 0; i < taille_laser; i++)
    for (int j = 0; j < taille_laser; j++) {
      int a;
      info[i][j].dist = floor(sqrt((40 - i)*(40 - i) + (40 - j)*(40 - j)));
      if (j == 40) {
        a = (i < 40) ? 90 : 270;
      } else {
        a = round(rtod(atan((double) abs(40 - i) / (double) abs(40 - j))));
        if (i < 40) {
          if (j < 40) a = 180 - a;
        } else {
          if (j < 40) a = 180 + a;
          if (j > 40) a = 360 - a;
        }
      }
      info[i][j].degree = a % 360;
    }
}

//fonction principale
int main(int argc, char **argv) {
  parse_args(argc, argv);
  bool comeback;
  // we throw exceptions on creation if we fail
  try {
    PlayerClient robot("localhost", 6665);
    Position2dProxy pp(&robot, 1);
    LaserProxy lp(&robot, 0);

    std::cout << robot << std::endl;

    pp.SetMotorEnable(true);
    initialisation();
    robot.Read();
    robot.Read();
    int xRacin = pp.GetXPos();
    int yRacin = pp.GetYPos();

    // go into read-think-act loop
    int temps = 0;
    player_pose2d_t target_pose;
    comeback = true;
    bool stop = false;
    cvNamedWindow("carte", 1);
    Mat map(640, 640, CV_8UC1, carte);
    while (!stop) {
      // show image
//      cout << "show carte" << endl;

      for (int i = 0; i < map.rows; i++) {
        for (int j = 0; j < map.cols; j++) {
          map.at<uchar > (i, j) = carte[i][j]*255 / 2;
          //if(map.at<uchar>(i,j)>0) cout<<map.at<uchar > (i, j)<<" ";
        }
        //cout<<endl;
      }
      imshow("carte", map);
      if (waitKey(30) >= 0) continue;
      //waitKey();

      //      for (int i = 0; i < map.rows; i++) {
      //        for (int j = 0; j < map.cols; j++) {
      //          cout << (int) map.at<uchar > (i, j);
      //        }
      //        cout << endl;
      //      }
      // this blocks until new data comes; 10Hz by default
      robot.Read();
      robot.Read();

      /* est le robot est en mouvement ou pas? */
      bool deplacer = ((pp.GetXSpeed() == 0.0) && (pp.GetYSpeed() == 0.0) && (pp.GetYawSpeed() == 0.0)) ? false : true;

      int posX = pp.GetXPos() - xRacin;
      int posY = pp.GetYPos() - yRacin;
      int xcarte = floor(posX / scale) + 319;
      int ycarte = floor(posY / scale) + 319;

      int a0 = round(rtod(pp.GetYaw()));
      if (a0 < 0) a0 += 360;
      int minX, minY, maxX, maxY;
      minX = ((xcarte - 40) < 0) ? 0 : (xcarte - 40);
      minY = ((ycarte - 40) < 0) ? 0 : (ycarte - 40);
      maxX = ((xcarte + 40) > 639) ? 639 : (xcarte + 40);
      maxY = ((ycarte + 40) > 639) ? 639 : (ycarte + 40);

      for (int i = minX; i < maxX; i++) {
        for (int j = minY; j < maxY; j++) {
          if (carte[639 - j][i] != 2) {
            int laserBegin = (360 + a0 - 90) % 360;

            int tmp = (360 + info[80 - j + minY][i - minX].degree - laserBegin) % 360;
            if ((tmp >= 0) && (tmp <= 180)) {
              int dist1 = info[80 - j + minY][i - minX].dist;
              int dist2 = floor(lp[tmp] / scale);
              if (dist2 == 40) {
                if (dist1 <= dist2) {
                  carte[639 - j][i] = 1;
                }//marquer exploré
              } else {
                if (dist1 < dist2) {
                  carte[639 - j][i] = 1; //marquer exploré
                } else if (dist1 == dist2) {
                  carte[639 - j][i] = 2; //marquer l'obstacle
                }
              }//end else
            }
          }
        }//end for 2
      }//end for 1

      if (deplacer) {
        temps++;
        if (temps > 10) {
          if ((abs(pp.GetXPos() - target_pose.px) < 1) && ((abs(pp.GetYPos() - target_pose.py) < 1)))
            pp.SetSpeed(0, 0);
        }
      } else {
        // trouver la position suivant pour déplacer
        // scan tous les laser qui ont la valeur maximale
        //         player_pose2d_t target_pose;
        bool avoirInfo;
        int scan_angle = 0;
        int c;
        while (scan_angle < 181) {
          //         while (scan_angle < 361){
          avoirInfo = false;
          if (lp.GetRange(scan_angle) == 2.0) {
            avoirInfo = true;
            int a = a0 - 90 + scan_angle;
            int dx = ceil(40 * cos(dtor(a)));
            int dy = ceil(40 * sin(dtor(a)));
            int x = xcarte + dx;
            int y = ycarte + dy;
            c = 0;
            for (int xi = x - 1; (xi <= x + 1) && (avoirInfo); xi++)
              for (int yi = y - 1; (yi <= y + 1) && (avoirInfo); yi++) {
                if (carte[639 - yi][xi] == 0) {
                  c++;
                }
                if (carte[639 - yi][xi] == 2) {
                  avoirInfo = false;
                }
              }
            if (avoirInfo) {
              if (c == 0)
                avoirInfo = false;
            }
            if (avoirInfo) {
              double desX, desY;
              desX = pp.GetXPos() + (2 * cos(dtor(a)));
              desY = pp.GetYPos() + (2 * sin(dtor(a)));

              target_pose.px = desX;
              target_pose.py = desY;
              target_pose.pa = dtor(a % 360);
              scan_angle = 182;
              //                   scan_angle = 362;
            }
          }
          scan_angle++;
        }
        if (avoirInfo) {

          pp.GoTo(target_pose, (player_pose2d_t) {
            10, 10, 0
          });
          sleep(1);
          track.push(target_pose);
          comeback = false;
        } else {

          if (comeback) {

            pp.GoTo((player_pose2d_t) {
              pp.GetXPos(), pp.GetYPos(), pp.GetYaw() + dtor(180)
            });
            comeback = false;
          } else {
            if (track.size() != 0) {
              target_pose = track.top();
              track.pop();
              pp.GoTo(target_pose);
              //                   sleep(2);
              comeback = true;
            } else {
              cout << "comeback" << endl;
              stop = true;
            }
          }
        }
      }//end if deplacer
    }

  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return -1;
  }
}

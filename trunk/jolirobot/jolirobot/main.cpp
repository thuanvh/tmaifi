
#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>
#include <math.h>
#include <curses.h>
#include <iostream>
using namespace std;
void display();
void keyspecial(int key,int x,int y);
void keyboard(unsigned char key,int x, int y);
void mousePress(int bouton,int state,int x,int y);
void MouseMotion(int x,int y);
void calcCosSinTable();
void testPosition();
void changePerspective();

float pz=0.0,px=0.0,Sin[360],Cos[360],theta=50;
int xold,r=0;

float angle_armxl0_corp=-195;
float angle_armxr0_corp=-5;
float angle_armyl0_corp=-45;
float angle_armyr0_corp=-215;
float angle_armzl0_corp=45;
float angle_armzr0_corp=45;

float angle_armxl1_corp=-115;
float angle_armxr1_corp=45;
float angle_armyl1_corp=-85;
float angle_armyr1_corp=-55;
float angle_armzl1_corp=45;
float angle_armzr1_corp=45;

float angle_armxl2_corp=175;
float angle_armxr2_corp=45;
float angle_armyl2_corp=-45;
float angle_armyr2_corp=-45;
float angle_armzl2_corp=45;
float angle_armzr2_corp=45;

float angle_legxl0_corp=45;
float angle_legxr0_corp=45;
float angle_legyl0_corp=-45;
float angle_legyr0_corp=-45;
float angle_legzl0_corp=45;
float angle_legzr0_corp=45;

float angle_legxl1_corp=45;
float angle_legxr1_corp=45;
float angle_legyl1_corp=-45;
float angle_legyr1_corp=-45;
float angle_legzl1_corp=45;
float angle_legzr1_corp=45;

float angle_legxl2_corp=45;
float angle_legxr2_corp=45;
float angle_legyl2_corp=-45;
float angle_legyr2_corp=-45;
float angle_legzl2_corp=45;
float angle_legzr2_corp=45;

float angle_headx_corp=0;
float angle_heady_corp=0;
float angle_headz_corp=0;

int main(int argc,char **argv)
{
  
  /* initialisation de glut et creation
   *    de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(500,500);
  glutCreateWindow("29");
  
  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  changePerspective();
  
  /* Precalcul des sinus et cosinus */
  calcCosSinTable();
  
  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyspecial);
  glutMouseFunc(mousePress);
  glutMotionFunc(MouseMotion);
  
  
  /* Entre dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

void display()
{
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  
  
  /*Application des transfos de visualisation */
  glRotated(r,0.0,1.0,0.0);
  glTranslatef(-px,0.0,-pz); /* fixer y */
  
  
  glPushMatrix();
  /* Dessin des objets */
  /* Mur */
  glTranslatef(0,1.5,0);
  glScalef(1.0,0.25,1.0);
  glColor3d(1,0,0);
  glutWireCube(20);
  
  /* robot */
  glPopMatrix();
  glPushMatrix();
  glTranslated(2,-1,3);
  glRotated(-90,1,0,0);
  glColor3f(1.0,1.0,1.0);
//  glRotated(-90,1,0,0);
  //body
  gluCylinder(gluNewQuadric(),0.1,0.2,0.5,20,20);  
  glPushMatrix();
  /* neck */
  glTranslated(0,0,0.5);
  glColor3f(1.0,0,1.0);
  gluCylinder(gluNewQuadric(),0.1,0.1,0.2,20,20);  
  /* head */
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,0,0.8);
  glRotatef(angle_headx_corp,-1,0,0);//angle of head with the body
  glRotatef(angle_heady_corp,0,-1,0);//angle of head with the body
  glRotatef(angle_headz_corp,0,0,-1);//angle of head with the body
  glColor3f(1.0,1.0,0);
  glutWireSphere(0.2,20,20);
  /* eyes */
  glPushMatrix();
  {
    glPopMatrix(); // eye 1
    glPushMatrix();
    glTranslated(0.1,0.2,0.1);
    glColor3f(1.0,0,0);
    //glutWireTorus(0.02,0.5,10,10);
    glutWireSphere(0.05,20,20);
    glPopMatrix(); // eye 2
    glPushMatrix();
    glTranslated(0.1,-0.2,0.1);
    glColor3f(1.0,0,0);
    //glutWireTorus(0.02,0.5,10,10);
    glutWireSphere(0.05,20,20);
  }
  glPopMatrix(); //end eye and head
  
  /* arm 1eft*/
  glPopMatrix();
  glPushMatrix();  
  glTranslated(0,0.2,0.5);
  glRotatef(angle_armxl0_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyl0_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzl0_corp,0,0,-1);//angle of arm with the body    
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  //glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(0.5,0.5,1);
  glRotatef(angle_armxl1_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyl1_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzl1_corp,0,0,-1);//angle of arm with the body  
  
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  //glRotatef(-45,0,-1,0);
  glRotatef(angle_armxl2_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyl2_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzl2_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//hand
  /* arm right*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,-0.2,0.5);
  glRotatef(angle_armxr0_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyr0_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzr0_corp,0,0,-1);//angle of arm with the body  
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  //glRotatef(-45,0,-1,0);//angle of arm with the body
  glRotatef(angle_armxr1_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyr1_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzr1_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
//  glRotatef(-45,0,-1,0);
  glRotatef(angle_armxr2_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_armyr2_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_armzr2_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//hand
  /* leg left*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,0.2,0);
  //glRotatef(135,0,1,0);//angle of arm with the body
  glRotatef(angle_legxl0_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyl0_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzl0_corp,0,0,-1);//angle of arm with the body  
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  //glRotatef(-45,0,-1,0);//angle of arm with the body
  glRotatef(angle_legxl1_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyl1_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzl1_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  //glRotatef(-45,0,-1,0);
  glRotatef(angle_legxl2_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyl2_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzl2_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//foot  
  /* leg 2*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,-0.2,0);
  //glRotatef(135,0,-1,0);//angle of arm with the body
  glRotatef(angle_legxr0_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyr0_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzr0_corp,0,0,-1);//angle of arm with the body  
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  //glRotatef(-45,0,-1,0);//angle of arm with the body
  glRotatef(angle_legxr1_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyr1_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzr1_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  //glRotatef(-90,1,0,0);
  glRotatef(angle_legxr2_corp,-1,0,0);//angle of arm with the body
  glRotatef(angle_legyr2_corp,0,-1,0);//angle of arm with the body
  glRotatef(angle_legzr2_corp,0,0,-1);//angle of arm with the body  
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//foot  
  /* end robot */
  glPopMatrix();
  
//   glPopMatrix();
//   glPushMatrix();
//   glTranslated(5,-1,5);
//   glRotated(-90,1,0,0);
//   glColor3f(1.0,1.0,1.0);
//   glutWireCone(1,2,20,1);
  
  glPopMatrix();
  glPushMatrix();
  glTranslated(-5,0,5);
  glutWireTorus(0.2,0.8,20,30);
  
  /* box */
  glPopMatrix();
  glPushMatrix();
  glTranslated(5,0,-5);
  glRotated(-90,1,0,0);
  glutSolidCube(0.8);
  glTranslated(0,0,0.4);
  glColor3f(0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.2,0.1,0.5,5,5);
  glTranslated(0,0,0.5);
  glColor3f(0.0,0.0,1.0);
  gluCylinder(gluNewQuadric(),0.01,0.01,1,5,5);
  glTranslated(0,0,1);
  glColor3f(1.0,0,1.0);
  glutSolidSphere(0.05,10,10);
  
  /* chair */
  glPopMatrix();
  glPushMatrix();
  glTranslated(7,0,-2.5);
  glRotated(-90,1,0,0);
  gluCylinder(gluNewQuadric(),2,2,0.5,5,5);;
  
  glPushMatrix(); // chair leg
  glColor3f(0,0.5,1.0);
  int posx[4]={1,1,-1,-1};
  int posy[4]={-1,1,-1,1};
  for(int i=0; i<4; i++){
    glPopMatrix();
    glPushMatrix();
    glTranslated(posx[i],posy[i],-2);  
    gluCylinder(gluNewQuadric(),0.2,0.2,2,4,4);
  }
  glPopMatrix(); // end chair
  
  /* bubble */
  glPopMatrix();
  glTranslated(-5,0,-5);
  glutWireSphere(1,20,20);
  
  
  /* on force l'affichage du resultat */
  glFlush();
  glutSwapBuffers();
}

bool isArm=false;
bool isLeg=false;
bool isHead=false;
bool isRight=false;
bool isLeft=false;
int  elementId=-1;
bool isX=false;
bool isY=false;
bool isZ=false;
void reset(){
  isArm=false;
  isLeft=false;
  isLeg=false;
  isRight=false;
  isHead=false;
}
void printOutConfigure(){
  cout<<" angle_armxl0_corp:"<< angle_armxl0_corp<<endl;
  cout<<" angle_armxr0_corp:"<< angle_armxr0_corp<<endl;
  cout<<" angle_armyl0_corp:"<< angle_armyl0_corp<<endl;
  cout<<" angle_armyr0_corp:"<< angle_armyr0_corp<<endl;
  cout<<" angle_armzl0_corp:"<< angle_armzl0_corp<<endl;
  cout<<" angle_armzr0_corp:"<< angle_armzr0_corp<<endl;
  
  cout<<" angle_armxl1_corp:"<< angle_armxl1_corp<<endl;
  cout<<" angle_armxr1_corp:"<< angle_armxr1_corp<<endl;
  cout<<" angle_armyl1_corp:"<< angle_armyl1_corp<<endl;
  cout<<" angle_armyr1_corp:"<< angle_armyr1_corp<<endl;
  cout<<" angle_armzl1_corp:"<< angle_armzl1_corp<<endl;
  cout<<" angle_armzr1_corp:"<< angle_armzr1_corp<<endl;
  
  cout<<" angle_armxl2_corp:"<< angle_armxl2_corp<<endl;
  cout<<" angle_armxr2_corp:"<< angle_armxr2_corp<<endl;
  cout<<" angle_armyl2_corp:"<< angle_armyl2_corp<<endl;
  cout<<" angle_armyr2_corp:"<< angle_armyr2_corp<<endl;
  cout<<" angle_armzl2_corp:"<< angle_armzl2_corp<<endl;
  cout<<" angle_armzr2_corp:"<< angle_armzr2_corp<<endl;
  
  cout<<" angle_legxl0_corp:"<< angle_legxl0_corp<<endl;
  cout<<" angle_legxr0_corp:"<< angle_legxr0_corp<<endl;
  cout<<" angle_legyl0_corp:"<< angle_legyl0_corp<<endl;
  cout<<" angle_legyr0_corp:"<< angle_legyr0_corp<<endl;
  cout<<" angle_legzl0_corp:"<< angle_legzl0_corp<<endl;
  cout<<" angle_legzr0_corp:"<< angle_legzr0_corp<<endl;
  
  cout<<" angle_legxl1_corp:"<< angle_legxl1_corp<<endl;
  cout<<" angle_legxr1_corp:"<< angle_legxr1_corp<<endl;
  cout<<" angle_legyl1_corp:"<< angle_legyl1_corp<<endl;
  cout<<" angle_legyr1_corp:"<< angle_legyr1_corp<<endl;
  cout<<" angle_legzl1_corp:"<< angle_legzl1_corp<<endl;
  cout<<" angle_legzr1_corp:"<< angle_legzr1_corp<<endl;
  
  cout<<" angle_legxl2_corp:"<< angle_legxl2_corp<<endl;
  cout<<" angle_legxr2_corp:"<< angle_legxr2_corp<<endl;
  cout<<" angle_legyl2_corp:"<< angle_legyl2_corp<<endl;
  cout<<" angle_legyr2_corp:"<< angle_legyr2_corp<<endl;
  cout<<" angle_legzl2_corp:"<< angle_legzl2_corp<<endl;
  cout<<" angle_legzr2_corp:"<< angle_legzr2_corp<<endl;
  
  cout<<" angle_headx_corp:"<< angle_headx_corp<<endl;
  cout<<" angle_heady_corp:"<< angle_heady_corp<<endl;
  cout<<" angle_headz_corp:"<< angle_headz_corp<<endl;
}
void normaliseAngle(float& angle,int min,int max){
  angle=(int)angle%360;
  if(angle<0) angle=360+angle;
  if(angle<min)
    angle=min;
  if(angle>max)
    angle=max;
}
void normaliseAllAngle(){
  normaliseAngle(angle_armxl0_corp,5,355);
  normaliseAngle(angle_armxr0_corp,5,355);
  normaliseAngle(angle_armyl0_corp,5,100);
  normaliseAngle(angle_armyr0_corp,5,100);
  normaliseAngle(angle_armzl0_corp,275,55);
  normaliseAngle(angle_armzr0_corp,275,55);
  
  normaliseAngle(angle_armxl1_corp,0,150);
  normaliseAngle(angle_armxr1_corp,0,150);
  normaliseAngle(angle_armyl1_corp,0,0);
  normaliseAngle(angle_armyr1_corp,0,0);
  normaliseAngle(angle_armzl1_corp,0,180);
  normaliseAngle(angle_armzr1_corp,0,180);
  
  normaliseAngle(angle_armxl2_corp,0,90);
  normaliseAngle(angle_armxr2_corp,0,90);
  normaliseAngle(angle_armyl2_corp,0,90);
  normaliseAngle(angle_armyr2_corp,0,90);
  normaliseAngle(angle_armzl2_corp,0,0);
  normaliseAngle(angle_armzr2_corp,0,0);
  
  normaliseAngle(angle_legxl0_corp,0,90);
  normaliseAngle(angle_legxr0_corp,0,90);
  normaliseAngle(angle_legyl0_corp,0,70);
  normaliseAngle(angle_legyr0_corp,0,70);
  normaliseAngle(angle_legzl0_corp,0,0);
  normaliseAngle(angle_legzr0_corp,0,0);
  
  normaliseAngle(angle_legxl1_corp,0,90);
  normaliseAngle(angle_legxr1_corp,0,90);
  normaliseAngle(angle_legyl1_corp,0,0);
  normaliseAngle(angle_legyr1_corp,0,0);
  normaliseAngle(angle_legzl1_corp,0,0);
  normaliseAngle(angle_legzr1_corp,0,0);
  
  normaliseAngle(angle_legxl2_corp,0,90);
  normaliseAngle(angle_legxr2_corp,0,90);
  normaliseAngle(angle_legyl2_corp,0,70);
  normaliseAngle(angle_legyr2_corp,0,70);
  normaliseAngle(angle_legzl2_corp,0,0);
  normaliseAngle(angle_legzr2_corp,0,0);
  
  normaliseAngle(angle_headx_corp,0,360);
  normaliseAngle(angle_heady_corp,0,360);
  normaliseAngle(angle_headz_corp,0,360);
}
void keyspecial(int key,int x,int y){
  switch(key){
    case GLUT_KEY_UP:
      isX=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armxl0_corp+=10;
	  else if(elementId==1) angle_armxl1_corp+=10;
	  else if(elementId==2) angle_armxl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_armxr0_corp+=10;
	  else if(elementId==1) angle_armxr1_corp+=10;
	  else if(elementId==2) angle_armxr2_corp+=10;
	}
      }else if(isHead){
	angle_headx_corp+=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legxl0_corp+=10;
	  else if(elementId==1) angle_legxl1_corp+=10;
	  else if(elementId==2) angle_legxl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_legxr0_corp+=10;
	  else if(elementId==1) angle_legxr1_corp+=10;
	  else if(elementId==2) angle_legxr2_corp+=10;
	}
      }      
      glutPostRedisplay();
      break;
    case GLUT_KEY_DOWN:
      isX=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armxl0_corp-=10;
	  else if(elementId==1) angle_armxl1_corp-=10;
	  else if(elementId==2) angle_armxl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_armxr0_corp-=10;
	  else if(elementId==1) angle_armxr1_corp-=10;
	  else if(elementId==2) angle_armxr2_corp-=10;
	}
      }else if(isHead){
	angle_headx_corp-=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legxl0_corp-=10;
	  else if(elementId==1) angle_legxl1_corp-=10;
	  else if(elementId==2) angle_legxl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_legxr0_corp-=10;
	  else if(elementId==1) angle_legxr1_corp-=10;
	  else if(elementId==2) angle_legxr2_corp-=10;
	}
      }
      glutPostRedisplay();
      break;
    case GLUT_KEY_LEFT:
      isY=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armyl0_corp+=10;
	  else if(elementId==1) angle_armyl1_corp+=10;
	  else if(elementId==2) angle_armyl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_armyr0_corp+=10;
	  else if(elementId==1) angle_armyr1_corp+=10;
	  else if(elementId==2) angle_armyr2_corp+=10;
	}
      }else if(isHead){
	angle_heady_corp+=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legyl0_corp+=10;
	  else if(elementId==1) angle_legyl1_corp+=10;
	  else if(elementId==2) angle_legyl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_legyr0_corp+=10;
	  else if(elementId==1) angle_legyr1_corp+=10;
	  else if(elementId==2) angle_legyr2_corp+=10;
	}
      }
      glutPostRedisplay();
      break;
    case GLUT_KEY_RIGHT:
      isY=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armyl0_corp-=10;
	  else if(elementId==1) angle_armyl1_corp-=10;
	  else if(elementId==2) angle_armyl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_armyr0_corp-=10;
	  else if(elementId==1) angle_armyr1_corp-=10;
	  else if(elementId==2) angle_armyr2_corp-=10;
	}
      }else if(isHead){
	angle_heady_corp-=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legyl0_corp-=10;
	  else if(elementId==1) angle_legyl1_corp-=10;
	  else if(elementId==2) angle_legyl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_legyr0_corp-=10;
	  else if(elementId==1) angle_legyr1_corp-=10;
	  else if(elementId==2) angle_legyr2_corp-=10;
	}
      }
      glutPostRedisplay();
      break;
    case GLUT_KEY_PAGE_UP:
      isZ=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armzl0_corp+=10;
	  else if(elementId==1) angle_armzl1_corp+=10;
	  else if(elementId==2) angle_armzl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_armzr0_corp+=10;
	  else if(elementId==1) angle_armzr1_corp+=10;
	  else if(elementId==2) angle_armzr2_corp+=10;
	}
      }else if(isHead){
	angle_headz_corp+=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legzl0_corp+=10;
	  else if(elementId==1) angle_legzl1_corp+=10;
	  else if(elementId==2) angle_legzl2_corp+=10;
	}else if(isRight){
	  if(elementId==0) angle_legzr0_corp+=10;
	  else if(elementId==1) angle_legzr1_corp+=10;
	  else if(elementId==2) angle_legzr2_corp+=10;
	}
      }
      glutPostRedisplay();
      break;
    case GLUT_KEY_PAGE_DOWN:
      isZ=true;
      if(isArm){
	if(isLeft){
	  if(elementId==0) angle_armzl0_corp-=10;
	  else if(elementId==1) angle_armzl1_corp-=10;
	  else if(elementId==2) angle_armzl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_armzr0_corp-=10;
	  else if(elementId==1) angle_armzr1_corp-=10;
	  else if(elementId==2) angle_armzr2_corp-=10;
	}
      }else if(isHead){
	angle_headz_corp-=10;
      }else if(isLeg){
	if(isLeft){
	  if(elementId==0) angle_legzl0_corp-=10;
	  else if(elementId==1) angle_legzl1_corp-=10;
	  else if(elementId==2) angle_legzl2_corp-=10;
	}else if(isRight){
	  if(elementId==0) angle_legzr0_corp-=10;
	  else if(elementId==1) angle_legzr1_corp-=10;
	  else if(elementId==2) angle_legzr2_corp-=10;
	}
      }
      glutPostRedisplay();
      break;
  }
  normaliseAllAngle();
  printOutConfigure();
}
void keyboard(unsigned char key,int x, int y)
{
//   cout<<key<<" = "<<(int)key<<endl;
  switch (key)
  {
    case 'q':
      exit(0);
    case 'a':
      reset();
      isArm=true;      
      break;
    case 'f':
      reset();
      isLeg=true;
      break;
    case 'l':
      isLeft=true;
      break;
    case 'r':
      isRight=true;
      break;
    case 'h':
      reset();
      isHead=true;
      break;
    case '0':
    case '1':
    case '2':
      elementId=key-'0';
      break;
    case 'z':
      pz-=0.5*Cos[r];
      px+=0.5*Sin[r];
      testPosition();
      glutPostRedisplay();
      break;
    case 's':
      pz+=0.5*Cos[r];
      px-=0.5*Sin[r];
      testPosition();
      glutPostRedisplay();
      break;
    case 'b':
      theta+=1;
      if (theta>180)
	theta=180;
      changePerspective();
      glutPostRedisplay();
      break;
    case 'n':
      theta-=1;
      if (theta<0)
	theta=0;
      changePerspective();
      glutPostRedisplay();
      break;
    
  }
}



void mousePress(int bouton,int state,int x,int y)
{
  if (bouton==GLUT_LEFT_BUTTON&&state==GLUT_DOWN){
    xold=x;
  }
}



void MouseMotion(int x,int y)
{
  r+=x-xold;
  if (r>=360)
    r-=360;
  if (r<0)
    r=360+r;
  xold=x;
  glutPostRedisplay();
}



void calcCosSinTable()
{
  int i;
  for (i=0;i<360;i++){
    Sin[i]=sin(i/360.0*6.283185);
    Cos[i]=cos(i/360.0*6.283185);
  }
}



void testPosition()
{
  if (px>9.8)
    px=9.8;
  if (px<-9.8)
    px=-9.8;
  if (pz>9.8)
    pz=9.8;
  if (pz<-9.8)
    pz=-9.8;
}



void changePerspective()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(theta,1.0,0.1,40.0);
  glMatrixMode(GL_MODELVIEW);
}



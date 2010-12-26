/********************************************************/
/*                     ballade.c                        */
/********************************************************/
/* Petite ballade dans un monde (reduit) en fil de fer  */
/********************************************************/

/* inclusion des fichiers d'entete Glut */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

void display();
void keyboard(unsigned char key,int x, int y);
void mousePress(int bouton,int state,int x,int y);
void MouseMotion(int x,int y);
void calcCosSinTable();
void testPosition();
void changePerspective();

float pz=0.0,px=0.0,Sin[360],Cos[360],theta=50;
int xold,r=0;


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
  glColor3f(1.0,1.0,0);
  glutWireSphere(0.2,20,20);
  /* eyes */
  glPushMatrix();
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
  glPopMatrix(); //end eye
  
  /* arm 1*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,0.2,0.5);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glRotatef(-45,0,-1,0);
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//hand
  /* arm 2*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,-0.2,0.5);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glRotatef(-45,0,-1,0);
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//hand
  /* leg 1*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,0.2,0);
  glRotatef(135,0,1,0);//angle of arm with the body
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glRotatef(-45,0,-1,0);
  glColor3f(0.5,1,0.5);
  glutWireCone(0.1,0.1,20,1);//foot  
  /* leg 2*/
  glPopMatrix();
  glPushMatrix();
  glTranslated(0,-0.2,0);
  glRotatef(135,0,-1,0);//angle of arm with the body
  glColor3f(1.0,0.5,1.0);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glColor3f(0.5,0.5,0.2);
  glutWireSphere(0.05,20,20);
  glRotatef(-45,0,-1,0);//angle of arm with the body
  glColor3f(0.5,0.5,1);
  gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
  glTranslated(0,0,0.3);
  glRotatef(-45,0,-1,0);
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


void keyboard(unsigned char key,int x, int y)
{
  switch (key)
  {
    case 'q':
      exit(0);
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
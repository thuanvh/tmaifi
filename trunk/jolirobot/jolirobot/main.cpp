
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <GL/glut.h>
#include <math.h>
#include <tiffio.h>
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
void initLight();
void printOutConfigure();
void normaliseAngle(float& angle,int min,int max);
void initTexture();
void chargeTextureJpeg(char *fichier,int numtex);
void chargeTextureTiff(char *fichier,int numtex);
void chargeTextureProc(int numtex);
int fonctionTexture(int x,int y);
void displayRoom();
enum Material{ Rubic,Metal,Plastic, Mat,Normal};
void setMaterial(float r,float g,float b,int shademode=GL_SMOOTH,Material material=Normal);
float pz=0.0,px=0.0,Sin[360],Cos[360],theta=50;
int xold,r=0;

float angle_armxl0_corp=180;
float angle_armxr0_corp=180;
float angle_armyl0_corp=5;
float angle_armyr0_corp=355;
float angle_armzl0_corp=0;
float angle_armzr0_corp=0;

float angle_armxl1_corp=0;
float angle_armxr1_corp=0;
float angle_armyl1_corp=0;
float angle_armyr1_corp=0;
float angle_armzl1_corp=0;
float angle_armzr1_corp=0;

float angle_armxl2_corp=180;
float angle_armxr2_corp=180;
float angle_armyl2_corp=0;
float angle_armyr2_corp=0;
float angle_armzl2_corp=0;
float angle_armzr2_corp=0;

float angle_legxl0_corp=180;
float angle_legxr0_corp=180;
float angle_legyl0_corp=0;
float angle_legyr0_corp=0;
float angle_legzl0_corp=0;
float angle_legzr0_corp=0;

float angle_legxl1_corp=0;
float angle_legxr1_corp=0;
float angle_legyl1_corp=0;
float angle_legyr1_corp=0;
float angle_legzl1_corp=0;
float angle_legzr1_corp=0;

float angle_legxl2_corp=180;
float angle_legxr2_corp=180;
float angle_legyl2_corp=0;
float angle_legyr2_corp=0;
float angle_legzl2_corp=0;
float angle_legzr2_corp=0;

float angle_headx_corp=0;
float angle_heady_corp=0;
float angle_headz_corp=0;

float angle_bodyx_corp=0;
float angle_bodyy_corp=0;
float angle_bodyz_corp=0;

float angle_allz_corp=0;
float robot_posx=0;
float robot_posy=0;

int IdTex[5]; /* tableau d'Id pour les 2 textures */
float decalage=5; /* décalage de la texture procedurale pour l'animation */

#define NUM_LIGHT_SOURCE 2
int lightSourceId=0;
bool lightEnableAll;
bool lightControlAll;
bool lightEnable[NUM_LIGHT_SOURCE];
float lightSourcePos[NUM_LIGHT_SOURCE][3]={
  5.0,2.0,5.0,
  0,0,5.0};
bool isLight;
#define LIGHT_POSITION 0
#define LIGHT_ROTATION 1
int lightChangeMode;

#define NUM_LIGHT_COLOR 4
int lightColor[NUM_LIGHT_SOURCE];
#define MATERIAL_LAMBERT true
#define MATERIAL_GOURAUD false
bool materialMode;

int main(int argc,char **argv)
{
  
  /* initialisation de glut et creation
   *    de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(700,700);
  glutCreateWindow("29");
  initLight();
  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  
  glEnable(GL_DEPTH_TEST);
  changePerspective();
  initTexture();
  /* Chargement de la texture */
  
  
  
  /* Precalcul des sinus et cosinus */
  calcCosSinTable();
  
  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyspecial);
  glutMouseFunc(mousePress);
  glutMotionFunc(MouseMotion);
  //glutTimerFunc();
  
  /* Entre dans la boucle principale glut */
  glutMainLoop();
  return 0;
}
void initTexture(){
/* Chargement des textures */
  glGenTextures(5,(GLuint*)IdTex);
  //chargeTextureTiff("texture.tif",IdTex[0]);
  chargeTextureJpeg("daisy.jpg",IdTex[0]);
  chargeTextureProc(IdTex[1]);
//   chargeTextureJpeg("ball.jpg",IdTex[2]);
  chargeTextureJpeg("golf.jpg",IdTex[2]);  
  chargeTextureJpeg("brick.256.jpg",IdTex[3]);
  chargeTextureJpeg("floor.jpg",IdTex[4]); 
  glEnable(GL_TEXTURE_2D);
}

/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
void initLight() 
{
  /* Paramétrage des lumières */
  GLfloat L0pos[]={ lightSourcePos[0][0],lightSourcePos[0][1],lightSourcePos[0][2]};
  GLfloat L0dif[]={ 0.5,0.5,0.5};
  GLfloat L1pos[]={ lightSourcePos[1][0],lightSourcePos[1][1],lightSourcePos[1][2]};
  GLfloat L1dif[]={ 0.2,0.2,0.8};
  GLfloat Mspec[]={ 1,1,1};
  GLfloat Mambi[]={ 0.1,0.1,0.1};
  GLfloat Mshiny=50;
  glShadeModel(GL_SMOOTH);//FLAT
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
  if(lightEnableAll){
    glEnable(GL_LIGHTING);
    if(lightEnable[0]){
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_AMBIENT,Mambi);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,L0dif);
      glLightfv(GL_LIGHT0,GL_SPECULAR,L0dif);
      glLightfv(GL_LIGHT0,GL_POSITION,L0pos);
    }
    else{
      glDisable(GL_LIGHT0);
    }
    if(lightEnable[1]){
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1,GL_AMBIENT,Mambi);
      glLightfv(GL_LIGHT1,GL_DIFFUSE,L1dif);
      glLightfv(GL_LIGHT1,GL_SPECULAR,L1dif);
      glLightfv(GL_LIGHT1,GL_POSITION,L1pos);
    }else{
      glDisable(GL_LIGHT1);
    }
    
  }else{
    glDisable(GL_LIGHTING);    
  }
  
  /* Paramétrage du matériau */
//   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Mspec);
//   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,Mshiny);
  
  glMaterialfv(GL_FRONT,GL_SPECULAR,Mspec);
  glMaterialf(GL_FRONT,GL_SHININESS,Mshiny);
  
}

void setMaterial(float r,float g,float b,int shademode,Material material){
  glColor3f(r,g,b);
  GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
  //GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
  GLfloat mat_ambient_color[] = { r, g, b, 1.0 };
  //GLfloat mat_diffuse[] = { 1, 0.5, 0.8, 1.0 };
  GLfloat mat_diffuse[] = { r, g, b, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat no_shininess[] = { 0.0 };
  GLfloat low_shininess[] = { 5.0 };
  GLfloat high_shininess[] = { 100.0 };
  GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
//   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  
  switch(material){
    case Plastic:
      glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
      glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); 
      glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
      break;
    case Rubic:
      glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
      glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);  
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
      break;
    case Normal:
      glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
      glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);  
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
      break;
  }
  glShadeModel(shademode);
}
void displayRobot(){
  /* robot */
  {
    glPopMatrix();
    glPushMatrix();
    //glTranslated(2,-1,4);
    glTranslated(0,0,-5);
    //     glViewport(2,2,5,5);
    glTranslated(robot_posx,0,robot_posy);
    glRotated(-90,1,0,0);    
    glRotatef(angle_allz_corp,0,0,-1);//angle of head with the body
    
    //glColor3f(1.0,1.0,1.0);
    setMaterial(0,1.0,0);
    //  glRotated(-90,1,0,0);
    //body
    glPushMatrix();
    glRotatef(angle_bodyx_corp,-1,0,0);//angle of head with the body
    glRotatef(angle_bodyy_corp,0,-1,0);//angle of head with the body
    glRotatef(angle_bodyz_corp,0,0,-1);//angle of head with the body
    gluCylinder(gluNewQuadric(),0.15,0.25,0.5,20,20);  
    
    glPushMatrix();
    {
      /* neck */
      glTranslated(0,0,0.5);
      glRotatef(angle_headx_corp,-1,0,0);//angle of head with the body
      glRotatef(angle_heady_corp,0,-1,0);//angle of head with the body
      glRotatef(angle_headz_corp,0,0,-1);//angle of head with the body
      //glColor3f(1.0,0,1.0);
      setMaterial(1.0,0,1.0);
      gluCylinder(gluNewQuadric(),0.1,0.1,0.2,20,20);  
      glPushMatrix();
      {
	/* head */
	glPopMatrix();
	glPushMatrix();
	glTranslated(0,0,0.3);  
	//glColor3f(1.0,1.0,0);
	setMaterial(1.0,1.0,0);
	glutSolidSphere(0.2,20,20);
	/* eyes */
	
	glPushMatrix();
	{
	  glPopMatrix(); // eye 1
	  glPushMatrix();
	  glTranslated(0.1,-0.15,0.1);
	  setMaterial(1.0,0,0,GL_FLAT,Rubic);
	  //glutWireTorus(0.02,0.5,10,10);
	  glutSolidSphere(0.05,10,10);
	  
	  glPopMatrix(); // eye 2
	  glPushMatrix();
	  glTranslated(-0.1,-0.15,0.1);
	  //glColor3f(1.0,0,0);
	  setMaterial(1.0,0,0,GL_FLAT,Rubic);
	  //glutWireTorus(0.02,0.5,10,10);
	  glutSolidSphere(0.05,10,10);
	  
	  glPopMatrix(); // mouth
	  glPushMatrix();
	  glTranslated(0,-0.15,-0.1);
	  //glColor3f(1.0,0,0);
	  setMaterial(1.0,0,0,GL_FLAT,Rubic);
	  //glutWireTorus(0.02,0.5,10,10);
	  glutSolidCube(0.05);
	}
	glPopMatrix(); //end eye and head
      }
      glPopMatrix();// end neck and head
      
      /* arm 1eft*/
      glPopMatrix();
      glPushMatrix();  
      glTranslated(-0.25,0,0.5);
      glRotatef(angle_armxl0_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyl0_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzl0_corp,0,0,-1);//angle of arm with the body    
      //glColor3f(1.0,0.5,1.0);
      setMaterial(1.0,0.5,1.0);
      gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
      glTranslated(0,0,0.32);
      //glColor3f(0.5,0.5,0.2);
      setMaterial(0.2,0.5,0.2,GL_FLAT,Rubic);
      glutSolidSphere(0.06,20,20);
      //glRotatef(-45,0,-1,0);//angle of arm with the body
      //glColor3f(0.5,0.5,1);
      glTranslated(0,0,0.02);
      setMaterial(0.5,0.5,1,GL_SMOOTH,Plastic);
      glRotatef(angle_armxl1_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyl1_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzl1_corp,0,0,-1);//angle of arm with the body        
      gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
      glTranslated(0,0,0.35);
      //glRotatef(-45,0,-1,0);
      glRotatef(angle_armxl2_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyl2_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzl2_corp,0,0,-1);//angle of arm with the body  
      //glColor3f(0.5,1,0.5);
      setMaterial(0.5,1,0.5,GL_FLAT,Rubic);
      glutSolidCone(0.1,0.1,10,1);//hand
      
      /* arm right*/
      glPopMatrix();
      glPushMatrix();
      glTranslated(0.25,0,0.5);
      glRotatef(angle_armxr0_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyr0_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzr0_corp,0,0,-1);//angle of arm with the body  
      //glColor3f(1.0,0.5,1.0);
      setMaterial(1.0,0.5,1.0);
      gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
      glTranslated(0,0,0.32);
      //glColor3f(0.5,0.5,0.2);
      setMaterial(0.2,0.5,0.2,GL_FLAT,Rubic);
      glutSolidSphere(0.06,20,20);
      //glRotatef(-45,0,-1,0);//angle of arm with the body
      glTranslated(0,0,0.02);
      glRotatef(angle_armxr1_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyr1_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzr1_corp,0,0,-1);//angle of arm with the body  
      //glColor3f(0.5,0.5,1);
      setMaterial(0.5,0.5,1,GL_SMOOTH,Plastic);
      gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
      glTranslated(0,0,0.35);
      //  glRotatef(-45,0,-1,0);
      glRotatef(angle_armxr2_corp,-1,0,0);//angle of arm with the body
      glRotatef(angle_armyr2_corp,0,-1,0);//angle of arm with the body
      glRotatef(angle_armzr2_corp,0,0,-1);//angle of arm with the body  
      //glColor3f(0.5,1,0.5);
      setMaterial(0.5,1,0.5,GL_FLAT,Rubic);
      glutSolidCone(0.1,0.1,10,1);//hand
    }
    glPopMatrix();//end body and up
    
    /* leg left*/
    glPopMatrix();
    glPushMatrix();
    glTranslated(-0.1,0,0);
    //glRotatef(135,0,1,0);//angle of arm with the body
    glRotatef(angle_legxl0_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyl0_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzl0_corp,0,0,-1);//angle of arm with the body  
    setMaterial(1.0,0.5,1.0);
    gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
    glTranslated(0,0,0.32);
    setMaterial(0.5,0.5,0.2);
    glutSolidSphere(0.06,20,20);
    //glRotatef(-45,0,-1,0);//angle of arm with the body
    glTranslated(0,0,0.02);
    glRotatef(angle_legxl1_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyl1_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzl1_corp,0,0,-1);//angle of arm with the body  
    setMaterial(0.5,0.5,1);
    gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
    glTranslated(0,0,0.35);
    //glRotatef(-45,0,-1,0);
    glRotatef(angle_legxl2_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyl2_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzl2_corp,0,0,-1);//angle of arm with the body  
    setMaterial(0.5,1,0.5,GL_FLAT);
    glutSolidCone(0.1,0.1,10,1);//foot  
    
    /* leg 2*/
    glPopMatrix();
    glPushMatrix();
    glTranslated(0.1,0,0);
    //glRotatef(135,0,-1,0);//angle of arm with the body
    glRotatef(angle_legxr0_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyr0_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzr0_corp,0,0,-1);//angle of arm with the body  
    setMaterial(1.0,0.5,1.0);
    gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
    glTranslated(0,0,0.32);
    setMaterial(0.5,0.5,0.2);
    glutSolidSphere(0.06,20,20);
    //glRotatef(-45,0,-1,0);//angle of arm with the body
    glTranslated(0,0,0.02);
    glRotatef(angle_legxr1_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyr1_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzr1_corp,0,0,-1);//angle of arm with the body  
    setMaterial(0.5,0.5,1);
    gluCylinder(gluNewQuadric(),0.05,0.05,0.3,20,20);
    glTranslated(0,0,0.35);
    //glRotatef(-90,1,0,0);
    glRotatef(angle_legxr2_corp,-1,0,0);//angle of arm with the body
    glRotatef(angle_legyr2_corp,0,-1,0);//angle of arm with the body
    glRotatef(angle_legzr2_corp,0,0,-1);//angle of arm with the body  
    setMaterial(0.5,1,0.5,GL_FLAT);
    glutSolidCone(0.1,0.1,10,1);//foot  
    /* end robot */
    glPopMatrix();
  }
}
void displayBox(){
  /* box */
  glPopMatrix();
  glPushMatrix();
  glTranslated(5,0,-5);
  glRotated(-90,1,0,0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,IdTex[0]);  
  glutSolidCube(1.5);  
  glDisable(GL_TEXTURE_2D);
  glTranslated(0,0,0.7);
  setMaterial(0,0.5,1.0);  
  gluCylinder(gluNewQuadric(),0.2,0.1,0.5,5,5);
  glPushMatrix();
  int angle[3]={0,45,-45};
  for(int i=0; i<3; i++){
    glPopMatrix();
    glPushMatrix();
    glTranslated(0,0,0.5);
    glRotatef(angle[i],1,0,0);
    setMaterial(0.0,0.0,1.0);    
    
    gluCylinder(gluNewQuadric(),0.01,0.01,1,5,5);
    glTranslated(0,0,1);
    setMaterial(1.0,0,1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,IdTex[2]);  
    glutSolidSphere(0.1,10,10);    
    glDisable(GL_TEXTURE_2D);
    
  }
  glPopMatrix();
}
void displayChair(){
  /* chair */
  glPopMatrix();
  glPushMatrix();
  glTranslated(7,0,-2.5);
  glRotated(-90,1,0,0);
  glScalef(0.5,0.5,0.5);
  setMaterial(0.7,0.5,0.2,GL_SMOOTH,Plastic);
  gluCylinder(gluNewQuadric(),2,2,0.5,25,25);
  
  glPushMatrix(); // chair leg  
  setMaterial(0,0.5,1.0,GL_SMOOTH,Plastic);
  int posx[4]={1,1,-1,-1};
  int posy[4]={-1,1,-1,1};
  for(int i=0; i<4; i++){
    glPopMatrix();
    glPushMatrix();
    glTranslated(posx[i],posy[i],-2);  
    gluCylinder(gluNewQuadric(),0.2,0.2,2,10,10);
  }
  glPopMatrix(); // end chair
}
void displayBall(){
  /* bubble */
  glPopMatrix();
  glPopMatrix();
  //   glPushMatrix();
  glTranslated(-5,0,-5);
  //   glBindTexture(GL_TEXTURE_2D,IdTex[2]);
  //   glutSolidSphere(1,20,20);
  //   gluSphere(gluNewQuadric(), 0.35f, 32, 16);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, IdTex[2]);				// Select Texture 3 (2)
  glColor4f(1.0f, 1.0f, 1.0f, 0.0f);					// Set Color To White With 40% Alpha
  //   glEnable(GL_BLEND);							// Enable Blending
  //   glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set Blending Mode To Mix Based On SRC Alpha
  glEnable(GL_TEXTURE_GEN_S);						// Enable Sphere Mapping
  glEnable(GL_TEXTURE_GEN_T);						// Enable Sphere Mapping
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glutSolidSphere(0.5f,20,20);
  //gluSphere(gluNewQuadric(), 0.35f, 32, 16);						// Draw Another Sphere Using New Texture
  // Textures Will Mix Creating A MultiTexture Effect (Reflection)
  glDisable(GL_TEXTURE_GEN_S);						// Disable Sphere Mapping
  glDisable(GL_TEXTURE_GEN_T);						// Disable Sphere Mapping
  //   glDisable(GL_BLEND);							// Disable Blending
  glDisable(GL_TEXTURE_2D);
  //   glPopMatrix();
}
void display()
{
  
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  initLight();
  glDisable(GL_TEXTURE_2D);
  /*Application des transfos de visualisation */
  glRotated(r,0.0,1.0,0.0);
  glTranslatef(-px,0.0,-pz); /* fixer y */
  
  
  glPushMatrix();
  /* Dessin des objets */
  /* Mur */  
  displayRoom();
  
  displayRobot();
//   glPopMatrix();
//   glPushMatrix();
//   glTranslated(5,-1,5);
//   glRotated(-90,1,0,0);
//   glColor3f(1.0,1.0,1.0);
//   glutWireCone(1,2,20,1);
  
  glPopMatrix();
  glPushMatrix();
  glTranslated(-5,0,5);
  glRotated(180,0,0,1);
  glutSolidTorus(0.2,0.8,20,30);
  
  displayBox();
  
  displayChair();
  
  displayBall();
  /* on force l'affichage du resultat */
  glFlush();
  glutSwapBuffers();
}

void displayRoom(){
  glEnable(GL_TEXTURE_2D);
  float boxsize=10;
  glTranslatef(0,1.5,0);
  glScalef(1.0,0.25,1.0);
//   glTranslatef(0,boxsize/10,0);
  setMaterial(0.3,0.3,0.3);
  //glBindTexture(GL_TEXTURE_CUBE_MAP,IdTex[0]);
  //glutSolidCube(20);  
    
//     /* Positionnement de l'observateur (ou de l'objet) */
//     glLoadIdentity();
//     gluLookAt(0.0,0.0,distance,0.0,0.0,0.0,0.0,1.0,0.0);
//     glRotatef(angley,1.0,0.0,0.0);
//     glRotatef(anglex,0.0,1.0,0.0);
    
    /* Description de l'obet */
    glBindTexture(GL_TEXTURE_2D,IdTex[3]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0,0.0,-1.0);
    glTexCoord2f(0.0,1.0);   glVertex3f(-boxsize, boxsize, boxsize);
    glTexCoord2f(0.0,0.0);   glVertex3f(-boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f( boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f( boxsize, boxsize, boxsize);
    glEnd();
    
    
    glBindTexture(GL_TEXTURE_2D,IdTex[3]);
    glBegin(GL_POLYGON); 
    glNormal3f(-1.0,0.0,0.0);
    glTexCoord2f(0.0,1.0);   glVertex3f( boxsize, boxsize, boxsize);
    glTexCoord2f(0.0,0.0);   glVertex3f( boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f( boxsize,-boxsize,-boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f( boxsize, boxsize,-boxsize);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,IdTex[3]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0,0.0,1.0);
    glTexCoord2f(0.0,1.0);   glVertex3f( boxsize, boxsize,-boxsize);
    glTexCoord2f(0.0,0.0);   glVertex3f( boxsize,-boxsize,-boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f(-boxsize,-boxsize,-boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f(-boxsize, boxsize,-boxsize);
    glEnd();
    
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,IdTex[3]);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBegin(GL_POLYGON);
    glNormal3f(1.0,0.0,0.0);
    glTexCoord2f(0.0,1.0);   glVertex3f(-boxsize, boxsize,-boxsize);
    glTexCoord2f(0.0,0.0);   glVertex3f(-boxsize,-boxsize,-boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f(-boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f(-boxsize, boxsize, boxsize);
    glEnd();
    
    // ceiling
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,IdTex[1]);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBegin(GL_POLYGON);
    glNormal3f(0.0,-1.0,0.0);
    glTexCoord2f(0.0,1.0);   glVertex3f(-boxsize, boxsize,-boxsize);
    glTexCoord2f(0.0,0.0);   glVertex3f(-boxsize, boxsize, boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f( boxsize, boxsize, boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f( boxsize, boxsize,-boxsize);
    glEnd();
    
    //floor
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,IdTex[4]);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBegin(GL_POLYGON);
    glNormal3f(0.0,1.0,0.0);
    glTexCoord2f(0.0,0.0);   glVertex3f(-boxsize,-boxsize,-boxsize);
    glTexCoord2f(0.0,1.0);   glVertex3f(-boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,1.0);   glVertex3f( boxsize,-boxsize, boxsize);
    glTexCoord2f(1.0,0.0);   glVertex3f( boxsize,-boxsize,-boxsize);
    glEnd();
    
    
    glDisable(GL_TEXTURE_2D);
  
}
bool isArm=false;
bool isLeg=false;
bool isHead=false;
bool isBody=false;
bool isRight=false;
bool isLeft=false;
bool isTotal=false;
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
  isBody=false;
  isTotal=false;
}

void go(){
#define NUM_ACT 6
#define NUM_FOOT 5
//   float legleft0[NUM_ACT]={180,200,180,160,180};
//   float legright0[NUM_ACT]={160,160,180,200,180};
  float legleft0[NUM_ACT]={0,1,0,-1,-1,0};
  float legright0[NUM_ACT]={-1,-1,0,0,1,0};
//   float legleft1[NUM_ACT]={};
//   float legright1[NUM_ACT]={};
  int angle_leg=80;
  int small=5;
  int angle_leg2=90;
  int angle_small=angle_leg/small;
  int angle_small2=angle_leg2/small*2;
  float veloc=Sin[angle_small]*0.3;
  for(int k=0; k<NUM_FOOT; k++){
    for(int i=0; i<NUM_ACT; i++){
      for(int l=1; l<=small; l++){
	
	angle_legxr0_corp= (180+legright0[i]*angle_small);
	angle_legxl0_corp= (180+legleft0[i]*angle_small);
	normaliseAngle(angle_legxl0_corp,0,360);
	normaliseAngle(angle_legxr0_corp,0,360);
	
	int delta_angle2=0;
	if(l-1>small/2)
	  delta_angle2=(small-l)*angle_small2;
	else
	  delta_angle2=(l-1)*angle_small2;
	if(legright0[i]<0) 
	  angle_legxr1_corp= 0+delta_angle2;
	else
	  angle_legxr1_corp=0;
	if(legleft0[i]<0) 
	  angle_legxl1_corp= 0+delta_angle2;
	else
	  angle_legxl1_corp=0;
	//cout<<angle_legxr1_corp<<"-"<<angle_legxl1_corp<<" ";
	
	robot_posy+=veloc*Cos[(int)angle_allz_corp];
	robot_posx+=veloc*Sin[(int)angle_allz_corp];
	
	//cout<<angle_allz_corp<<endl;
	//printOutConfigure();
	display();
	glFlush();
	glutSwapBuffers();
	//glutPostRedisplay();
	//glDrawBuffer(GL_BACK);
	//sleep(1);
      }
    }  
  }
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
  
  cout<<" angle_bodyx_corp:"<< angle_bodyx_corp<<endl;
  cout<<" angle_bodyy_corp:"<< angle_bodyy_corp<<endl;
  cout<<" angle_bodyz_corp:"<< angle_bodyz_corp<<endl;
  cout<<" light position:"<<endl;
  for(int i=0; i<NUM_LIGHT_SOURCE; i++){
    cout<<i<<": ";
    for(int j=0; j<3; j++){
      cout<<lightSourcePos[i][j]<<" ";
    }
    cout<<endl;
  }
}

void normaliseAngle(float& angle,int min,int max){
  min=min%360;
  max=max%360;
  
  
//   angle=(int)angle%360;
  if(min==290) cout<<angle<<endl;
  if(max<min) {
    if(angle<min)
      angle=angle+360;
    max=max+360;
//     int a=max;
//     max=min;
//     min=a;
  }
  if(min==290)
    cout<<angle<<"-"<<max<<"-"<<min<<endl;
  if(angle>max || angle<min) {
    //angle=abs(max-angle)%360<abs(angle-min)%360?max:min;
    if(abs((int)angle%360-min%360)<=10)
      angle=min;
    else angle=max;
  } 
  if(min==290) cout<<angle<<endl;
//   if(angle<min && min<=max)
//     angle=min;
//   else if(angle>max && max>=min)
//     angle=max;
//   else if(angle<max && max<=min)
//     angle=max;
//   else if(angle>min && min>=max)
//     angle=min;
  angle=(int)angle%360;
  if(angle<0) angle=360+angle;
}

void normaliseAllAngle(){
  normaliseAngle(angle_armxl0_corp,90,350);
  normaliseAngle(angle_armxr0_corp,90,350);
  normaliseAngle(angle_armyl0_corp,5,100);
  normaliseAngle(angle_armyr0_corp,260,355);
  normaliseAngle(angle_armzl0_corp,275,0);
  normaliseAngle(angle_armzr0_corp,275,0);
  
  normaliseAngle(angle_armxl1_corp,0,150);
  normaliseAngle(angle_armxr1_corp,0,150);
  normaliseAngle(angle_armyl1_corp,0,0);
  normaliseAngle(angle_armyr1_corp,0,0);
  normaliseAngle(angle_armzl1_corp,0,180);
  normaliseAngle(angle_armzr1_corp,0,180);
  
  normaliseAngle(angle_armxl2_corp,135,225);
  normaliseAngle(angle_armxr2_corp,135,225);
  normaliseAngle(angle_armyl2_corp,315,45);
  normaliseAngle(angle_armyr2_corp,315,45);
  normaliseAngle(angle_armzl2_corp,0,0);
  normaliseAngle(angle_armzr2_corp,0,0);
  
  normaliseAngle(angle_legxl0_corp,0,200);
  normaliseAngle(angle_legxr0_corp,0,200);
  normaliseAngle(angle_legyl0_corp,0,70);
  normaliseAngle(angle_legyr0_corp,290,0);
  normaliseAngle(angle_legzl0_corp,0,0);
  normaliseAngle(angle_legzr0_corp,0,0);
  
  normaliseAngle(angle_legxl1_corp,270,0);
  normaliseAngle(angle_legxr1_corp,270,0);
  normaliseAngle(angle_legyl1_corp,0,0);
  normaliseAngle(angle_legyr1_corp,0,0);
  normaliseAngle(angle_legzl1_corp,0,0);
  normaliseAngle(angle_legzr1_corp,0,0);
  
  normaliseAngle(angle_legxl2_corp,135,225);
  normaliseAngle(angle_legxr2_corp,135,225);
  normaliseAngle(angle_legyl2_corp,315,45);
  normaliseAngle(angle_legyr2_corp,315,45);
  normaliseAngle(angle_legzl2_corp,0,0);
  normaliseAngle(angle_legzr2_corp,0,0);
  
  normaliseAngle(angle_headx_corp,315,45);
  normaliseAngle(angle_heady_corp,315,45);
  normaliseAngle(angle_headz_corp,315,45);
  
  normaliseAngle(angle_bodyx_corp,240,5);
  normaliseAngle(angle_bodyy_corp,315,45);
  normaliseAngle(angle_bodyz_corp,315,45);
  
  normaliseAngle(angle_allz_corp,0,360);
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
      }else if(isBody){
	angle_bodyx_corp+=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][0]+=1;
      }
      
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
      }else if(isBody){
	angle_bodyx_corp-=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][0]-=1;
      }
      
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
      }else if(isBody){
	angle_bodyy_corp+=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][1]+=1;
      }
     
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
      }else if(isBody){
	angle_bodyy_corp-=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][1]-=1;
      }
      
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
      }else if(isBody){
	angle_bodyz_corp+=10;
      }else if(isTotal){
	angle_allz_corp+=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][2]+=1;
      }
      
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
      }else if(isBody){
	angle_bodyz_corp-=10;
      }else if(isTotal){
	angle_allz_corp-=10;
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
      }else if(isLight && !lightControlAll){
	lightSourcePos[elementId][2]-=1;
      }
      
      break;
  }
  if(isLight && !lightControlAll){
    initLight();
  }
  glutPostRedisplay();
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
      if(isArm || isLeg){
	isLeft=true;
	isRight=false;
      }else if(isLight && !lightControlAll){
	lightChangeMode=LIGHT_POSITION;
      }
      break;
    case 'r':
      if(isArm || isLeg){
	isRight=true;
	isLeft=false;
      }else if(isLight && !lightControlAll){
	lightChangeMode=LIGHT_ROTATION;
      }
      break;
    case 'h':
      reset();
      isHead=true;
      break;
    case 'b':
      reset();
      isBody=true;
      break;
    case 'g':
      reset();
      go();
      break;
    case 'c':
      reset();
      isTotal=true;
      break;
    case 'o':
      reset();
      isLight=true;
      lightControlAll=true;
      break;
    case ' ':
      if(isLight){
	if(lightControlAll){
	  lightEnableAll=!lightEnableAll;
	}else{
	  lightEnable[elementId]=!lightEnable[elementId];
	}
	initLight();
	glutPostRedisplay();
      }
      break;
    case '0':
    case '1':
    case '2':
      elementId=key-'0';
      if(isLight){
	lightControlAll=false;
      }
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
    case 't':      
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

void chargeTextureJpeg(char *fichier,int numtex)
{
//   unsigned char image[256*256*3];
//   unsigned char texture[256][256][3];
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;	
  unsigned char *ligne;
  int i,j;
  
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  if ((file=fopen(fichier,"rb"))==NULL)
  {
    fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
    exit(1);
  }
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  
  unsigned char image[cinfo.image_width*cinfo.image_height*3];
  unsigned char texture[cinfo.image_height][cinfo.image_width][3];
//   if ((cinfo.image_width!=256)||(cinfo.image_height!=256)) {
//     fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
//     exit(1);
//   }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }
  
  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
  {
    ligne=image+3*cinfo.image_width*cinfo.output_scanline;
    jpeg_read_scanlines(&cinfo,&ligne,1);
  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  
  for (i=0;i<cinfo.image_height;i++)
    for (j=0;j<cinfo.image_width;j++) {
      texture[i][j][0]=image[i*cinfo.image_width*3+j*3];
      texture[i][j][1]=image[i*cinfo.image_width*3+j*3+1];
      texture[i][j][2]=image[i*cinfo.image_width*3+j*3+2];
    }
  /* Parametrage du placage de textures */
  glBindTexture(GL_TEXTURE_2D,numtex);
  // select modulate to mix texture with color for shading
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  
//   // when texture area is small, bilinear filter the closest mipmap
//   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
// 		   GL_LINEAR_MIPMAP_NEAREST );
//   // when texture area is large, bilinear filter the first mipmap
//   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  bool wrap=true;
  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  
//   if (useMipmaps) {
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
//     gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256,GL_RGB, GL_UNSIGNED_BYTE, texture);
//   } else {
//     if (linearFiltering) {
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     } else {
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     }
//     glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0,GL_RGB, GL_UNSIGNED_BYTE, floorTexture);
//   }
//   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
//      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,cinfo.image_width,cinfo.image_height,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
  // build our texture mipmaps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, cinfo.image_width,cinfo.image_height, GL_RGB, GL_UNSIGNED_BYTE, texture );
}

/****************************************************/
/* void chargeTextureTiff(char *fichier,int numtex) */
/****************************************************/
/* chargement de l'image tif 'fichier' et placement */
/* dans la texture de numero 'numtex'               */
/****************************************************/

void chargeTextureTiff(char *fichier,int numtex)
{
  
  uint32 l, h;
  int i,j;
  size_t npixels;
  uint32* raster;
  
  /* chargement de l'image TIF */
  TIFF* tif = TIFFOpen(fichier, "r");
  if (tif) {
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &l);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h); 
    npixels = l * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32)); 
    unsigned char image[h][l][3];
    if (raster != NULL) {
      /* lecture de l'image */
      if (TIFFReadRGBAImage(tif, l, h, raster, 1)) {
	/* transfert de l'image vers le tableau 'image' */
	for (i=0;i<h;i++)
	  for (j=0;j<l;j++) {
	    image[i][j][0]=((unsigned char *)raster)[i*l*4+j*4+0];
	    image[i][j][1]=((unsigned char *)raster)[i*l*4+j*4+1];
	    image[i][j][2]=((unsigned char *)raster)[i*l*4+j*4+2];
	  }
      }
      else {
	printf("erreur de chargement du fichier %s\n",fichier);
	exit(0);
      }
      _TIFFfree(raster); 
    }
    TIFFClose(tif);
    
    /*      paramétrage de la texture */
    glBindTexture(GL_TEXTURE_2D,numtex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,
		 GL_RGB,GL_UNSIGNED_BYTE,image);
  }
  
}

/**************************************/
/* void chargeTextureProc(int numtex) */
/**************************************/
/* Création de la texture procedurale */
/* de numero 'numtex'                 */
/**************************************/

void chargeTextureProc(int numtex)
{
  unsigned char image[256][256][3];
  int i,j;
  int a;
  
  /* calcule de l'image */
  for (i=0;i<256;i++)
    for (j=0;j<256;j++) {
      a=fonctionTexture(i,j);
      image[i][j][0]=a;
      image[i][j][1]=128;
      image[i][j][2]=128;
    }
    
    /* Paramètrage de la texture */
    glBindTexture(GL_TEXTURE_2D,numtex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,
	       GL_RGB,GL_UNSIGNED_BYTE,image);
}



/*****************************************************/
/* int fonctionTexture(int x,int y)                  */
/*****************************************************/
/* Calcule et renvoie la valeur de la fonction       */
/* utilisee pour la texture procedurale au point x,y */
/*****************************************************/

int fonctionTexture(int x,int y)
{
  float dx=(128.0-(float)x)/255.0*40.0;
  float dy=(128.0-(float)y)/255.0*40.0;
  float a=cos(sqrt(dx*dx+dy*dy)+decalage);
  return (int)((a+1.0)/2.0*255);
  
}
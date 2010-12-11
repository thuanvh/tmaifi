#include <GL/glut.h>

void affichage();
void clavier(unsigned char touche,int x,int y);

int main(int argc,char **argv)
{
  
  /* initialisation de glut et creation
   *    de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(250,250);
  glutCreateWindow("ogl1");
  
  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);
  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  
  
  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

void affichage()
{
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* Dessin du polygone */
  glBegin(GL_POLYGON);
  glColor3f(1.0,0.0,0.0);
  glVertex2f(-0.5,-0.5);
  glColor3f(0.0,1.0,0.0);
  glVertex2f(0.5,-0.5);
  glColor3f(0.0,0.0,1.0);
  glVertex2f(0.5,0.5);
  glColor3f(1.0,1.0,1.0);
  glVertex2f(-0.5,0.5);
  glEnd();
  
  /* on force l'affichage du resultat */
  glFlush();
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
  {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
  }
}

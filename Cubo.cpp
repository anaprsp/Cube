#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
  
void display();
void comandos();
  
double rot_y=0; 
double rot_x=0;
 
bool aberto = false;
GLfloat ang;
GLfloat Angle;

void display(){
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glRotatef( rot_x, 1.0, 0.0, 0.0 );
  glRotatef( rot_y, 0.0, 1.0, 0.0 );
  
  //Lado rosa
 glBegin(GL_POLYGON);
    glColor3f( 1.0, 0.078, 0.576 );    
    glVertex3f(  0.5, -0.5, 0.5 );      
    glVertex3f(  0.5,  0.5, 0.5 );     
    glVertex3f( -0.5,  0.5, 0.5 );      
    glVertex3f( -0.5, -0.5, 0.5 );      
 glEnd();
  
    if (aberto) {
        glPushMatrix();
            glTranslatef( 0.5, -0.5, 0.5 );
            glRotatef(ang, 0.0, -1.0, 0.0);
            glBegin(GL_POLYGON);
                glColor3f( 1.0,  1.0,  0.0 ); // amarelo
                glVertex3f( 0.0, -0.0, -1.0 );
                glVertex3f( 0.0,  1.0, -1.0 );
                glVertex3f( 0.0,  1.0,  0.0 );
                glVertex3f( 0.0, 0.0,  0.0 );
            glEnd();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef( -0.5,  0.5, 0.5 );
            glRotatef(ang, 1.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
                glColor3f( 0.0,  0.0,  1.0 );  // azul
                glVertex3f( 1.0,  0.0,  0.0 );
                glVertex3f( 1.0,  0.0, -1.0 );
                glVertex3f( 0.0,  0.0, -1.0 );
                glVertex3f( 0.0,  0.0,  0.0 );
            glEnd();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef( 0.5, -0.5, 0.5 );
            glRotatef(ang, -1.0, 0.0, 0.0 );
            glBegin(GL_POLYGON);
                glColor3f( 1.0,  0.0,  0.0 );  // vermelho
                glVertex3f( 0.0, 0.0, -1.0 );
                glVertex3f( 0.0, 0.0,  0.0 );
                glVertex3f( -1.0, 0.0, 0.0 );
                glVertex3f( -1.0, 0.0, -1.0 );
            glEnd();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef(-0.5, -0.5, 0.5);
            glRotatef(ang, 0.0, 1.0, 0.0); 
            glBegin(GL_POLYGON);
                glColor3f( 0.0,  1.0,  0.0 ); //verde
                glVertex3f( 0.0, 0.0, 0.0 );
                glVertex3f( 0.0,  1.0,  0.0 );
                glVertex3f( 0.0,  1.0, -1.0 );
                glVertex3f( 0.0, 0.0, -1.0 );
            glEnd();
            glTranslatef(0.0, 0.0, -1.0); //-0.5, -0.5, -0.5
            glRotatef(ang, 0.0, 1.0, 0.0);
            glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.647, 0.0 ); //laranja
                glVertex3f( 1.0, 0.0, 0.0 );
                glVertex3f( 1.0,  1.0, 0.0 );
                glVertex3f( 0.0,  1.0, 0.0 );
                glVertex3f( 0.0, 0.0, 0.0 );
            glEnd();
        glPopMatrix();
        
        if (ang<90) {
            ang+=5.0;
        } 
    } 
	else {
    // Lado laranja 
      glBegin(GL_POLYGON);
      glColor3f( 1.0, 0.647, 0.0 );
      glVertex3f( 0.5, -0.5, -0.5 );
      glVertex3f( 0.5,  0.5, -0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glEnd();
      
      // Lado amarelo 
     glBegin(GL_POLYGON);
      glColor3f(  1.0,  1.0,  0.0 );
      glVertex3f( 0.5, -0.5, -0.5 );
      glVertex3f( 0.5,  0.5, -0.5 );
      glVertex3f( 0.5,  0.5,  0.5 );
      glVertex3f( 0.5, -0.5,  0.5 );
      glEnd();
      
      // Lado verde
     glBegin(GL_POLYGON);
      glColor3f(  0.0,  1.0,  0.0 );
      glVertex3f( -0.5, -0.5, 0.5 );
      glVertex3f( -0.5,  0.5,  0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glEnd();
      
      // Lado azul
     glBegin(GL_POLYGON);
      glColor3f(   0.0,  0.0,  1.0 );
      glVertex3f(  0.5,  0.5,  0.5 );
      glVertex3f(  0.5,  0.5, -0.5 );
      glVertex3f( -0.5,  0.5, -0.5 );
      glVertex3f( -0.5,  0.5,  0.5 );
      glEnd();
      
      // Lado vermelho 
     glBegin(GL_POLYGON);
      glColor3f(   1.0,  0.0,  0.0 );
      glVertex3f(  0.5, -0.5, -0.5 );
      glVertex3f(  0.5, -0.5,  0.5 );
      glVertex3f( -0.5, -0.5, 0.5 );
      glVertex3f( -0.5, -0.5, -0.5 );
      glEnd();
    }
  glFlush();
  glutSwapBuffers();
}

void comandos( int key, int x, int y ) {
  if (key == GLUT_KEY_RIGHT)
    rot_y += 10;
  
  else if (key == GLUT_KEY_LEFT)
    rot_y -= 10;
  
  else if (key == GLUT_KEY_UP)
    rot_x += 10;
  
  else if (key == GLUT_KEY_DOWN)
    rot_x -= 10;
  glutPostRedisplay(); 
}

void myTimer(int id) {
    glutPostRedisplay();
    glutTimerFunc(50, myTimer, 0);
}
   
void myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-3, 3, -3, 3);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void myMouse(int b, int s, int x, int y) {
    if (b==GLUT_LEFT_BUTTON) {
        if (s==GLUT_UP) {
            aberto = not aberto;
            ang=Angle/30.0;
            glutPostRedisplay();
        }
    }
}

int main(int argc, char* argv[]){
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500,500);
  glutCreateWindow("Cubo Coloridinho");
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutSpecialFunc(comandos);
  glutMouseFunc(myMouse);
  glutReshapeFunc(myReshape);
  glutTimerFunc(50, myTimer, 0);
  glutMainLoop();
  return 0;
}


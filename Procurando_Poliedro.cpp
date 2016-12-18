//Trabalho Final da disciplina Computação Gráfica - EEL882 
//Professor Cláudio Esperança
//UFRJ

//O código foi implementado para somente 2 tipos de Poliedro, o Hexaedro(Cubo) e o Dodecaedro. Para conferir as opções de Poliedros, Texturas 
//e o Editor de Textura, basta clicar com o botão direito do mouse em qualquer parte do escopo da janela que aparecerá o Menu!!
//Para rotacionar o poliedro, aberto ou fechado, basta usar as setas do teclado.

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#define FORMAT 0x80E0
#include <GL/GLAux.h>
#include "anemona.h"


int width;
int height;
int dx, dy = 0;
int NumFaces=6;
int sourceFace=0;
int type=Cubo;
int min_x, min_y = 3;
int max_x, max_y = -3;

double rot_x=0;
double rot_y=0; 

bool open=false; 
bool tipo=false;
bool showEditor=false;
bool updateTexture = true;
bool moving = false;
char * imageFile = (char *)"Nemo.bmp";
GLfloat pAngle;
GLfloat angle=0;
vector<vector<vector<GLfloat> > > facesModelview(NumFaces);
vector<vector<vector<GLfloat> > > facesProjection(NumFaces);
vector<pair<int, int> > movingVertices;
vector<vector<GLfloat> > texCoordMatrix;
vector<vector<pair<GLfloat, GLfloat> > > texCoord;
unsigned int imageWidth, imageHeight;
GLuint textureID;
unsigned char *myPixelArray; 



//////////////////////////////////////////////////////////////////////////////
void LoadBMP(char *Filename){
 	// Data read from the header of the BMP file
 	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
 	unsigned int dataPos;     // Position in the file where the actual data begins
 	unsigned int imageSize;   // = width*height*3
 	// Actual RGB data
 	unsigned char * data;
 	
 	FILE * file = fopen(Filename,"rb");
 	if (!file){printf("Image could not be opened\n"); return;}
 	if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
 	    printf("Not a correct BMP file\n");
 	    return;
  	}
  	if ( header[0]!='B' || header[1]!='M' ){
     printf("Not a correct BMP file\n");
     return;
  	}
    // Read ints from the byte array
 	dataPos    = *(int*)&(header[0x0A]);
 	imageSize  = *(int*)&(header[0x22]);
 	imageWidth      = *(int*)&(header[0x12]);
 	imageHeight     = *(int*)&(header[0x16]);
 	
 	// Some BMP files are misformatted, guess missing information
 	if (imageSize==0)    imageSize=imageWidth*imageHeight*4; // 3 : one byte for each Red, Green and Blue component
 	if (dataPos==0)      dataPos=54; // The BMP header is done that way
 	// Create a buffer
 	myPixelArray = new unsigned char [imageSize];
 	
 	// Read the actual data from the file into the buffer
 	fread(myPixelArray,1,imageSize,file);
 	
 	//Everything is in memory now, the file can be closed
 	fclose(file);
 	for (int i=0; i<imageSize/4; i++) {
 		unsigned char b = myPixelArray[i*4];
 		unsigned char g = myPixelArray[1+i*4];
 		unsigned char r = myPixelArray[2+i*4];
 		myPixelArray[i*4] = r;
 		myPixelArray[1+i*4] = g;
 		myPixelArray[2+i*4] = b;
 	}
 			
}
/////////////////////////////////////////////////////////////////////////////  	
vector<vector<GLfloat> > getModelview() {
	vector<vector<GLfloat> > modelview(4);
	GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	for (int j=0; j<4; j++) {
		modelview[j].resize(4);
		modelview[j][0]=mv[0+j*4];
		modelview[j][1]=mv[1+j*4];
		modelview[j][2]=mv[2+j*4];
		modelview[j][3]=mv[3+j*4];
	}
	modelview = transpose(modelview);
	return modelview;
}
//////////////////////////////////////////////////////////////////////////////
vector<vector<GLfloat> > getProjection() {
	vector<vector<GLfloat> > projection(4);
	GLfloat pj[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pj);
	for (int j=0; j<4; j++) {
		projection[j].resize(4);
		projection[j][0]=pj[0+j*4];
		projection[j][1]=pj[1+j*4];
		projection[j][2]=pj[2+j*4];
		projection[j][3]=pj[3+j*4];
	}
	projection=transpose(projection);
	return projection;
}
//////////////////////////////////////////////////////////////////////////////
vector<vector<GLfloat> > updateTexCoord(int i, Poliedro ph) {
	vector<vector<GLfloat> > vertices=ph.vertices;
	vector<vector<int> > faces=ph.faces;
	vector<GLfloat> u=sub(vertices[faces[i][1]],vertices[faces[i][0]]);
	vector<GLfloat> v=sub(vertices[faces[i][2]],vertices[faces[i][1]]);
	vector<GLfloat> n=normalize(crossProduct(u,v));
	GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	vector<vector<GLfloat> > modelview(4);
	for (int j=0; j<4; j++) {
		modelview[j].resize(4);
		modelview[j][0]=mv[0+j*4];
		modelview[j][1]=mv[1+j*4];
		modelview[j][2]=mv[2+j*4];
		modelview[j][3]=mv[3+j*4];
	}
	modelview=transpose(modelview);
	vector<vector<GLfloat> > newn=normalize(multMatrix(modelview,coToVe(n,0)));
	vector<vector<GLfloat> > v1=multMatrix(modelview, coToVe(u,0));
	vector<vector<GLfloat> > nv=coToVe(crossProduct(veToCo(newn),veToCo(v1)),0);
	vector<vector<GLfloat> > p=multMatrix(modelview,coToVe(vertices[faces[i][0]],1));
	vector<vector<GLfloat> > marray[]={v1, nv, newn, p};
	vector<vector<GLfloat> > m(4);
	for (int k=0; k<4; k++) {
		m[k].resize(4);
		for (int l=0; l<4; l++) {
			m[k][l]=marray[l][k][0];
		}
	}
	m=inverse(m);
	return m;
}
//////////////////////////////////////////////////////////////////////////////
void updateFaceTexCoord(int i, Poliedro p) {
	vector<vector<GLfloat> > vertices=p.vertices;
	vector<int> face=p.faces[i];
	vector<vector<GLfloat> > modelview(4);
	GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	for (int j=0; j<4; j++) {
		modelview[j].resize(4);
		modelview[j][0]=mv[0+j*4];
		modelview[j][1]=mv[1+j*4];
		modelview[j][2]=mv[2+j*4];
		modelview[j][3]=mv[3+j*4];
	}
	modelview=transpose(modelview);
	for (int j=0; j<face.size(); j++) {
		vector<vector<GLfloat> > tc = multMatrix(texCoordMatrix, multMatrix(modelview, coToVe(vertices[face[j]],1)));
		texCoord[i][j].first=tc[0][0];
		texCoord[i][j].second=tc[1][0];
		max_x = ceil(max(texCoord[i][j].first, (float)max_x));
		max_y = ceil(max(texCoord[i][j].second, (float)max_y));
		min_x = floor(min(texCoord[i][j].first, (float)min_x));
		min_y = floor(min(texCoord[i][j].second, (float)min_y));
	}
}
//////////////////////////////////////////////////////////////////////////////
void initializeTexCoord() {
	texCoord.resize(NumFaces);
	switch (NumFaces) {
		case 6:
			{ for (int i=0; i<texCoord.size(); i++) {
					texCoord[i].resize(4);
				}
				break;
		case 12:
			{ for (int i=0; i<texCoord.size(); i++) {
					texCoord[i].resize(5);
				} break;
			}
		default:
			break;
	    }
    }
}
//////////////////////////////////////////////////////////////////////////////
void drawPolyhedron() {
	Poliedro polyhedron(type);
	pAngle=polyhedron.angulo;
	polyhedron.bfs(sourceFace);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  	glLoadIdentity();
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
    glOrtho(-3,3,-3,3,-3,3);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
  	glRotatef( rot_x, 1.0, 0.0, 0.0 );
 	glRotatef( rot_y, 0.0, 1.0, 0.0 );
	vector<vector<pair<vector<GLfloat>, vector<GLfloat> > > > transformations=polyhedron.Transf();
	if (updateTexture) {
		max_x = 3;
		max_y =  3;
		min_x = -3;
		min_y = -3;
		texCoordMatrix=updateTexCoord(sourceFace, polyhedron);
		updateFaceTexCoord(sourceFace, polyhedron);
	}
	facesModelview[sourceFace]=getModelview();
	facesProjection[sourceFace]=getProjection();
	polyhedron.DesenhaFace(sourceFace, texCoord[sourceFace]);
	for (int i=0; i<NumFaces; i++) {
		if (i==sourceFace) {
			continue;
		}
		if (updateTexture) {
			glPushMatrix();
				for (int j=transformations[i].size()-1; j>=0; j--) {
					vector<GLfloat> vect=transformations[i][j].first;
					glTranslatef(vect[0], vect[1], vect[2]);
					vector<GLfloat> vecr=transformations[i][j].second;
					glRotated(pAngle, vecr[0], vecr[1], vecr[2]);
					glTranslatef(-vect[0], -vect[1], -vect[2]);
				}
				updateFaceTexCoord(i, polyhedron);
			glPopMatrix();
		}
		glPushMatrix();
			for (int j=transformations[i].size()-1; j>=0; j--) {
				vector<GLfloat> vect=transformations[i][j].first;
				glTranslatef(vect[0], vect[1], vect[2]);
				vector<GLfloat> vecr=transformations[i][j].second;
				glRotated(angle, vecr[0], vecr[1], vecr[2]);
				glTranslatef(-vect[0], -vect[1], -vect[2]);
			}
			facesModelview[i]=getModelview();
			facesProjection[i]=getProjection();
			polyhedron.DesenhaFace(i, texCoord[i]);
		glPopMatrix();
	}
	if (updateTexture) {
		updateTexture = false;
		max_x = max(max(max_x, max_y), 3);
		max_y = max(max(max_x, max_y), 3);
		min_x = min(min(min_x, min_y),-3);
		min_y = min(min(min_x, min_y),-3);
		
		max_x = max(abs(min_x),max_x);
		max_y = max_x;
		min_x = -max_x;
		min_y = -max_y;
	}
	if (angle<pAngle && open) {
		angle+=pAngle/30.0f;
		if (angle>=pAngle) {
			angle=pAngle;
		}
	}
	if (angle>0 && !open) {
		angle-=pAngle/30.0f;
		if (angle<0) {
			angle=0;
		}
	}
	glPopMatrix();
}
//////////////////////////////////////////////////////////////////////////////
void drawEditor() {
	glClearColor(0,0,0,1);	
	glDisable(GL_LIGHTING);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();
	glLoadIdentity();
	glScalef(3/(float)max_x, 3/(float)max_y, 1.0f);
	for (int i=0; i<2*max_x; i++) {
		for (int j=0; j<2*max_y; j++) {
			glBegin(GL_POLYGON);
				glTexCoord2d(0.0f, 0.0f); glVertex3f(i-max_x,j-max_x,-1);
				glTexCoord2d(1.0f, 0.0f); glVertex3f(i-max_x+1,j-max_x,-1);
				glTexCoord2d(1.0f, 1.0f); glVertex3f(i-max_x+1,j-max_x+1,-1);
				glTexCoord2d(0.0f, 1.0f); glVertex3f(i-max_x,j-max_x+1,-1);	
			glEnd();
		}
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.078f, 0.576f);
	glLineWidth(3);
	for (int i=0; i<NumFaces; i++) {
		glBegin(GL_LINE_LOOP);
			for (int j=0; j<texCoord[i].size(); j++) {
				glVertex2f(texCoord[i][j].first, texCoord[i][j].second);
			}
		glEnd();	
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}
//////////////////////////////////////////////////////////////////////////////
void display(){
	int x, y, w, h;
	if (showEditor) {
		if (width>=2*height) {
			dx = floor((width-2*height)/2.0);
			dy = 0;
			h=height;
		} else {
			dx = 0;
			dy = floor((height-width/2)/2.0);
			w=width;
			h=w/2;
		}
		glViewport(dx+h,dy,h,h);
		drawPolyhedron();
		glViewport(dx,dy,h, h);
		drawEditor(); 
	} else {
		glViewport(max((width-height)/2,0), max(0, (height-width)/2), min(width,height), min(width, height));
		drawPolyhedron();
	}
	    glFlush();
    glutSwapBuffers();
}
//////////////////////////////////////////////////////////////////////////////
void myTimer(int id) {
	glutPostRedisplay();
	glutTimerFunc(25, myTimer, 0);
}
//////////////////////////////////////////////////////////////////////////////
void myReshape(int w, int h) {
	height=h;
	width=w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-3.0, 3.0, -3.0, 3.0);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}
////////////////////////////////////////////////////////////////////////////// 
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
//////////////////////////////////////////////////////////////////////////////
void initializeTexture() {
	LoadBMP(imageFile);
 	GLuint textureID; 
 	glGenTextures(1, &textureID); 
 	glBindTexture(GL_TEXTURE_2D, textureID); 
 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, myPixelArray);
 	glEnable(GL_TEXTURE_2D); 
 	glBindTexture(GL_TEXTURE_2D, textureID); 
 	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLfloat tcArray[][4] = { {1.0f, 0.0f, 0.0f, 0.5f},{0.0f, 1.0f, 0.0f, 0.5f},{0.0f, 0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 0.0f, 1.0f}};
	texCoordMatrix.resize(4);
	for (int i=0; i<4; i++) {
		texCoordMatrix[i].resize(4);
		for (int j=0; j<4; j++) {
			texCoordMatrix[i][j]=tcArray[i][j];
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void polyhedronMenu(int option) {
	switch (option) {
		case 1:
			type = Cubo;
			NumFaces = 6;
			break;
		case 3:
			type = Dodecaedro;
			NumFaces = 12;
			break;
	}
	sourceFace = 0;
	facesModelview.resize(NumFaces);
	facesProjection.resize(NumFaces);
	open=false;
	angle=0.0f;
	updateTexture = true;
	initializeTexCoord();
	glutPostRedisplay();
}
//////////////////////////////////////////////////////////////////////////////
void textureMenu(int option) {
	switch(option) {
		case 0:
			imageFile=(char *)"Nemo.bmp";
			break;
		case 1:
			imageFile=(char *)"Dory.bmp";
			break;
	}
	initializeTexture();
	glutPostRedisplay();
}
//////////////////////////////////////////////////////////////////////////////
void mainMenu(int option){
	if (!option && !showEditor) {
		showEditor=true;
	} else if(!option && showEditor) {
		showEditor=false;
	}
	glutPostRedisplay();
}
//Estrutura do Menu de seleção/////////////////////////////////////////////////
void createMenu() {
	int menu, submenu1, submenu2;
	
	submenu1 = glutCreateMenu(polyhedronMenu);
	glutAddMenuEntry("Cubo",1);
	glutAddMenuEntry("Dodecaedro",3);
	
	submenu2 = glutCreateMenu(textureMenu);
	glutAddMenuEntry("Nemo", 0);
	glutAddMenuEntry("Dory", 1);
	
	menu = glutCreateMenu(mainMenu);
	glutAddSubMenu("Poliedro",submenu1);
	glutAddSubMenu("Textura", submenu2);
	glutAddMenuEntry("Mostrar/Esconder editor", 0);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//////////////////////////////////////////////////////////////////////////////
vector<pair<int, int> > getVertex(int x, int y) {
	vector<pair<int, int> > ret;
	for (int i=0; i<texCoord.size(); i++) {
		for (int j=0; j<texCoord[i].size(); j++) {
			int vx=(texCoord[i][j].first*(3.0/max_x)+3)*height/6.0+dx;
			int vy=(-texCoord[i][j].second*(3.0/max_y)+3)*height/6.0+dy;
			if (vx<=x+5 && vx>=x-5 && vy<=y+5 && vy>=y-5) {
				ret.push_back(make_pair(i, j));
			}
		}
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////////
pair<GLfloat, GLfloat> windowCoordinates(vector<GLfloat> c, int face) {
	int h=min(height, width/2);
	vector<vector<GLfloat> > modelview=facesModelview[face];
	vector<vector<GLfloat> > projection=facesProjection[face];
	vector<vector<GLfloat> > vertex=coToVe(c, 1);
	vertex=multMatrix(modelview, vertex);
	vertex=multMatrix(projection, vertex);
	for (int i=0; i<4; i++) {
		vertex[i][0]/=vertex[3][0];
	}
	pair<GLfloat, GLfloat> ret=make_pair(vertex[0][0], vertex[1][0]);
	ret.first=(showEditor ? dx+h+(ret.first+1)*height/2.0 : max((width-height)/2,0)+(ret.first+1)*min(width,height)/2.0);
	ret.second=(showEditor ? height-(dy+(ret.second+1)*height/2.0) : height-(max(0, (height-width)/2)+(ret.second+1)*min(width, height)/2.0));
	return ret;
}
//////////////////////////////////////////////////////////////////////////////
int index(int x, int y) {
	Poliedro p(type);
	vector<vector<GLfloat> > vertices=p.vertices;
	vector<vector<int> > faces=p.faces;
	pair<GLfloat, GLfloat> wc;
	for (int i=0; i<NumFaces; i++) {
		int sum=0;
		for (int j=0; j<p.faces[i].size(); j++) {
			int next=(j==faces[i].size()-1 ? 0 : j+1);
			wc=windowCoordinates (vertices[faces[i][j]], i);
			vector<GLfloat> vec3;
			vec3.push_back(wc.first);
			vec3.push_back(wc.second);
			vec3.push_back(0);
			vector<vector<GLfloat> > p = coToVe(vec3,1);
			wc=windowCoordinates (vertices[faces[i][next]], i);
			vector<GLfloat> vec1,vec2;
			vec1.push_back(wc.first);
			vec1.push_back(wc.second);
			vec1.push_back(0);
			vec2.push_back(x);
			vec2.push_back(y);
			vec2.push_back(0);
			vector<vector<GLfloat> > q = coToVe(vec1,1);
			vector<vector<GLfloat> > r = coToVe(vec2,1);
			sum+=Orientation3p(p,q,r);
		}
		if (sum==-faces[i].size()) {
			return i;
		}
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////////
void myMouse(int b, int s, int x, int y) {
	if (b==GLUT_LEFT_BUTTON) {
		if (!showEditor || (x>height)) {
			if (s==GLUT_UP) {
				if (open) {
					open=false;
					glutPostRedisplay();
				} else {
					int i=index(x,y);
					open=(i>=0 && angle==0);
					updateTexture = open;
					sourceFace=(i>=0 && angle==0 ? i : sourceFace);
					glutPostRedisplay();
				}
			}
		}
		else {
			vector<pair<int, int> > v=getVertex(x, y);
			if (v.size()>0 && s==GLUT_DOWN) {
				movingVertices=v;
			} else {
				movingVertices.clear();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void myMotion(int x, int y) {
	for (int i=0; i<movingVertices.size(); i++) {
		cout << "OK" << endl;
		GLfloat new_x=(max_x/3.0)*(((x-dx)*6)/(float)height-3);
		GLfloat new_y=-(max_y/3.0)*(((y-dy)*6)/(float)height-3);
		cout << "Vertice " << new_x << " " << new_y << endl;
	    cout << max_x << " " << min_x << " " << max_y << " " << min_y << endl;
		texCoord[movingVertices[i].first][movingVertices[i].second].first=(max(min(new_x, (float)max_x), (float)min_x));
		texCoord[movingVertices[i].first][movingVertices[i].second].second=(max(min(new_y, (float)max_y), (float)min_y));
		glutPostRedisplay();
	}
}
//////////////////////////////////////////////////////////////////////////////
void myKeyboard(unsigned char key, int x, int y ) {
	if (key=='h'|| key=='H') {
		type=Cubo;
		open=false;
		NumFaces=6;
		angle=0.0f;
		sourceFace = 0;
		updateTexture = true;
	} else if (key=='d' || key=='D') {
		type=Dodecaedro;
		open=false;
		NumFaces=12;
		angle=0.0f;
		sourceFace = 0;
		updateTexture = true;
	} 
	initializeTexCoord();
	facesModelview.resize(NumFaces);
	facesProjection.resize(NumFaces);
	glutPostRedisplay();
}
//////////////////////////////////////////////////////////////////////////////
void initializations() {
	glClearColor(0.0, 0.0, 0.0, 1.0); 
 	glEnable(GL_NORMALIZE); 
 	glShadeModel(GL_SMOOTH); 
 	glEnable(GL_LIGHTING);
	GLfloat ambientIntensity[4] = {0.8, 0.8, 0.8, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientIntensity);
	GLfloat lt0Intensity[4] = {1.5, 1.5, 1.5, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt0Intensity);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt0Intensity);
	GLfloat lt0Position[4] = {2.0, 4.0, 5.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lt0Position);
	glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1);
	glEnable(GL_LIGHT0);
	initializeTexture();
	initializeTexCoord();
	createMenu();
}
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1080,540);
  glutCreateWindow("Procurando Poliedros");
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutSpecialFunc(comandos);
  glutMouseFunc(myMouse);
  glutMotionFunc(myMotion);
  glutReshapeFunc(myReshape);
  glutTimerFunc(50, myTimer, 0);
  glutKeyboardFunc(myKeyboard);
  initializations();
  glutMainLoop();
  return 0;
}


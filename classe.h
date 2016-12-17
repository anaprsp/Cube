#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>
#include <cmath>
#define HEXAHEDRON 2
#define DODECAHEDRON 4

using namespace std;

vector<GLfloat> sub( vector<GLfloat> a, vector<GLfloat> b) {
    	vector<GLfloat> ret;
    	ret[0]=a[0]-b[0];
    	ret[1]=a[1]-b[1];
    	ret[2]=a[2]-b[2];
    	return ret;
}
vector<GLfloat> crossProduct( vector<GLfloat> u, vector<GLfloat> v) {
	vector<GLfloat> ret;
	ret[0] = u[1]*v[2] - u[2]*v[1];
	ret[1] = u[2]*v[0] - u[0]*v[2];
	ret[2] = u[0]*v[1] - u[1]*v[0];
	return ret;
}
vector<GLfloat> normalize( vector<GLfloat> v) {
	vector<GLfloat> ret;
	GLfloat mod=sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
	ret[0]=v[0]/mod;
	ret[1]=v[1]/mod;
	ret[2]=v[2]/mod;
	return ret;
}
vector<vector<GLfloat> > normalize(vector<vector<GLfloat> > v) {
	vector<vector<GLfloat> > ret=v;
	GLfloat mod=sqrt(pow(v[0][0],2) + pow(v[1][0],2) + pow(v[2][0],2));
	ret[0][0]/=mod;
	ret[1][0]/=mod;
	ret[2][0]/=mod;
	return ret;
}
vector<vector<GLfloat> > transpose(vector<vector<GLfloat> > m) {
	vector<vector<GLfloat> > ret(m[0].size());
	for (int i=0; i<ret.size(); i++) {
		ret[i].resize(m.size());
		for (int j=0; j<m.size(); j++) {
			ret[i][j]=m[j][i];
		}
	}
	return ret;
}
GLfloat determinant(vector<vector<GLfloat> > m) {
	if (m.size()==1) {
		return m[0][0];
	}
	GLfloat sum=0;
	for (int i=0; i<m.size(); i++) {
		vector<vector<GLfloat> > m2=m;
		m2.erase(m2.begin()+i);
		for (int j=0; j<m2.size(); j++) {
			m2[j].erase(m2[j].begin());
		}
		sum+=m[i][0]*pow(-1,i+2)*determinant(m2);
	}
	return sum;
}
int Orientation3p(vector<vector<GLfloat> > p, vector<vector<GLfloat> > q, vector<vector<GLfloat> > r) {
//	cout << "OK" << endl;
	vector<vector<GLfloat> > a[3]={p, q, r};
	vector<vector<GLfloat> > matrix(3);
	for (int i=0; i<3; i++) {
		matrix[i].resize(3);
		matrix[0][i]=1;
		if (i==0) {
			continue;
		}
	//	cout << "OK" << endl;
		for (int j=0; j<3; j++) {
			matrix[i][j]=a[j][i-1][0];
		}

	}
//	cout << "OK" << endl;
	int det=determinant(matrix);
	if (det>0) {
		return 1;
	} else if (det<0) {
		return -1;
	}
	return 0;	
}
vector<vector<GLfloat> > coToVe(vector<GLfloat> v, int w) {
	vector<vector<GLfloat> > ret(4);
	ret[0].push_back(v[0]);
	ret[1].push_back(v[1]);
	ret[2].push_back(v[2]);
	ret[3].push_back(w);
	return ret;
}
vector<GLfloat> veToCo(vector<vector<GLfloat> > v) {
	vector<GLfloat> ret;
	ret[0]=v[0][0];
	ret[1]=v[1][0];
	ret[2]=v[2][0];
	return ret;
}

vector<vector<GLfloat> > multMatrix(vector<vector<GLfloat> > a, vector<vector<GLfloat> > b) {
	if (a[0].size()!=b.size()) {
		cout << "Dimensao incorreta da matriz" << endl;
	}
	vector<vector<GLfloat> > ret(a.size());
	for (int i=0; i<a.size(); i++) {
		ret[i].resize(b[0].size());
		for(int j=0; j<b[0].size(); j++) {
			GLfloat sum=0;
			for (int k=0; k<b.size(); k++) {
				sum+=a[i][k]*b[k][j];
			}
			ret[i][j]=sum;
		}
	}
	return ret;
}
vector<vector<GLfloat> > inverse(vector<vector<GLfloat> > m) {
	vector<vector<GLfloat> > c(m.size());
	for (int i=0; i<c.size(); i++) {
		c[i].resize(m.size());
		for (int j=0; j<c.size(); j++) {
			vector<vector<GLfloat> > m2=m;
			m2.erase(m2.begin()+i);
			for (int k=0; k<m2.size(); k++) {
				m2[k].erase(m2[k].begin()+j);
			}
			c[i][j]=determinant(m2)*pow(-1,i+j+2);
		}
		
	}
	vector<vector<GLfloat> > ret;
	ret=transpose(c);
	GLfloat det=determinant(m)
	;for (int i=0; i<ret.size(); i++) {
		for (int j=0; j<ret.size(); j++) {
			ret[i][j]/=det;
		}
	}
	return ret;
}
class Poliedro{
	public:
	vector <vector <int> > faces;
	vector <vector<GLfloat> > vertices;
	GLfloat angulo;
	vector<vector<int> > arestas;
	vector<vector<int> > spanningTree;
	
	int isInVector(int v1, int v2, vector<vector<int> > v) {
		for (int i=0; i<v.size(); i++) {
			if ((v1==v[i][0] && v2==v[i][1]) || (v2==v[i][0] && v1==v[i][1])) {
				return i;
			} 
		}
		return -1;
	}
	bool isInVector(int v1, vector<int> v) {
		for (int i=0; i<v.size(); i++) {
			if (v1==v[i]) {
				return true;
			} 
		}
		return false;
	}
	void generateArestas() {
		for (int i=0; i<faces.size(); i++) {
			for (int j=0; j<faces[i].size(); j++) {
				if (j==faces[i].size()-1){
					int test=isInVector(faces[i][j],faces[i][0], arestas);
					if (test==-1){
						int e[]={faces[i][j],faces[i][0],i,-1};
						vector<int> aresta(e,e+sizeof(e)/sizeof(int));
						arestas.push_back(aresta);
					} else {
						arestas[test][3]=i;
					}
				} else {
					int test=isInVector(faces[i][j], faces[i][j+1], arestas);
					if (test==-1) {
						int e[]={faces[i][j],faces[i][j+1],i,-1};
						vector<int> aresta(e,e+sizeof(e)/sizeof(int));
						arestas.push_back(aresta);
					} else {
						arestas[test][3]=i;
					}
				}
			}
		}
	}
	Poliedro(bool tipo){
		if (tipo){
			angulo = 90;
				GLfloat v[][3]={{0.5, -0.5, 0.5},{0.5, 0.5, 0.5},{-0.5, -0.5, 0.5},{-0.5, -0.5, 0.5},{0.5, -0.5, -0.5},{0.5, 0.5, -0.5},{-0.5, 0.5, -0.5},{-0.5, -0.5, -0.5}};
				
				int f[][4]={{0,1,2,3},
				   {7,6,5,4},
				   {4,5,1,0},
				   {3,2,6,7},
				   {1,5,6,2},
				   {4,0,3,7}};
				for (int i=0; i<6; i++) {
					vector<int> face(f[i], f[i]+sizeof(f[i])/sizeof(int));
					faces.push_back(face);
				}
				generateArestas();
		}
		else{
			angulo = 180-acos(-1.0/sqrt(5))*180/M_PI;
			GLfloat t=(1.0 + sqrt(5.0)) / 2.0; 
			GLfloat r=1.0/t;
			GLfloat v[][3]={{- 0.5, - 0.5, - 0.5},{- 0.5, - 0.5,  0.5},{- 0.5,  0.5, - 0.5},{- 0.5,  0.5,  0.5},{0.5, - 0.5, - 0.5},{0.5, - 0.5,  0.5},{0.5,  0.5, - 0.5},{0.5,  0.5,  0.5},{0, - r/2, - t/2},{0, - r/2,  t/2},{0,  r/2, - t/2},{0,  r/2,  t/2},{- r/2, - t/2,  0},{- r/2,  t/2,  0},{r/2, - t/2,  0 },{r/2,  t/2,  0},{- t/2,  0, - r/2},{},{}};
			
			int f[][5]={{3, 11,  7, 15, 13},
				{7, 19, 17, 6, 15},
				{17,  4,  8, 10, 6},
				{8,  0, 16,  2, 10},
				{0, 12,  1, 18, 16},
				{6, 10,  2, 13, 15},
				{2, 16, 18, 3, 13},
				{18,  1, 9, 11,  3},
				{4, 14, 12, 0, 8},
				{11, 9, 5, 19, 7},
				{19, 5, 14, 4, 17},
				{1, 12, 14, 5,  9}};
			
			for (int i=0; i<12; i++) {
				vector<int> face(f[i], f[i]+sizeof(f[i])/sizeof(int));
				faces.push_back(face);
			}
			generateArestas();
		}
	}
	
	void drawFace(int i, vector<pair<GLfloat, GLfloat> > tc) {
		GLfloat color[] = {0.0, 0.0, 1.0, 1.0};
		GLfloat white[]  ={1.0, 1.0, 1.0, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
		vector<GLfloat> u=sub(vertices[faces[i][1]],vertices[faces[i][0]]);
		vector<GLfloat> v=sub(vertices[faces[i][2]],vertices[faces[i][1]]);
		vector<GLfloat> n=normalize(crossProduct(u,v));
		glBegin(GL_POLYGON);
			for (int j=0; j<faces[i].size(); j++) {
				GLdouble s = tc[j].first;
				GLdouble t = tc[j].second;
				glNormal3f(n[0], n[1], n[2]);
				glTexCoord2d(s,t);
				glVertex3f(vertices[faces[i][j]][0], vertices[faces[i][j]][1], vertices[faces[i][j]][2]);
			}
		glEnd();
	}
	void bfs(int sourceFace) {
		vector<bool> marking(faces.size());
		vector<vector<int> > adjacencyVector(faces.size());
		spanningTree.resize(faces.size());
		for (int i=0; i<faces.size(); i++){
			marking[i]=0;
			spanningTree[i].clear();
		}
		queue<int> vqueue;
		marking[sourceFace]=1;
		vqueue.push(sourceFace);
		for (int i=0; i<arestas.size(); i++) {
			if (!isInVector(arestas[i][2], adjacencyVector[arestas[i][3]])) {
				adjacencyVector[arestas[i][3]].push_back(arestas[i][2]);
			}
			if (!isInVector(arestas[i][3], adjacencyVector[arestas[i][2]])) {
				adjacencyVector[arestas[i][2]].push_back(arestas[i][3]);
			}
		}
		while(!vqueue.empty()){
			int v=vqueue.front();
			vqueue.pop();
			for (int i=0; i<adjacencyVector[v].size(); i++){
				int x=adjacencyVector[v][i];
				if (marking[x]==0){
					marking[x]=1;
					spanningTree[x].clear();
					spanningTree[x].insert(spanningTree[x].begin(),spanningTree[v].begin(), spanningTree[v].end());
					spanningTree[x].push_back(v);
					vqueue.push(x);
				}
			}
		}
		for (int i=0; i<spanningTree.size(); i++) {
			reverse(spanningTree[i].begin(), spanningTree[i].end());
		}
	}
	
	pair<vector<GLfloat>, vector<GLfloat> > getAresta(int face1, int face2) {
		int i=arestaPosition(face1, face2, arestas);
		for (int j=0; j<faces[face1].size(); j++) {
			if (faces[face1][j]==arestas[i][0]) {
				if (previous(faces[face1], j)==arestas[i][1]) {
					return make_pair(vertices[arestas[i][0]], vertices[arestas[i][1]]);
				} else {
					return make_pair(vertices[arestas[i][1]], vertices[arestas[i][0]]);
				}
			}
		}
	}
	
	int arestaPosition(int f1, int f2, vector<vector<int> > v) {
		for (int i=0; i<v.size(); i++) {
			if ((f1==v[i][2] && f2==v[i][3]) || (f2==v[i][2] && f1==v[i][3])) {
				return i;
			} 
		}
		return -1;
	}
	
	int previous(vector<int> v, int i) {
		if (i==0) {
			return v.back();
		} else {
			return v[i-1];
		}
	}
	
	vector<vector<pair<vector<GLfloat>, vector<GLfloat> > > > Transf() {
		vector<vector<pair<vector<GLfloat>, vector<GLfloat> > > > ret(faces.size());
		int c=1;
		bool b=true;
		while (b) {
			b=false;
			for (int i=0; i<spanningTree.size(); i++) {
				 if (spanningTree[i].size()==c) {
					pair<vector<GLfloat>, vector<GLfloat> > aresta=getAresta(spanningTree[i][0], i);
					pair<vector<GLfloat>, vector<GLfloat> > p(aresta.first,sub(aresta.second,aresta.first));
					ret[i].push_back(make_pair(aresta.first, sub(aresta.second, aresta.first)));
					if (c>1) {
						ret[i].insert(ret[i].end(), ret[spanningTree[i][0]].begin(), ret[spanningTree[i][0]].end());
					}
					b=true;	
				}
			}
			c+=1;
		}
		return ret;
	}
};


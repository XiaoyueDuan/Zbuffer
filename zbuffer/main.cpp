#include "loadOBJfiles.h"
#include "geometryTransfer.h"
#include "scanlineZbuffer.h"

int SCREEN_WIDTH = 500;
int SCREEN_HEIGHT = 500;
float Ortho = 5;

vector < vec3 > orig_vertices;
vector < ivec3 > scrn_vertices;
vector < nPolygon > polygons;

map<int, vector<classifiedPolygon>> cpTable;
map<int, vector<classifiedEdge>> ceTable;

vector<float> zbuffer_depth;
vector<vec3> framebuffer_color;

void reshape(int w, int h)
{
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	float scale = float(w) / float(h);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-Ortho*scale, Ortho*scale, -Ortho, Ortho);
	////gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0, 0, 10, // MODIFY!!!!	(0,0,X)
	//		  0, 0, 0,	// look at original point
	//		  0, 1, 0);	// y-coordinate direction
}

void display()
{
	// Initialize
	glClear(GL_COLOR_BUFFER_BIT);
	cpTable.clear();
	ceTable.clear();

	geoTrasProcessor proc;
	proc.getScreenPos(orig_vertices, scrn_vertices);
	proc.trans2classifiedPolygon(scrn_vertices, polygons, cpTable, ceTable);

	scanlineProccer scanproc;
	scanproc.scanlineZbuffer(cpTable, ceTable, SCREEN_WIDTH);

	//glutSwapBuffers();
	cout << "Finish a frame!" << endl;
}

int main(int argc, char *argv[])
{
	loadOBJ("../model/obj_ford_sample.obj", orig_vertices, polygons, Ortho);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Z-buffer");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	glutMainLoop();
	return 0;
}
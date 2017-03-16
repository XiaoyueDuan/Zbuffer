#include "loadOBJfiles.h"
#include "geometryTransfer.h"
#include "scanlineZbuffer.h"

#include <string> 

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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

	cout << "Finish a frame!" << endl;
}

int main(int argc, char *argv[])
{	
	cout << "Please enter the name of .obj file.";
	cout << "The program will draw 'obj_ford_sample.obj' if there is no entering."<< endl;
	cout << "(The default folder is ./model)";

	string objname;
	getline(cin, objname);

	if (objname.empty())
		objname = "../model/obj_ford_sample.obj";
	else
		objname = "../model/"+ objname;

	if (loadOBJ(objname.c_str(), orig_vertices, polygons, Ortho))
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Z-buffer");

		glutDisplayFunc(display);
		glutReshapeFunc(reshape);

		glutMainLoop();
	}

	system("PAUSE ");
	return 0;
}
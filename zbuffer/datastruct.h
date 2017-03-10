#pragma once

// datastruct.h Description: 
// 
#include "glm/glm.hpp"
#include <vector>
#include <cstring>
#include <map>

using namespace std;
using namespace glm;

class nPolygon
{
public:
	vector<int> vertices;

	ivec3 *getVertex(vector<ivec3> &, int index);
};

class classifiedPolygon
{
public:
	float a, b, c, d;	// the coefficient of plane polygon belonging to
	int id;
	int dy;				// how many scanning lines the polygon occupies
	float color[3];	
	// new
	int ymax;

	classifiedPolygon(){}
	classifiedPolygon(nPolygon &, vector<ivec3> &, int index);
	//bool operator == (const classifiedPolygon &ce2);
	
private:
	void genColor();
	void calPlane(ivec3 &v0, ivec3 &v1, ivec3 &v2);
	void calDy(nPolygon &p, vector<ivec3> &vertices);
};

class classifiedEdge
{
public:
	float x;
	float dx;
	int dy;
	int id;
	// new
	int ymax;
	int sid, eid; // start and end index
				  // in fact, the order of start and end is meaningless

	classifiedEdge() {  }
	classifiedEdge(ivec3 &v1, ivec3 &v2,int s,int e, int index);

	bool operator == (const classifiedEdge &ce2);
	bool leftThan(classifiedEdge &ce2);
	bool canComp(classifiedEdge &ce2);

private:
	void calX(ivec3 &v1, ivec3 &v2);
	void calDx(ivec3 &v1, ivec3 &v2);
	void calDy(ivec3 &v1, ivec3 &v2);
};

class activatedPolygon
{
public:
	float a, b, c, d;
	int id;
	int dy;
	float color[3];

	activatedPolygon(classifiedPolygon &cp);
};

class activatedEdge
{
public:
	float xl, xr;
	float dxl, dxr;
	int dyl, dyr;

	float zl;
	float dzx;
	float dzy;
	int id;

	activatedEdge(classifiedPolygon &cp, classifiedEdge &celeft, classifiedEdge &ceright);

	void changeLeftEdge(classifiedEdge &celeft);
	void changeRightEdge(classifiedEdge &ceright);
};
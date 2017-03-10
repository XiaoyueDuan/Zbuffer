#pragma once

#include <map>
#include "gl\glut.h"
#include "datastruct.h"

class geoTrasProcessor
{
public:
	ivec3 view;					// the view, default is (0,0,-1)

	geoTrasProcessor() { view = *(new ivec3(0, 0, -1)); }
	
	bool trans2classifiedPolygon(vector<ivec3> &vertices, vector<nPolygon> &polygons,
								 map<int, vector<classifiedPolygon>> &cpTable,
								 map<int, vector<classifiedEdge>> &ceTable);
	bool insert2classifiedLineTable(nPolygon &p, int &lineid,
		vector<ivec3> &vertices,
		map<int, vector<classifiedEdge>> &ceTable);
	bool backface(classifiedPolygon &);
	bool perpendicular(classifiedPolygon &);
	void setView(int x1, int x2, int x3);
	void getScreenPos(vector<vec3> &, vector<ivec3> &);

private:
	bool insertCPTable(map<int, vector<classifiedPolygon>> &cpTable, classifiedPolygon &cp);
	bool insertCETable(map<int, vector<classifiedEdge>> &ceTable, classifiedEdge &cp);
	//ivec3 getScreenPos(vec3 &);
};


#pragma once

#include "gl\glut.h"
#include "datastruct.h"

class scanlineProccer
{
public:
	void scanlineZbuffer(map<int, vector<classifiedPolygon>> &cpTable,
						 map<int, vector<classifiedEdge>> &ceTable, 
						 int width);
	void draw(vector<double> &zbuffer, vector<int> &colorsId);

private:
	bool addActivatedPolygonTable(map<int, vector<classifiedPolygon>> &cpTable,
								  vector<activatedPolygon> &apTable, int ymax);

	bool addActivatedEdgeTable(vector <classifiedEdge> &ceTable_y,
		vector<activatedEdge> &aeTable, vector<classifiedPolygon> &classifiedPolygons);

	bool addActivatedEdge(vector<classifiedEdge> &ceTable,
		vector<activatedEdge> &aeTable, classifiedPolygon &cp);

	
	void modifyActivatedPolygon_EdgeTable(
		vector<activatedPolygon> &apTable,	vector<activatedEdge> &aeTable,
		vector<classifiedEdge> &ceTable_y);

	bool modifyActivatedPolygon(activatedPolygon &ap);	// if it should be deleted, return false

	void modifyActivatedEdge(activatedEdge &ae, vector<classifiedEdge> &aeTable_y);
	int findNextClassifiedEdge(activatedEdge &ae, float x, vector<classifiedEdge> &aeTable_y);

	void drawingFixedY(int y, vector<int> &colorsId, vector<activatedPolygon> &apTable,
						int width);
	bool getColor(int id, vector<activatedPolygon> &apTable, vec3 &color);
	void drawLine(vec2 &start, vec2 &end, vec3 &color);
};

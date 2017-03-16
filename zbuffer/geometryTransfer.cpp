# include <algorithm>

# include "geometryTransfer.h"

bool geoTrasProcessor::trans2classifiedPolygon
	(vector<ivec3> &vertices, vector<nPolygon> &polygons,
	map<int, vector<classifiedPolygon>> &cpTable,
	map<int, vector<classifiedEdge>> &ceTable)
{
	int polyid = 0;
	for (int i = 0; i < polygons.size(); ++i)
	{
		classifiedPolygon cp(polygons[i], vertices, polyid);

		//if (backface(cp))	// if it is a back face, don't put it into classified polygon table
		//	continue;

		if (perpendicular(cp)|| !insertCPTable(cpTable, cp))
			continue;

		if(insert2classifiedLineTable(polygons[i], polyid,vertices, ceTable))
			++polyid;
	}
	return true;
}

bool geoTrasProcessor::insert2classifiedLineTable(
	nPolygon &p, 
	int &polyid,
	vector<ivec3> &vertices,
	map<int, vector<classifiedEdge>> &ceTable)
{
	ivec3 *s, *e;	// start and end vertex
	int sid, eid;	// start and end index
	for (int i = 0; i < p.vertices.size(); ++i)
	{
		sid = i;
		eid = (i + 1) % p.vertices.size();		

		s = p.getVertex(vertices, sid);
		e = p.getVertex(vertices, eid);

		classifiedEdge cp(*s, *e, p.vertices[sid], p.vertices[eid], polyid);
		insertCETable(ceTable, cp);	
	}
	return true;
}

bool geoTrasProcessor::backface(classifiedPolygon &cp)
{
	if ((cp.a*view.x + cp.b*view.y + cp.c*view.z) < 0)
		return false;	// don't need to cull
	else
		return true;	// should be culled since it is a back face
}

bool geoTrasProcessor::perpendicular(classifiedPolygon &cp)
{
	if ((cp.a*view.x + cp.b*view.y + cp.c*view.z) == 0)
		return true;	// the polygon is perpendicular to the view
	return false;
}

void geoTrasProcessor::setView(int x1, int x2, int x3)
{
	view.x = x1;
	view.y = x2;
	view.z = x3;
}

void geoTrasProcessor::getScreenPos(vector<vec3> &original, vector<ivec3> &screen_output)
{
	// original vertices are the vertex position in Model Cooridinate
	// output are the position in Screen Cooridinate

	screen_output.clear();

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	vector<vec3>::iterator iter;
	for (iter = original.begin(); iter != original.end(); ++iter)
	{
		GLdouble x,y,z;

		gluProject(iter->x, iter->y, iter->z,
					modelview, projection, viewport,
					&x, &y, &z);
		ivec3 screen_pos(x, y, iter->z);
		screen_output.push_back(screen_pos);
	}
}

bool geoTrasProcessor::insertCPTable(
	map<int, vector<classifiedPolygon>> &cpTable,
	classifiedPolygon &cp)
{
	// if the classified polygon exists in cpTable, return false, representing failure
	// else return true
	bool not_exist = (cpTable.find(cp.ymax) == cpTable.end());
	if(not_exist)
		cpTable[cp.ymax] = vector <classifiedPolygon> (1,cp);	
	else	
		cpTable[cp.ymax].push_back(cp);	

	// or only 
	// cpTable[cp.ymax].push_back(cp);	
	return true;
}

bool geoTrasProcessor::insertCETable(
	map<int, vector<classifiedEdge>> &ceTable, 
	classifiedEdge &ce)
{
	// if the classified edge exists in cpTable, return false, representing failure
	// else return true
	bool succ=true;

	// not exist case 1: ce is not in ceTable, since no ceTable[ce.ymax]		
	map<int, vector<classifiedEdge>>:: iterator iter = ceTable.find(ce.ymax);
	if (iter ==ceTable.end()) 	
		ceTable[ce.ymax] = vector<classifiedEdge>(1, ce);	
	else
	{
		vector<classifiedEdge>::iterator iter2 = 
			find(ceTable[ce.ymax].begin(), ceTable[ce.ymax].end(), ce);

		if (iter2 != ceTable[ce.ymax].end())
			succ = false;	// exist already
		else
			// not exist case 2: ce is not in ceTable[ce.ymax]
			ceTable[ce.ymax].push_back(ce);
	}
	
	return succ;
}
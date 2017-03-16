#include <random>
#include "datastruct.h"

// Polygon defination
ivec3 * nPolygon::getVertex(vector<ivec3> &vertices, int index)
{
	// return the 
	// for example, (7,8,9)
	//				 1 2 3
	// getVertex(...,1) get the 7th ivec3 in "vector<ivec3> vertices"
	return &(vertices[ this->vertices[index]-1 ]);
}


// classifiedPolygon defination
classifiedPolygon::classifiedPolygon(nPolygon &p, vector<ivec3> &vertices, int index)
{
	id = index;
	genColor();
	
	ivec3 *v0 = p.getVertex(vertices, 0);
	ivec3 *v1 = p.getVertex(vertices, 1);
	ivec3 *v2 = p.getVertex(vertices, 2);
	
	calPlane(*v0, *v1, *v2);
	calDy(p, vertices);
}

void classifiedPolygon::genColor()
{
	for (int i = 0; i < 3; ++i)
		color[i] = float(rand() % 1000+1) / 1000;
}

void classifiedPolygon::calDy(nPolygon &p, vector<ivec3> &vertices)
{
	int minY, maxY;

	for (int i = 0; i < p.vertices.size(); ++i)
	{
		ivec3 *v=p.getVertex(vertices,i);
		if (i == 0)		
			minY = maxY = (*v).y;	
		else
		{
			minY = min(minY, v->y);
			maxY = max(maxY, v->y);
		}
	}

	ymax = maxY;
	dy= int(maxY - minY);
}

void classifiedPolygon::calPlane(ivec3 &v1, ivec3 &v2, ivec3 &v3)
{
	a = float( (v2.y - v1.y)*(v3.z - v1.z) - (v2.z - v1.z)*(v3.y - v1.y) );
	b = float( (v2.z - v1.z)*(v3.x - v1.x) - (v2.x - v1.x)*(v3.z - v1.z) );
	c = float( (v2.x - v1.x)*(v3.y - v1.y) - (v2.y - v1.y)*(v3.x - v1.x) );
	d = float( -(a*v1.x + b*v1.y + c*v1.z) );
}


// classifiedEdge defination
classifiedEdge::classifiedEdge(ivec3 &v1, ivec3 &v2,int s,int e, int index)
{
	id = index;
	sid = s;
	eid = e;

	calX(v1,v2);
	calDx(v1, v2);
	calDy(v1, v2);
}

bool classifiedEdge:: operator == (const classifiedEdge &ce2)
{
	// the two vertices of two line are the same
	return ((this->sid == ce2.sid) && (this->eid == ce2.eid)
		|| (this->sid == ce2.eid) && (this->eid == ce2.sid))
		&& this->id==ce2.id;
}

bool classifiedEdge::canComp(classifiedEdge &ce2)
{
	if (id == ce2.id)
		return true;
	else
		return false;
}

bool classifiedEdge::leftThan(classifiedEdge &ce2)
{
	if (canComp(ce2))
	{
		if (x < ce2.x)
			return true;
		else if (x == ce2.x)
		{
			if (ce2.dx==-INT_MAX)// condition 1&2
				return true;
			else if (dx < ce2.dx)// other condition
				return true;	
		}
	}
	return false;
}

void classifiedEdge::calX(ivec3 &v1, ivec3 &v2)
{
	if (v1.y < v2.y)
		x = v2.x;
	else
		x = v1.x;
}

void classifiedEdge::calDx(ivec3 &v1, ivec3 &v2)
{
	if (v1.y == v2.y)	
		dx = -INT_MAX;	
	else
		dx = float(v2.x - v1.x) / float(v1.y - v2.y);	// -1/k
}

void classifiedEdge::calDy(ivec3 &v1, ivec3 &v2)
{
	ymax = max(v1.y, v2.y);
	dy = int(abs(v1.y - v2.y));
}


// activatedPolygon
activatedPolygon::activatedPolygon(classifiedPolygon &cp)
{
	a = cp.a;
	b = cp.b;
	c = cp.c;
	d = cp.d;

	id = cp.id;
	dy = cp.dy;	
	memcpy(color, cp.color, sizeof(cp.color));
}


// activatedEdge
activatedEdge::activatedEdge(classifiedPolygon &cp, classifiedEdge &celeft, classifiedEdge &ceright)
{
	xl = celeft.x;		xr = ceright.x;
	dxl = celeft.dx;	dxr = ceright.dx;
	dyl = celeft.dy;	dyr = ceright.dy;
	
	if (cp.c != 0)
	{
		dzx = -cp.a / cp.c;
		dzy = cp.b / cp.c;
		zl = dzx*xl-dzy*celeft.ymax-cp.d/cp.c;
	}
	else
	{
		dzx = -INT_MAX;
		dzy = INT_MAX;
		zl = -INT_MAX;	// a line, invisiable
	}
		
	id = cp.id;
}

void activatedEdge::changeLeftEdge(classifiedEdge &celeft)
{
	xl = celeft.x;		
	dxl = celeft.dx;	
	dyl = celeft.dy;	

}

void activatedEdge::changeRightEdge(classifiedEdge &ceright)
{
	xr = ceright.x;
	dxr = ceright.dx;
	dyr = ceright.dy;
}
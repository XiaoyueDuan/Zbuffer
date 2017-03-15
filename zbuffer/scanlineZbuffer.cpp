#include "scanlineZbuffer.h"

void scanlineProccer::scanlineZbuffer(map<int, vector<classifiedPolygon>> &cpTable,
	map<int, vector<classifiedEdge>> &ceTable,
	int width)
{

	int low = cpTable.begin()->first;	// not the lowest one, but if all polygons with ymax==low are drawed,
										// the scannning task completes.
	int high = (--cpTable.end())->first;

	////
	//map<activatedPolygon, activatedEdge> a;// first is ap, second is ae accordingly!!!!!!!!!!!!!!!!!!!!!! 有没有map以外的配对？
	////

	vector<activatedPolygon> apTable;
	vector<activatedEdge> aeTable;

	//glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_LINE_SMOOTH);

	for (int i = high;	; --i)
	{
		vector<double> zbuffer(width, -INT_MAX);
		vector<int> colorsId(width, -1);

		// add new polygon with the edges accordingly
		if (addActivatedPolygonTable(cpTable, apTable, i))
		{
			// if success
			// then, add the edges belonging to the polygons whose ymax == i
			addActivatedEdgeTable(ceTable[i], aeTable, cpTable[i]);
		}

		// depth updates
		vector<activatedEdge>::iterator iter;
		for (iter = aeTable.begin(); iter != aeTable.end(); ++iter)
		{
			float zx;
			for (int x = int(iter->xl); x <= iter->xr; ++x)
			{
				if (x == int(iter->xl))
					zx = iter->zl;
				else
					zx += iter->dzx;

				if (zx > zbuffer[x])
				{
					zbuffer[x] = zx;
					colorsId[x] = iter->id;					
				}					
			}
		}		

		// DRAW!!!!!
		drawingFixedY(i, colorsId, apTable, width);

		// activated polygons and edges update
		modifyActivatedPolygon_EdgeTable(apTable, aeTable, ceTable[i]);		

		if (i<=low&&apTable.empty())
			break;
	}	


}

void scanlineProccer::draw(vector<double> &zbuffer, vector<int> &colorsId)
{

}

bool scanlineProccer::addActivatedPolygonTable(map<int, vector<classifiedPolygon>> &cpTable,
	vector<activatedPolygon> &apTable, int ymax)
{
	map<int, vector<classifiedPolygon>>::iterator iter = cpTable.find(ymax);
	if (iter == cpTable.end())	// not exist
		return false;
	else
	{
		vector<classifiedPolygon>::iterator iter2;
		for (iter2 = cpTable[ymax].begin(); iter2 != cpTable[ymax].end(); ++iter2)
		{
			activatedPolygon ap(*iter2);
			apTable.push_back(ap);
		}
		return true;
	}
}

bool scanlineProccer::addActivatedEdgeTable( vector <classifiedEdge> &ceTable_y,
	vector<activatedEdge> &aeTable, vector<classifiedPolygon> &classifiedPolygons)
{
	// add all correct edges of all polygons in classifiedPolygons into aeTable
	// the correct edges belong to ceTable_y

	for (int i = 0; i < classifiedPolygons.size(); ++i)
		addActivatedEdge(ceTable_y, aeTable, classifiedPolygons[i]);
	return true;
}

bool scanlineProccer::addActivatedEdge(vector<classifiedEdge> &ceTable_y,
	vector<activatedEdge> &aeTable,  classifiedPolygon &cp)
{
	int left = -1, right = -1;
	
	for (int i = 0; i < ceTable_y.size(); ++i)
	{
		if (ceTable_y[i].id == cp.id)		
			if ((left == -1) || (right == -1))			
				left = right = i;			
			else
			{
				if (ceTable_y[i].leftThan(ceTable_y[left]))	// the new lefter than the leftest one 
					left = i;
				else if (ceTable_y[right].leftThan(ceTable_y[i]))	// rightest one lefter than the new one
					right = i;
			}	
	}

	if (left == right)
		return false;
	else
	{
		activatedEdge ae(cp, ceTable_y[left], ceTable_y[right]);
		aeTable.push_back(ae);
		return true;
	}
}

void scanlineProccer::modifyActivatedPolygon_EdgeTable(
	vector<activatedPolygon> &apTable, 
	vector<activatedEdge> &aeTable,
	vector<classifiedEdge> &ceTable_y)
{
	for (int i = 0; i < apTable.size(); ++i)
	{
		if (modifyActivatedPolygon(apTable[i]))		
			modifyActivatedEdge(aeTable[i], ceTable_y);		
		else
		{
			apTable.erase(apTable.begin()+i);	// erase the polygon
			aeTable.erase(aeTable.begin() + i);	// erase the edge accordingly
			--i;	// if not, it will skip one
					// without --i: delete first two of them
					// yes(*)	yes	no	no
					// yes		no(*)	no
					// yes		no		no(*)
		}
	}
}

bool scanlineProccer::modifyActivatedPolygon(activatedPolygon &ap)
{
	// if it should be deleted, return false
	if ((--ap.dy) < 0)
		return false;
	else
		return true;
}

void scanlineProccer::modifyActivatedEdge(activatedEdge &ae, vector<classifiedEdge> &ceTable_y)
{
	// the polygon is still in activated polygon table
	if ((--ae.dyl) < 0)
	{
		int id=findNextClassifiedEdge(ae, ae.xl, ceTable_y);
		if (id != -1)
			ae.changeLeftEdge(ceTable_y[id]);
	}
	if ((--ae.dyr) < 0)
	{
		int id = findNextClassifiedEdge(ae, ae.xr, ceTable_y);
		if (id != -1)
			ae.changeRightEdge(ceTable_y[id]);
	}
	ae.xl += ae.dxl;
	ae.xr += ae.dxr;
	ae.zl += (ae.dzx*ae.dxl + ae.dzy);
}

int scanlineProccer::findNextClassifiedEdge(activatedEdge &ae,float x, vector<classifiedEdge> &ceTable_y)
{
	int id = ae.id;
	for (int i = 0; i < ceTable_y.size(); ++i)
	{
		if ((ceTable_y[i].id == id) && (abs(ceTable_y[i].x - x)<1) )
			return i;
	}
	return -1;	// if not found
}

// drawing
void scanlineProccer::drawingFixedY
(int y, vector<int> &colorsId, vector<activatedPolygon> &apTable,
	int width)
{
	int start=-1;	// index of start point
	//float z = 0.0;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	glBegin(GL_LINES);
	for (int i = 0; i < colorsId.size(); ++i)
	{
		if (colorsId[i] == -1)	
		{
			if(start == -1)	// background
				continue;
			else			// a line finished with background
			{
				vec3 color;
				getColor(colorsId[start], apTable, color);
				GLdouble sx, ex, yf, tmpZ;

				gluUnProject(start, y, 0,
					modelview, projection, viewport,
					&sx, &yf, &tmpZ);
				gluUnProject(i - 1, y, 0,
					modelview, projection, viewport,
					&ex, &yf, &tmpZ);

				drawLine(vec2(sx, yf),vec2( ex, yf), color);

				start = -1;
				continue;	// two continue for easier understanding
			}
		}			

		if (start == -1)
			start = i;
		else
		{
			if (colorsId[i] == colorsId[start])
				continue;
			else
			{
				vec3 color;
				getColor(colorsId[start],apTable,color);
				GLdouble sx ,ex, yf, tmpZ;

				gluUnProject(start, y, 0,
					modelview, projection, viewport,
					&sx, &yf, &tmpZ);

				gluUnProject(i - 1, y, 0,
					modelview, projection, viewport,
					&ex, &yf, &tmpZ);

				//float sx = float(start) / width;
				//float ex = float(i - 1) / width;
				//float yf = float(y) / height;
				drawLine(vec2(sx, yf), vec2(ex, yf), color);

				start = i;
				continue;
			}
		}
	}
	glEnd();
	glFlush();
}

bool scanlineProccer::getColor(int id, vector<activatedPolygon> &apTable, vec3 &color)
// id is the polygon index which the edge belongs to 
{
	vector<activatedPolygon>::iterator iter = apTable.begin();
	for (; iter != apTable.end(); ++iter)
	{
		if (iter->id == id)
		{
			color.x = iter->color[0];
			color.y = iter->color[1];
			color.z = iter->color[2];
			return true;
		}
	}
	// if not find
	return false;
}

void scanlineProccer::drawLine(vec2 &start, vec2 &end, vec3 &color)
{
	// set color	
	glColor3f(GLfloat(color.x), GLfloat(color.y), GLfloat(color.z));

	// draw line	
	glVertex2f(GLfloat(start.x), GLfloat(start.y));
	glVertex2f(GLfloat(end.x), GLfloat(end.y));
}
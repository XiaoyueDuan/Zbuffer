#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "datastruct.h"

bool loadOBJ(
	const char * path,
	vector < vec3 > & out_vertices,
	vector < nPolygon > & out_polygon,
	float &Ortho
);

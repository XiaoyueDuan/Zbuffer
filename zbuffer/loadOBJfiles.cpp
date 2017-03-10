#include <string>
#include <sstream>
#include "loadOBJfiles.h"

bool loadOBJ(
	const char * path,
	vector < vec3 > & out_vertices,
	vector < nPolygon > & out_polygon,
	float &Ortho
)
{
	// step 1: test whether the file exists 
	ifstream in(path);
	if (in.fail())
	{
		cout << "The file does not exist!" << endl;
		return false;
	}

	streambuf *cinbuf = cin.rdbuf();	//save old buf
	cin.rdbuf(in.rdbuf());				//redirect cin to file

	float minX = INT_MAX, minY = INT_MAX;
	float maxX = -INT_MAX, maxY = -INT_MAX;
	// step 2: load the data according to its start character
	// case 1: v v1, v2, v3
	// case 2: f v1, v2, ...
	string header;
	while (true)
	{
		// read the head of line
		getline(cin, header);
		if (cin.eof())
			break; // End Of File. Quit the loop.
		else
		{		
			// else : parse lineheader		
			istringstream ss(header);
			
			if (header.empty())
				continue;
			else if (header[0] == 'v')
			{
				string type;
				vec3 vertex;				
				
				ss >> type;
				ss >> vertex.x;
				ss >> vertex.y;
				ss >> vertex.z;
				out_vertices.push_back(vertex);

				minX = min(minX, vertex.x);
				minY = min(minY, vertex.y);
				maxX = max(maxX, vertex.x);
				maxY = max(maxY, vertex.y);
			}
			else if ((header[0] == 'f')||(header.substr(0,2) == "#f"))
			{
				string type;
				int input;
				nPolygon plgon;
				
				ss >> type;
				while (ss >> input)				
					plgon.vertices.push_back(input);

				out_polygon.push_back(plgon);
			}
			//else if((header[0]!=' ')||(header[0]!='\t'))
			//{
			//	cout << "the file format is illegal!" << endl;
			//	cin.rdbuf(cinbuf);//reset to standard output again
			//	return false;
			//}				
		}
	}
	cin.rdbuf(cinbuf);//reset to standard output again
	Ortho = max(abs(minX), abs(minY));
	Ortho = max(Ortho, abs(maxX));
	Ortho = 1.5*max(Ortho, abs(maxY));
	return true;
}
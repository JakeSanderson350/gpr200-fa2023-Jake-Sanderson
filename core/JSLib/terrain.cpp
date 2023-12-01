#include "procGen.h"
namespace JSLib
{
	ew::MeshData createTerrain(float width, float height, int subdivisions)
	{
		int row, col, start;
		ew::Vertex v;
		ew::MeshData mesh;

		//Vertices
		for (row = 0; row <= subdivisions; row++)
		{
			for (col = 0; col <= subdivisions; col++)
			{
				v.pos.x = width * (col / (float)subdivisions);
				v.pos.z = -height * (row / (float)subdivisions);

				v.normal = ew::Vec3(0.0f, 1.0f, 0.0f);

				v.uv.x = col / (float)subdivisions;
				v.uv.y = row / (float)subdivisions;

				mesh.vertices.push_back(v);
			}
		}

		//Indices
		int columns = subdivisions + 1;

		for (row = 0; row < subdivisions; row++)
		{
			for (col = 0; col < subdivisions; col++)
			{
				start = row * columns + col;

				//Bottom right triangle
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns + 1);

				//Top left triangle
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns);
			}
		}

		return mesh;
	}
}
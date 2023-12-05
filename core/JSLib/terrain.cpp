#include "terrain.h"
namespace JSLib
{
	ew::MeshData createTerrain(float width1, float height1, int subdivisions, char* heightMap)
	{
		int row, col, start;
		ew::Vertex v;
		ew::MeshData mesh;

		int width, height, numComponents;
		unsigned char* data = stbi_load(heightMap, &width, &height, &numComponents, 0);

		float yScale = 64.0f / 256.0f, yShift = 16.0f;

		//Vertices
		for (row = 0; row < height; row++)
		{
			for (col = 0; col < width; col++)
			{
				unsigned char* texel = data + (col + width * row) * numComponents;
				unsigned char y = texel[0];

				/*v.pos.x = width * (col / (float)subdivisions);
				v.pos.y = (int)y;
				v.pos.z = -height * (row / (float)subdivisions);*/

				v.pos.x = -height / 2.0f + row;
				v.pos.y = (int)y * yScale - yShift;
				v.pos.z = -width / 2.0f + col;

				v.normal = ew::Vec3(0.0f, 1.0f, 0.0f);

				v.uv.x = col / (float)height;
				v.uv.y = row / (float)width;

				mesh.vertices.push_back(v);
			}
		}

		stbi_image_free(data);

		//Indices
		int columns = width ;

		for (row = 0; row < height - 1; row++)
		{
			for (col = 0; col < width; col++)
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

		/*for (row = 0; row < height - 1; row++)
		{
			for (col = 0; col < width; col++)
			{
				for (start = 0; start < 2; start++)
				{
					mesh.indices.push_back(col + width * (row + start));
				}
			}
		}*/

		return mesh;
	}
}
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
		int indBottomLeft, indTopLeft, indTopRight, indBottomRight;

		for (row = 0; row < height - 1; row++)
		{
			for (col = 0; col < width - 1; col++)
			{
				//Indices for each quad
				indBottomLeft = row * width + col;
				indTopLeft = (row + 1) * width + col;
				indTopRight = (row + 1) * width + col + 1;
				indBottomRight = row * width + col + 1;

				//Top left triangle
				mesh.indices.push_back(indBottomLeft);
				mesh.indices.push_back(indTopRight);
				mesh.indices.push_back(indTopLeft);

				//Bottom right triangle
				mesh.indices.push_back(indBottomLeft);
				mesh.indices.push_back(indBottomRight);
				mesh.indices.push_back(indTopRight);
			}
		}

		return mesh;
	}
}
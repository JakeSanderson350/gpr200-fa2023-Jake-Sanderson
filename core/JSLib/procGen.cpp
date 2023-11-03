#include "procGen.h"
namespace JSLib
{
	ew::MeshData createSphere(float radius, int numSegments)
	{
		int row, col, start, i;
		ew::Vertex v;
		ew::MeshData mesh;

	//Vertices
		float thetaStep = (2 * ew::PI) / numSegments;
		float theta;
		float phiStep = ew::PI / numSegments;
		float phi;

		for (row = 0; row <= numSegments; row++)
		{
			phi = row * phiStep;

			for (col = 0; col <= numSegments; col++)
			{
				theta = col * thetaStep;

				v.pos.x = radius * cos(theta) * sin(phi);
				v.pos.y = radius * cos(phi);
				v.pos.z = radius * sin(theta) * sin(phi);

				v.normal = ew::Normalize(v.pos);

				v.uv.x = col / (float)numSegments;
				v.uv.y = row / (float)numSegments;

				mesh.vertices.push_back(v);
			}
		}

	//Indices
		//Top cap
		int poleStart = 0;
		int sideStart = numSegments + 1;
		for (i = 0; i < numSegments; i++)
		{
			mesh.indices.push_back(sideStart + i);
			mesh.indices.push_back(poleStart + i);
			mesh.indices.push_back(sideStart + i + 1);
		}

		//Row indices
		int columns = numSegments + 1;
		for (row = 1; row < numSegments; row++)
		{
			for (col = 0; col < numSegments; col++)
			{
				start = row * columns + col;

				//Triangle 1
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns);

				//Triangle 2
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns);
			}
		}

		return mesh;
	}

	
	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		int i;
		ew::Vertex v;
		ew::MeshData mesh;

	//Vertices
		float topY = height / 2;
		float bottomY = -topY;

		float thetaStep = (2 * ew::PI) / numSegments;
		float theta;

		//Top Center
		ew::Vertex topCenter;
		topCenter.pos = ew::Vec3(0, topY, 0);
		topCenter.normal = ew::Vec3(0, 1, 0);
		topCenter.uv = ew::Vec2(0.5, 0.5);
		mesh.vertices.push_back(topCenter);

		//Top Ring up normals
		for (i = 0; i <= numSegments; i++)
		{
			theta = i * thetaStep;
			
			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;
			v.pos.y = topY;

			v.normal = ew::Vec3(0, 1, 0);

			v.uv.x = (cos(theta) + 1) * 0.5;
			v.uv.y = (sin(theta) + 1) * 0.5;
			
			mesh.vertices.push_back(v);
		}

		//Top ring side normals
		for (i = 0; i <= numSegments; i++)
		{
			theta = i * thetaStep;

			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;
			v.pos.y = topY;

			v.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta)));

			v.uv.x = (cos(theta) + 1) * 0.5;
			v.uv.y = 1;

			mesh.vertices.push_back(v);
		}

		//Bottom ring side normals
		for (i = 0; i <= numSegments; i++)
		{
			theta = i * thetaStep;

			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;
			v.pos.y = bottomY;

			v.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta)));

			v.uv.x = (cos(theta) + 1) * 0.5;
			v.uv.y = 0;

			mesh.vertices.push_back(v);
		}

		//Bottom ring down normals
		for (i = 0; i <= numSegments; i++)
		{
			theta = i * thetaStep;

			v.pos.x = cos(theta) * radius;
			v.pos.z = sin(theta) * radius;
			v.pos.y = bottomY;

			v.normal = ew::Vec3(0, -1, 0);

			v.uv.x = (cos(theta) + 1) * 0.5;
			v.uv.y = (sin(theta) + 1) * 0.5;

			mesh.vertices.push_back(v);
		}

		//Bottom center
		ew::Vertex bottomCenter;
		bottomCenter.pos = ew::Vec3(0, bottomY, 0);
		bottomCenter.normal = ew::Vec3(0, -1, 0);
		bottomCenter.uv = ew::Vec2(0.5, 0.5);
		mesh.vertices.push_back(bottomCenter);

	//Indices
		//Top ring
		int start = 1;
		int center = 0;
		for (i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(start + i);
			mesh.indices.push_back(center);
			mesh.indices.push_back(start + i + 1);
		}

		//Sides
		int sideStart = numSegments + 1;
		int columns = numSegments + 1;
		for (i = 0; i < columns; i++)
		{
			start = sideStart + i;

			//Triangle 1
			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns);

			//Triangle 2
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
		}

		//Bottom ring
		start = ((numSegments + 1) * 3) + 1;
		center = ((numSegments + 1) * 4) + 1;
		for (i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(start + i + 1);
			mesh.indices.push_back(center);
			mesh.indices.push_back(start + i);
			
		}

		return mesh;
	}
	
	
	ew::MeshData createPlane(float width, float height, int subdivisions)
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
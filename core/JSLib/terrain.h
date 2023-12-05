#pragma once
#include "../ew/mesh.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"
namespace JSLib
{
	ew::MeshData createTerrain(float width, float height, int subdivisions, char* heightMap);
}
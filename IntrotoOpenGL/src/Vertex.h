#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Vertex
{
public:
	glm::vec4 position;
	glm::vec4 colour;
	glm::vec3 normal;
};

struct VertexAdv
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
};

#endif
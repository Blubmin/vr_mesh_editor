#pragma once

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

class Edge;
class Triangle;
class Vertex;

class Mesh
{
	GLuint _VAO;
	GLuint _VBO_verts;
	GLuint _VBO_indices;

	std::vector<glm::vec3> _vert_pos;
	std::vector<GLuint> _indices;

	std::vector<Vertex*> _verts;
	std::vector<Edge*> _edges;
	std::vector<Triangle*> _tris;
public:
	Mesh();
	~Mesh();
	void init_geom();
	void bind_geom();
	GLuint vao();
	GLuint num_indices();
	Triangle* create_tri(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
	Edge* create_edge(glm::vec3 v1, glm::vec3 v2);
	Vertex* create_vert(glm::vec3 v);
};


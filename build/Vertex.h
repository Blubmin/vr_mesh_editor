#pragma once

#include <vector>

#include <glm\glm.hpp>

class Edge;
class Quad;
class Triangle;

class Vertex
{
	bool _selected;
	glm::vec3 _pos;
	std::vector<Edge*> _edges;
	std::vector<Triangle*> _tris;
	std::vector<Quad*> _quads;

public:
	Vertex();
	Vertex(glm::vec3 pos);
	~Vertex();

	void set_pos(glm::vec3 pos);
	glm::vec3 get_pos();
	void add_triangle(Triangle * tri);
	void remove_triangle(Triangle * tri);
	void add_quad(Quad * quad);
	void remove_quad(Quad * quad);
	void add_edge(Edge* edge);
	void remove_edge(Edge* edge);
	void select();
	void deselect();
	bool selected();

	bool operator==(const Vertex& other);
	bool operator==(const glm::vec3& v);
};


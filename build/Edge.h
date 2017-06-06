#pragma once

#include <vector>

#include <glm\glm.hpp>

class Quad;
class Triangle;
class Vertex;

class Edge
{
	bool _selected;
	Vertex* _start;
	Vertex* _end;
	std::vector<Triangle*> _tris;
	std::vector<Quad*> _quads;

public:
	Edge();
	Edge(Vertex* start, Vertex* end);
	~Edge();

	Vertex* get_start();
	Vertex* get_end();
	void add_triangle(Triangle* tri);
	void remove_triangle(Triangle* tri);
	void add_quad(Quad * quad);
	void remove_quad(Quad * quad);
	std::vector<Triangle*> tris();
	std::vector<Quad*> quads();
	void select();
	void deselect();
	bool selected();

	bool operator==(const Edge& other);
};


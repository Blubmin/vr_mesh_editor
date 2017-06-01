#pragma once

#include <vector>

#include <glm\glm.hpp>

class Triangle;
class Vertex;

class Edge
{
	bool _selected;
	Vertex* _start;
	Vertex* _end;
	std::vector<Triangle*> _tris;

public:
	Edge();
	Edge(Vertex* start, Vertex* end);
	~Edge();

	Vertex* get_start();
	Vertex* get_end();
	void add_triangle(Triangle* tri);
	void remove_triangle(Triangle* tri);
	void select();
	void deselect();
	bool selected();

	bool operator==(const Edge& other);
};


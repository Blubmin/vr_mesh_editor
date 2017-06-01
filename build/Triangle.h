#pragma once

#include <vector>

class Edge;
class Vertex;

class Triangle
{
	bool _selected;
	std::vector<Edge*> _edges;
	std::vector<Vertex*> _verts;

public:
	Triangle();
	Triangle(Edge* e1, Edge* e2, Edge* e3);
	~Triangle();
	void add_vertex(Vertex * vert);
	void remove_vertex(Vertex * vert);
	void add_edge(Edge * edge);
	void remove_edge(Edge * edge);
	std::vector<Edge*> get_edges();
	void select();
	void deselect();
	bool selected();

	bool operator==(const Triangle& other);
};


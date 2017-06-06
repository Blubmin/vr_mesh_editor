#pragma once

#include <vector>

#include <glm\glm.hpp>

class Edge;
class Vertex;

class Quad
{
	bool _selected;
	std::vector<Edge*> _edges;
	std::vector<Vertex*> _verts;

public:
	Quad();
	~Quad();

	void add_vertex(Vertex * vert);
	void remove_vertex(Vertex * vert);
	void add_edge(Edge * edge);
	void remove_edge(Edge * edge);
	std::vector<Edge*> get_edges();
	void select();
	void deselect();
	bool selected();

	bool operator==(const Quad& other);
};


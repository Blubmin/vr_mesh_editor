#include "Quad.h"

#include <algorithm>

using namespace std;

Quad::Quad()
{
}


Quad::~Quad()
{
}

void Quad::add_vertex(Vertex * vert)
{
	_verts.push_back(vert);
}

void Quad::remove_vertex(Vertex * vert)
{
	_verts.erase(remove(_verts.begin(), _verts.end(), vert));
}

void Quad::add_edge(Edge * edge)
{
	_edges.push_back(edge);
}

void Quad::remove_edge(Edge * edge)
{
	_edges.erase(remove(_edges.begin(), _edges.end(), edge));
}

std::vector<Edge*> Quad::get_edges()
{
	return _edges;
}

void Quad::select()
{
	_selected = true;
}

void Quad::deselect()
{
	_selected = false;
}

bool Quad::selected()
{
	return _selected;
}

bool Quad::operator==(const Quad & other)
{
	bool cmp = true;
	for (int i = 0; cmp && i < _verts.size(); i++) {
		cmp &= _verts[i] == other._verts[i];
	}
	return cmp;
}


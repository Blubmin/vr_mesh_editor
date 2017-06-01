#include "Triangle.h"

#include <algorithm>

#include "Edge.h"

Triangle::Triangle()
{
}

Triangle::Triangle(Edge * e1, Edge * e2, Edge * e3)
{
	_edges.push_back(e1);
	_edges.push_back(e2);
	_edges.push_back(e3);

	_verts.push_back(e1->get_start());
	_verts.push_back(e2->get_start());
	_verts.push_back(e3->get_start());
}

Triangle::~Triangle()
{
}

void Triangle::add_vertex(Vertex * vert)
{
	_verts.push_back(vert);
}

void Triangle::remove_vertex(Vertex * vert)
{
	_verts.erase(remove(_verts.begin(), _verts.end(), vert));
}

void Triangle::add_edge(Edge * edge)
{
	_edges.push_back(edge);
}

void Triangle::remove_edge(Edge * edge)
{
	_edges.erase(remove(_edges.begin(), _edges.end(), edge));
}

std::vector<Edge*> Triangle::get_edges()
{
	return _edges;
}

void Triangle::select()
{
	_selected = true;
}

void Triangle::deselect()
{
	_selected = false;
}

bool Triangle::selected()
{
	return _selected;
}

bool Triangle::operator==(const Triangle& other) {
	bool cmp = true;
	for (int i = 0; cmp && i < _verts.size(); i++) {
		cmp &= _verts[i] == other._verts[i];
	}
	return cmp;
}
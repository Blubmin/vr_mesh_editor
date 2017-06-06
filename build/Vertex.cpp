#include "Vertex.h"

#include <algorithm>

using namespace glm;
using namespace std;

Vertex::Vertex()
{
}

Vertex::Vertex(glm::vec3 pos) {
	_pos = pos;
}

Vertex::~Vertex()
{
}

void Vertex::set_pos(glm::vec3 pos)
{
	_pos = pos;
}

glm::vec3 Vertex::get_pos()
{
	return _pos;
}

void Vertex::add_triangle(Triangle * tri)
{
	_tris.push_back(tri);
}

void Vertex::remove_triangle(Triangle * tri)
{
	_tris.erase(remove(_tris.begin(), _tris.end(), tri));
}

void Vertex::add_quad(Quad * quad)
{
	_quads.push_back(quad);
}

void Vertex::remove_quad(Quad * quad)
{
	_quads.erase(remove(_quads.begin(), _quads.end(), quad));
}

void Vertex::add_edge(Edge * edge)
{
	_edges.push_back(edge);
}

void Vertex::remove_edge(Edge * edge)
{
	_edges.erase(remove(_edges.begin(), _edges.end(), edge));
}

void Vertex::select()
{
	_selected = true;
}

void Vertex::deselect()
{
	_selected = false;
}

bool Vertex::selected()
{
	return _selected;
}

bool Vertex::operator==(const Vertex& other) {
	return _pos == other._pos;
}

bool Vertex::operator==(const glm::vec3& v) {
	return _pos == v;
}
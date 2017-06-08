#include "Edge.h"

#include <algorithm>

using namespace glm;
using namespace std;

Edge::Edge()
{
}

Edge::Edge(Vertex* start, Vertex* end)
{
	_start = start;
	_end = end;
    _selected = false;
}

Edge::~Edge()
{
}

Vertex * Edge::get_start()
{
	return _start;
}

Vertex * Edge::get_end()
{
	return _end;
}

void Edge::add_triangle(Triangle * tri)
{
	_tris.push_back(tri);
}

void Edge::remove_triangle(Triangle * tri)
{
	_tris.erase(remove(_tris.begin(), _tris.end(), tri));
}

void Edge::add_quad(Quad * quad)
{
	_quads.push_back(quad);
}

void Edge::remove_quad(Quad * quad)
{
	_quads.erase(remove(_quads.begin(), _quads.end(), quad));
}

void Edge::replace(Vertex * old_val, Vertex * new_val) {
    if (_start == old_val) _start = new_val;
    if (_end == old_val) _end = new_val;
}


vector<Triangle*>& Edge::tris() {
	return _tris;
}

vector<Quad*>& Edge::quads() {
	return _quads;
}

void Edge::select()
{
	_selected = true;
}

void Edge::deselect()
{
	_selected = false;
}

bool Edge::selected()
{
	return _selected;
}

bool Edge::operator==(const Edge& other) {
	return _start == other._start && _end == other._end || _start == other._end && _end == other._start;
}
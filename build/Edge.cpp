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
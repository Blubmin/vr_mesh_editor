#include "Mesh.h"

#include <vector>

#include "Vertex.h"
#include "Edge.h"
#include "Triangle.h"

using namespace glm;
using namespace std;

Mesh::Mesh()
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO_verts);
	glGenBuffers(1, &_VBO_indices);

	init_geom();
	bind_geom();
}

void Mesh::init_geom() {
	vector<vec3> cube_verts;

	cube_verts.emplace_back( 1,  1,  1); // 0
	cube_verts.emplace_back( 1,  1, -1); // 1
	cube_verts.emplace_back( 1, -1,  1); // 2
	cube_verts.emplace_back( 1, -1, -1); // 3
	cube_verts.emplace_back(-1,  1,  1); // 4
	cube_verts.emplace_back(-1,  1, -1); // 5
	cube_verts.emplace_back(-1, -1,  1); // 6
	cube_verts.emplace_back(-1, -1, -1); // 7

	create_tri(cube_verts[0], cube_verts[3], cube_verts[1]);
	create_tri(cube_verts[2], cube_verts[1], cube_verts[3]);

	create_tri(cube_verts[0], cube_verts[1], cube_verts[4]);
	create_tri(cube_verts[5], cube_verts[4], cube_verts[1]);

	create_tri(cube_verts[0], cube_verts[4], cube_verts[6]);
	create_tri(cube_verts[2], cube_verts[6], cube_verts[4]);

	create_tri(cube_verts[1], cube_verts[3], cube_verts[7]);
	create_tri(cube_verts[5], cube_verts[7], cube_verts[3]);

	create_tri(cube_verts[4], cube_verts[5], cube_verts[7]);
	create_tri(cube_verts[6], cube_verts[7], cube_verts[5]);

	create_tri(cube_verts[2], cube_verts[6], cube_verts[3]);
	create_tri(cube_verts[7], cube_verts[3], cube_verts[6]);
}

void Mesh::bind_geom() {
	glBindVertexArray(_VAO);

	_vert_pos.clear();
	for (auto v : _verts) {
		_vert_pos.push_back(v->get_pos());
	}

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_verts);
	glBufferData(GL_ARRAY_BUFFER, _vert_pos.size() * sizeof(vec3),
		_vert_pos.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	_indices.clear();
	for (auto t : _tris) {
		for (auto e : t->get_edges()) {
			auto pos = find(_verts.begin(), _verts.end(), e->get_start());
			auto idx = std::distance(_verts.begin(), pos);
			_indices.push_back(idx);
		}		
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		_indices.size() * sizeof(GLuint), _indices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mesh::vao()
{
	return _VAO;
}

GLuint Mesh::num_indices()
{
	return _indices.size();
}

Triangle* Mesh::create_tri(vec3 v1, vec3 v2, vec3 v3) {
	Vertex* vert1 = create_vert(v1);
	Vertex* vert2 = create_vert(v2);
	Vertex* vert3 = create_vert(v3);

	Edge* e1 = create_edge(v1, v2);
	Edge* e2 = create_edge(v2, v3);
	Edge* e3 = create_edge(v3, v1);

	Triangle* t = new Triangle(e1, e2, e3);

	vert1->add_edge(e1);
	vert1->add_edge(e2);
	vert1->add_edge(e3);
	vert1->add_triangle(t);

	vert2->add_edge(e1);
	vert2->add_edge(e2);
	vert2->add_edge(e3);
	vert2->add_triangle(t);

	vert3->add_edge(e1);
	vert3->add_edge(e2);
	vert3->add_edge(e3);
	vert3->add_triangle(t);

	e1->add_triangle(t);
	e2->add_triangle(t);
	e3->add_triangle(t);

	_tris.push_back(t);
	return t;
}

Edge * Mesh::create_edge(vec3 v1, vec3 v2)
{
	Edge* edge = nullptr;
	for (auto temp : _edges) {
		if (*temp->get_start() == v1 && *temp->get_end() == v2 ||
			*temp->get_start() == v2 && *temp->get_end() == v1) {
			edge = temp;
			break;
		}
	}
	if (edge == nullptr) {
		edge = new Edge(create_vert(v1), create_vert(v2));
		_edges.push_back(edge);
	}
	return edge;
}

Vertex* Mesh::create_vert(vec3 v) {
	Vertex* vert = nullptr;
	for (auto temp : _verts) {
		if (*temp == v) {
			vert = temp;
			break;
		}
	}
	if (vert == nullptr) {
		vert = new Vertex(v);
		_verts.push_back(vert);
	}
	return vert;
}

Mesh::~Mesh()
{
}

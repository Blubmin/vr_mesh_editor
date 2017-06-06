#include "Mesh.h"

#include <fstream>
#include <string>
#include <vector>

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "Edge.h"
#include "Quad.h"
#include "Triangle.h"
#include "Vertex.h"

using namespace glm;
using namespace std;

Mesh::Mesh()
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO_verts);
	glGenBuffers(1, &_VBO_selected);
	glGenBuffers(1, &_VBO_tri_indices);
	glGenBuffers(1, &_VBO_edge_indices);
	glGenBuffers(1, &_VBO_vert_indices);

	init_geom();
	bind_geom();
}

Mesh::Mesh(const string & filename)
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO_verts);
	glGenBuffers(1, &_VBO_selected);
	glGenBuffers(1, &_VBO_tri_indices);
	glGenBuffers(1, &_VBO_edge_indices);
	glGenBuffers(1, &_VBO_vert_indices);

	std::fstream hFile(filename.c_str(), std::ios::in | std::ios::binary);
	if (!hFile.is_open()) { throw std::invalid_argument("Error: File Not Found."); }
	
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	aiMesh* mesh = scene->mMeshes[0];

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D pos = mesh->mVertices[i];
		_verts.push_back(create_vert(vec3(pos.x, pos.y, pos.z)));
		_vert_selected.push_back(0);
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (int j = 0; j < face.mNumIndices; j++) {
			_tri_indices.push_back(face.mIndices[j]);
		}
	}

	bind_geom();
}

void Mesh::init_geom() {
	vector<vec3> cube_verts;

	cube_verts.emplace_back(1, 1, 1); // 0
	cube_verts.emplace_back(1, 1, -1); // 1
	cube_verts.emplace_back(1, -1, 1); // 2
	cube_verts.emplace_back(1, -1, -1); // 3
	cube_verts.emplace_back(-1, 1, 1); // 4
	cube_verts.emplace_back(-1, 1, -1); // 5
	cube_verts.emplace_back(-1, -1, 1); // 6
	cube_verts.emplace_back(-1, -1, -1); // 7

	create_tri(cube_verts[0], cube_verts[3], cube_verts[1]);
	create_tri(cube_verts[2], cube_verts[3], cube_verts[0]);

	create_tri(cube_verts[0], cube_verts[1], cube_verts[4]);
	create_tri(cube_verts[5], cube_verts[4], cube_verts[1]);

	create_tri(cube_verts[0], cube_verts[4], cube_verts[6]);
	create_tri(cube_verts[2], cube_verts[0], cube_verts[6]);

	create_tri(cube_verts[1], cube_verts[3], cube_verts[7]);
	create_tri(cube_verts[5], cube_verts[1], cube_verts[7]);

	create_tri(cube_verts[4], cube_verts[5], cube_verts[7]);
	create_tri(cube_verts[6], cube_verts[4], cube_verts[7]);

	create_tri(cube_verts[2], cube_verts[6], cube_verts[3]);
	create_tri(cube_verts[7], cube_verts[3], cube_verts[6]);

	/*create_quad(cube_verts[0], cube_verts[2], cube_verts[3], cube_verts[1]);
	create_quad(cube_verts[0], cube_verts[1], cube_verts[5], cube_verts[4]);
	create_quad(cube_verts[0], cube_verts[4], cube_verts[6], cube_verts[2]);
	create_quad(cube_verts[4], cube_verts[5], cube_verts[7], cube_verts[6]);
	create_quad(cube_verts[5], cube_verts[1], cube_verts[3], cube_verts[7]);
	create_quad(cube_verts[6], cube_verts[7], cube_verts[3], cube_verts[2]);*/

	_vert_pos.clear();
	for (auto v : _verts) {
		_vert_pos.push_back(v->get_pos());
	}

	_vert_selected.clear();
	for (auto v : _verts) {
		_vert_selected.push_back(v->selected());
	}

	_tri_indices.clear();
	for (auto t : _tris) {
		for (auto e : t->get_edges()) {
			auto pos = find(_verts.begin(), _verts.end(), t == e->tris()[0] ? e->get_start() : e->get_end());
			auto idx = std::distance(_verts.begin(), pos);
			_tri_indices.push_back(idx);
		}
	}

	_edge_indices.clear();
	for (auto e : _edges) {
		auto pos = find(_verts.begin(), _verts.end(), e->get_start());
		auto idx = std::distance(_verts.begin(), pos);
		_edge_indices.push_back(idx);

		pos = find(_verts.begin(), _verts.end(), e->get_end());
		idx = std::distance(_verts.begin(), pos);
		_edge_indices.push_back(idx);
	}

	_vert_indices.clear();
	for (int i = 0; i < _verts.size(); i++) {
		_vert_indices.push_back(i);
	}
}

void Mesh::bind_geom() {
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_verts);
	glBufferData(GL_ARRAY_BUFFER, _vert_pos.size() * sizeof(vec3),
		_vert_pos.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_selected);
	glBufferData(GL_ARRAY_BUFFER, _vert_selected.size() * sizeof(GLuint),
		_vert_selected.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_tri_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		_tri_indices.size() * sizeof(GLuint), _tri_indices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_edge_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		_edge_indices.size() * sizeof(GLuint), _edge_indices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_vert_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		_vert_indices.size() * sizeof(GLuint), _vert_indices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw_tris()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_tri_indices);
	glDrawElements(GL_TRIANGLES, _tri_indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw_edges()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_edge_indices);
	glDrawElements(GL_LINES, _edge_indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw_verts()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_vert_indices);
	glDrawElements(GL_POINTS, _vert_indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mesh::vao()
{
	return _VAO;
}

Quad* Mesh::create_quad(vec3 v1, vec3 v2, vec3 v3, vec3 v4) {
	Vertex* vert1 = create_vert(v1);
	Vertex* vert2 = create_vert(v2);
	Vertex* vert3 = create_vert(v3);
	Vertex* vert4 = create_vert(v4);

	Edge* e1 = create_edge(v1, v2);
	Edge* e2 = create_edge(v2, v3);
	Edge* e3 = create_edge(v3, v4);
	Edge* e4 = create_edge(v4, v1);

	Quad* q = new Quad();
	q->add_edge(e1);
	q->add_edge(e2);
	q->add_edge(e3);
	q->add_edge(e4);
	q->add_vertex(vert1);
	q->add_vertex(vert2);
	q->add_vertex(vert3);
	q->add_vertex(vert4);

	vert1->add_edge(e4);
	vert1->add_edge(e1);
	vert1->add_quad(q);

	vert2->add_edge(e1);
	vert2->add_edge(e2);
	vert2->add_quad(q);

	vert3->add_edge(e2);
	vert3->add_edge(e3);
	vert3->add_quad(q);

	vert4->add_edge(e3);
	vert4->add_edge(e4);
	vert4->add_quad(q);

	e1->add_quad(q);
	e2->add_quad(q);
	e3->add_quad(q);
	e4->add_quad(q);

	_quads.push_back(q);
	return q;
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

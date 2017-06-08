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
	if (!hFile.is_open()) { throw std::invalid_argument("Error: File " + filename + " Not Found."); }
	
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename,
        aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	aiMesh* mesh = scene->mMeshes[0];

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D pos = mesh->mVertices[i];
        _vert_pos.push_back(vec3(pos.x, pos.y, pos.z));
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

    update_positions();

    update_selected();

    update_indices();
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

    if (!_tri_indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_tri_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            _tri_indices.size() * sizeof(GLuint), _tri_indices.data(), GL_DYNAMIC_DRAW);
    }

    if (!_edge_indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_edge_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            _edge_indices.size() * sizeof(GLuint), _edge_indices.data(), GL_DYNAMIC_DRAW);
    }

    if (!_vert_indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBO_vert_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            _vert_indices.size() * sizeof(GLuint), _vert_indices.data(), GL_DYNAMIC_DRAW);
    }

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

void Mesh::select(glm::vec3 pos, float rad) {
    for (auto v : _verts) {
        if (glm::distance(pos, v->get_pos()) >= rad) continue;
        v->select();
    }

    for (auto e : _edges) {
        if (!(e->get_start()->selected() && e->get_end()->selected())) continue;
        e->select();
    }

    for (auto t : _tris) {
        bool selected = true;
        for (auto e : t->get_edges()) {
            selected &= e->selected();
        }
        if (!selected) continue;
        t->select();
    }

    update_selected();
}

void Mesh::update_selected() {
    _vert_selected.clear();
    for (auto v : _verts) {
        _vert_selected.push_back(v->selected());
    }
}

void Mesh::update_positions() {
    _vert_pos.clear();
    for (auto v : _verts) {
        _vert_pos.push_back(v->get_pos());
    }
}

void Mesh::update_indices() {
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

void Mesh::translate_selected(glm::vec3 dist) {
    for (auto v : _verts) {
        if (!v->selected()) continue;
        v->translate(dist);
    }
    update_positions();
}

void Mesh::scale_selected(float percent) {
    vec3 center;
    int num = 0;

    for (auto v : _verts) {
        if (!v->selected()) continue;
        center += v->get_pos();
        num++;
    }

    center /= (float) num;

    for (auto v : _verts) {
        if (!v->selected()) continue;
        v->translate((v->get_pos() - center) * percent);
    }

    update_positions();
}

void Mesh::extrude() {
    Mesh temp;
    vector<Edge*> horizon_e;
    vector<Triangle*> selected;

    for (auto e : _edges) {
        int selected = 0;
        for (auto t : e->tris()) {
            if (t->selected()) selected++;
        }
        if (selected == 1) horizon_e.push_back(e);
    }

    for (auto t : _tris) {
        if (t->selected()) selected.push_back(t);
    }

    for (auto e : horizon_e) {
        e->get_start()->deselect();
        e->get_end()->deselect();
        e->deselect();

        Vertex* start = temp.create_vert(e->get_start()->get_pos());
        start->select();
        Vertex* end = temp.create_vert(e->get_end()->get_pos());
        end->select();
        Edge* edge = temp.create_edge(start, end);
        edge->tris() = vector<Triangle*>(2);

        Triangle* t;
        Triangle* t1;
        Triangle* t2;
        Edge* left;
        Edge* diag;
        Edge* right;

        if (e->tris()[0]->selected()) {
            t = e->tris()[0];
            left = create_edge(start, e->get_start());
            diag = create_edge(e->get_start(), end);
            right = create_edge(e->get_end(), end);

            t1 = new Triangle(edge, left, diag);
            t2 = new Triangle(e, right, diag);
            
            edge->tris()[0] = t;
            edge->tris()[1] = t1;

            e->tris()[0] = t2;
        }
        else {
            t = e->tris()[1];
            left = create_edge(end, e->get_end());
            diag = create_edge(e->get_end(), start);
            right = create_edge(e->get_start(), start); 

            t1 = new Triangle(edge, left, diag);
            t2 = new Triangle(e, right, diag);

            edge->tris()[0] = t1;
            edge->tris()[1] = t;
            
            e->tris()[1] = t2;
        }

        t->replace(e, edge);

        left->tris().push_back(t1);

        diag->tris().push_back(t1);
        diag->tris().push_back(t2);

        right->tris().push_back(t2);

        
        auto pos = find(_verts.begin(), _verts.end(), start);
        if (pos == _verts.end()) _verts.push_back(start);
        pos = find(_verts.begin(), _verts.end(), end);
        if (pos == _verts.end()) _verts.push_back(end);

        _edges.push_back(edge);
        _tris.push_back(t1);
        _tris.push_back(t2);
    }

    update_positions();
    update_selected();
    update_indices();
}

void Mesh::deselect() {
    for (auto v : _verts) {
        v->deselect();
    }
    for (auto e : _edges) {
        e->deselect();
    }
    for (auto t : _tris) {
        t->deselect();
    }
    update_selected();
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

	Edge* e1 = create_edge(vert1, vert2);
	Edge* e2 = create_edge(vert2, vert3);
	Edge* e3 = create_edge(vert3, vert4);
	Edge* e4 = create_edge(vert4, vert1);

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

	Edge* e1 = create_edge(vert1, vert2);
	Edge* e2 = create_edge(vert2, vert3);
	Edge* e3 = create_edge(vert3, vert1);

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

Edge * Mesh::create_edge(Vertex* v1, Vertex* v2)
{
	Edge* edge = nullptr;
	for (auto temp : _edges) {
		if (temp->get_start() == v1 && temp->get_end() == v2 ||
			temp->get_start() == v2 && temp->get_end() == v1) {
			edge = temp;
			break;
		}
	}
	if (edge == nullptr) {
		edge = new Edge(v1, v2);
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

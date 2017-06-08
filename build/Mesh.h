#pragma once

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

class Edge;
class Quad;
class Triangle;
class Vertex;

class Mesh
{
	GLuint _VAO;
	GLuint _VBO_verts;
	GLuint _VBO_selected;
	GLuint _VBO_tri_indices;
	GLuint _VBO_edge_indices;
	GLuint _VBO_vert_indices;

	std::vector<glm::vec3> _vert_pos;
	std::vector<GLuint> _vert_selected;
	std::vector<GLuint> _tri_indices;
	std::vector<GLuint> _edge_indices;
	std::vector<GLuint> _vert_indices;

	std::vector<Vertex*> _verts;
	std::vector<Edge*> _edges;
	std::vector<Triangle*> _tris;
	std::vector<Quad*> _quads;
public:
	Mesh();
	Mesh(const std::string& filename);
	~Mesh();
	void init_geom();
	void bind_geom();
	void draw_tris();
	void draw_edges();
	void draw_verts();
    void select(glm::vec3 pos, float rad);
    void update_selected();
    void update_positions();
    void update_indices();
    void translate_selected(glm::vec3 dist);
    void scale_selected(float percent);
    void extrude();
    void deselect();
	GLuint vao();
	Quad * create_quad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	Triangle* create_tri(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
	Edge* create_edge(Vertex* v1, Vertex* v2);
	Vertex* create_vert(glm::vec3 v);
};


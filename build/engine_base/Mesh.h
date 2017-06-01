#pragma once

#include <vector>

#include <assimp\scene.h>
#include <GL\glew.h>
#include <glm\glm.hpp>

class Mesh
{
public:
    Mesh();
    Mesh(const aiMesh* scene);
    ~Mesh();    
    void bindBuffers();

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<GLuint> indices;
    unsigned int materialIdx;

    GLuint VAO;
    GLuint VBO_VERT;
    GLuint VBO_NORM;
    GLuint IND;

private:
    
};


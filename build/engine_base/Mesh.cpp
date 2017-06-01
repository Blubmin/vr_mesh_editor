#include "Mesh.h"

#include <climits>

using namespace glm;
using namespace std;

Mesh::Mesh()
{
}

Mesh::Mesh(const aiMesh* mesh)
{
    this->verts = vector<vec3>();

    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D pos = mesh->mVertices[i];
        this->verts.push_back(vec3(pos.x, pos.y, pos.z));

        aiVector3D norm = mesh->mNormals[i];
        this->norms.push_back(vec3(norm.x, norm.y, norm.z));
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    assert(indices.size() == mesh->mNumFaces * 3);
    materialIdx = mesh->mMaterialIndex;
}

Mesh::~Mesh()
{
}

void Mesh::bindBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO_VERT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_VERT);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(vec3),
		verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &VBO_NORM);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_NORM);
    glBufferData(GL_ARRAY_BUFFER, norms.size()*sizeof(vec3),
        norms.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &IND);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IND);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
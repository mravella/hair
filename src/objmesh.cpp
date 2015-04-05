#include "objmesh.h"
#include "errorchecker.h"
#include "objloader.hpp"

ObjMesh::ObjMesh()
{
}

void ObjMesh::init(const char *objFile)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool loaded = OBJLoader::loadOBJ(objFile, vertices, uvs, normals);

    if (!loaded) {
        printf("Failed to load OBJ: %s\n", objFile);
        exit(1);
    }

    // Initialize m_triangles
    for (unsigned int i=0; i < vertices.size()/3; i++) {
        unsigned int i1 = i*3, i2 = i1+1, i3=i2+1;
        Triangle t(vertices[i1], vertices[i2], vertices[i3],
                   uvs[i1], uvs[i2], uvs[i3],
                   normals[i1], normals[i2], normals[i3]);
        triangles.push_back(t);
    }

    // Initialize vbo
    std::vector<GLfloat> vboData;
    for (unsigned int i=0; i < vertices.size(); i++) {
        vboData.push_back(vertices[i].x);
        vboData.push_back(vertices[i].y);
        vboData.push_back(vertices[i].z);
        vboData.push_back(uvs[i].x);
        vboData.push_back(uvs[i].y);
        vboData.push_back(normals[i].x);
        vboData.push_back(normals[i].y);
        vboData.push_back(normals[i].z);
    }
    m_shape.create();
    m_shape.setVertexData(&vboData[0], sizeof(GLfloat) * vboData.size(), vertices.size());
    m_shape.setAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
    m_shape.setAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 3 * sizeof(GLfloat));
    m_shape.setAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 5 * sizeof(GLfloat));
}

void ObjMesh::draw()
{
    m_shape.draw(GL_TRIANGLES);
}

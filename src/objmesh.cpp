#include "objmesh.h"
#include "errorchecker.h"
#include "objloader.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/random.hpp>

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

/*
 * Shoot a ray from the point and if it intersects an even number of triangles, it is outside
 * If it intersects an odd number of triangles it is inside
 * TODO: Generate a random vector more elegantly
 */
bool ObjMesh::contains(glm::vec3 &normal, glm::vec3 ro)
{
    int numIntersections = 0;
    double rand1 = rand() % 1000; double rand2 = rand() % 1000; double rand3 = rand() % 1000;
    glm::vec3 randDir = glm::normalize(glm::vec3(rand1, rand2, rand3));
    randDir = glm::normalize(ro);

    for (unsigned int i = 0; i < triangles.size(); ++i)
    {
        Triangle currTriangle = triangles.at(i);
        glm::vec3 intersectionPoint = glm::vec3(0.0);

        if (intersect(intersectionPoint, ro, randDir, currTriangle))
        {
            normal = (currTriangle.n1 + currTriangle.n2 + currTriangle.n3) / 3.0f;
            numIntersections++;
        }
    }
    return (numIntersections % 2);
}

// Using Moller-Trumbore method for triangle-ray intersection
bool ObjMesh::intersect(glm::vec3 &intersection, glm::vec3 ro, glm::vec3 rd, Triangle tri)
{
    glm::vec3 edge1 = tri.v2 - tri.v1;
    glm::vec3 edge2 = tri.v3 - tri.v1;
    glm::vec3 pVec = glm::cross(rd, edge2);

    float det = glm::dot(edge1, pVec);
    if (det == 0) return false;
    float invDet = 1.0f / det;

    glm::vec3 tVec = ro - tri.v1;
    float u = glm::dot(tVec, pVec) * invDet;
    if (u < 0.0 || u > 1.0) return false;

    glm::vec3 qVec = glm::cross(tVec, edge1);
    float v = glm::dot(rd, qVec) * invDet;
    if (v < 0.0 || u + v > 1.0) return false;

    glm::vec3 intersectionPoint = tri.v1 + u * edge1 + v * edge2;
    float t = (intersectionPoint.x - ro.x) / rd.x;
    if (t < 0) return false;

    intersection = intersectionPoint;
    return true;
}

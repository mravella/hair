#include "octtreetriangle.h"

#include "objmesh.h"

OctTreeTriangle::OctTreeTriangle(ObjMesh *mesh)
{
    m_mesh = mesh;

    m_root = new OctTreeTriNode();

    calculateBoundingBox();

    for (int i = 0; i < m_mesh->triangles.size(); i++){

        insertTriangle(m_mesh->triangles.at(i));


    }

}

OctTreeTriangle::~OctTreeTriangle()
{
    delete m_root;
}


void OctTreeTriangle::calculateBoundingBox(){
    if (m_mesh == NULL || m_mesh->triangles.size() <= 0){
        cerr << "mesh is borked" << endl;
        return;
    }

    boundingMin = m_mesh->triangles.at(0).v1;
    boundingMax = m_mesh->triangles.at(0).v1;

    for (int i = 0; i < m_mesh->triangles.size(); i++){
        Triangle tri = m_mesh->triangles.at(i);
        glm::vec3 pt = tri.v1;
        for (int j = 0; j < 3; j++){
            if (j == 1) pt = tri.v2;
            if (j == 2) pt = tri.v3;

            if (pt.x < boundingMin.x) boundingMin.x = pt.x;
            if (pt.x > boundingMax.x) boundingMax.x = pt.x;


        }
    }
}


void OctTreeTriangle::insertTriangle(Triangle tri)
{
    OctTreeTriNode *current = m_root;




}

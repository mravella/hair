#ifndef OCTTREETRIANGLE_H
#define OCTTREETRIANGLE_H

#include "QList"

#include "hairCommon.h"


class ObjMesh;

struct OctTreeTriNode {

    OctTreeTriNode **children;
    QList<Triangle> triangles;

    OctTreeTriNode(){
        children = new OctTreeTriNode*[8];
        for (int i = 0; i < 8; i++){
            children[i] = NULL;
        }
    }

    ~OctTreeTriNode(){
        delete[] children;
    }


};

class OctTreeTriangle
{
public:
    OctTreeTriangle(ObjMesh *mesh);
    ~OctTreeTriangle();


    void calculateBoundingBox();
    void insertTriangle(Triangle tri);

public:
    ObjMesh *m_mesh;
    OctTreeTriNode *m_root;


    glm::vec3 boundingMin;
    glm::vec3 boundingMax;


};

#endif // OCTTREETRIANGLE_H

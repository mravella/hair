#ifndef OBJMESH_H
#define OBJMESH_H

#include "hairCommon.h"
#include "openglshape.h"

class ObjMesh
{
public:
    ObjMesh();

    void init(const char * objFile);

    void draw();

    bool contains(glm::vec3 ro);
    bool intersect(glm::vec3 &intersection, glm::vec3 ro, glm::vec3 rd, Triangle tri);

    std::vector<Triangle> triangles;

private:
    OpenGLShape m_shape;

};

#endif // OBJMESH_H

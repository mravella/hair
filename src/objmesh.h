#ifndef OBJMESH_H
#define OBJMESH_H

#include "hairCommon.h"
#include "openglshape.h"
#include <limits>

class ObjMesh
{
public:
    ObjMesh();

    void init(const char * objFile);

    void draw();

    bool contains(glm::vec3 &normal, glm::vec3 ro);

    std::vector<Triangle> triangles;

private:
    OpenGLShape m_shape;

    glm::vec3 m_min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_max = glm::vec3(std::numeric_limits<float>::min());

};

#endif // OBJMESH_H

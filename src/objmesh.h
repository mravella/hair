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

    std::vector<Triangle> triangles;

private:
    OpenGLShape m_shape;

};

#endif // OBJMESH_H

#ifndef OBJMESH_H
#define OBJMESH_H

#include "hairCommon.h"
#include "openglshape.h"
#include <limits>

class MeshOctTree;

class ObjMesh
{
public:
    ObjMesh();

    /**
     * @param objFile OBJ file to initialize mesh
     * @param scale Factor by which to scale the mesh during computations (i.e. collision detection)
     */
    void init(const char * objFile, float scale = 1);

    void draw();

    bool contains(glm::vec3 &normal, glm::vec3 ro);

    std::vector<Triangle> triangles;

private:
    OpenGLShape m_shape;

    glm::vec3 m_min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_max = glm::vec3(std::numeric_limits<float>::min());

    MeshOctTree *m_octTree;

};

#endif // OBJMESH_H

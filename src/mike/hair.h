#ifndef HAIR_H
#define HAIR_H

#include "hairCommon.h"

#include "openglshape.h"
#include "shaderprogram.h"


class Hair
{
public:
    Hair(int numSegments, double length, glm::vec3 location, glm::vec3 dir);

    virtual ~Hair();

    void update(float time);
    void paint(ShaderProgram &_program);


public:
    QList<HairVertex*> m_vertices;

    OpenGLShape m_patch;
    int m_numSegments;
    double m_length;
    glm::vec3 m_triangleFace[2];

};

#endif // HAIR_H

#ifndef QUAD_H
#define QUAD_H

#include "openglshape.h"

class Quad
{
public:
    Quad() { }

    virtual ~Quad();

    void init();

    void draw();

private:
    OpenGLShape m_shape;
};

#endif // QUAD_H

#include "quad.h"

Quad::~Quad()
{
    m_shape.destroy();
}

void Quad::init()
{
    GLfloat data[] = {-1, 1, 0, // position 1
                       0, 1,    // UV 1
                      -1,-1, 0,
                       0, 0,
                       1, 1, 0,
                       1, 1,
                       1,-1, 0,
                       1, 0};

    m_shape.create();
    m_shape.setVertexData(data, sizeof(data), 4);
    m_shape.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    m_shape.setAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
}

void Quad::draw()
{
    m_shape.draw(GL_TRIANGLE_STRIP);
}

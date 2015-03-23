#include "hairpatch.h"

HairPatch::HairPatch()
{ }

HairPatch::~HairPatch()
{
    m_patch.destroy();
}

void HairPatch::init()
{
    GLfloat data[] = {-.5, +.5, 0,
                      +.5, +.5, 0,
                      -.5, -.5, 0,
                      +.5, -.5, 0};
    m_patch.create();
    m_patch.setVertexData(data, sizeof(data), 4);
    m_patch.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void HairPatch::draw()
{
    m_patch.draw(GL_PATCHES);
}

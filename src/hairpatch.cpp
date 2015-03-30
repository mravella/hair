#include "hairpatch.h"

#include <math.h>
#include "resourceloader.h"

HairPatch::HairPatch()
{ }

HairPatch::~HairPatch()
{
    m_patch.destroy();
}

void HairPatch::init()
{
    m_fullProgram = ResourceLoader::createTessShaderProgram(
                ":/shaders/full.vert", ":/shaders/full.frag", //":/shaders/full.geom",
                ":/shaders/full.tcs", ":/shaders/full.tes");

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
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    m_patch.draw(GL_PATCHES);
}

// Fills in the hair data array with a sine curve that varies through time.
void HairPatch::testHairData(GLfloat *data, int numVertices, float time)
{
    for (int i = 0; i < numVertices; i++) {
        data[3*i]   = 0.1 * sin(8 * (time + (float)i / numVertices));
        data[3*i+1] = -1 + 2.f * i / numVertices;
        data[3*i+2] = 0;
    }
}

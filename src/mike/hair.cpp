#include "hair.h"
#include "errorchecker.h"

/*
 * @file hair.cpp
 *
 * Stores the vertices representing a single simulated hair
 */


Hair::Hair(int numSegments, double length, glm::vec3 location, glm::vec3 dir)
{
    if (numSegments < 1)
        cerr << "Number of hair segments should be at least 1" << endl;

    if (length <= 0)
        cerr << "Hair length should be positive" << endl;

    dir = glm::normalize(dir);

    m_numSegments = numSegments;
    m_length = length;

    double stepSize = (double) length / numSegments;
    for (int i = 0; i < numSegments + 1; ++i)
    {
        HairVertex *newVert = new HairVertex(glm::vec3(location.x, location.y, location.z) + dir * (float) (stepSize * i));
        if (i > 0)
        {
            HairVertex *oldVert = m_vertices.at(i - 1);
            double dot = CLAMP(glm::dot(oldVert->position - newVert->position, glm::vec3(0, -1, 0)), -1.0, 1.0);
            double det = CLAMP(glm::dot(oldVert->position - newVert->position, glm::vec3(1, 0, 0)), -1.0, 1.0);
            newVert->theta = -atan2(det, dot);
        }
        newVert->segLen = stepSize;
        m_vertices.append(newVert);
        cout << glm::to_string(m_vertices.last()->position) << endl;
        
    }


    GLfloat data[] = {-.5, +.5, 0,
                      +.5, +.5, 0,
                      -.5, -.5, 0,
                      +.5, -.5, 0};
    m_patch.create();
    m_patch.setVertexData(data, sizeof(data), 4);
    m_patch.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

Hair::~Hair()
{
    m_patch.destroy();
}


void Hair::update(float time)
{

}

void Hair::paint(ShaderProgram &_program)
{
    _program.uniforms.numHairVertices = MIN(m_vertices.size(), MAX_HAIR_VERTICES);
    for (int i = 0; i < _program.uniforms.numHairVertices; i++){
        _program.uniforms.vertexData[i] = m_vertices.at(i)->position;
    }
    _program.setPerGuideHairUniforms();

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    m_patch.draw(GL_PATCHES);

}



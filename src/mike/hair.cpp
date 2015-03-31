#include "hair.h"

/*
 * @file hair.cpp
 *
 * Stores the vertices representing a single simulated hair
 */


Hair::Hair(int numSegments, double length, glm::vec3 location)
{
    if (numSegments < 2)
        cerr << "Number of hair segments should be at least 2" << endl;

    if (length <= 0)
        cerr << "Hair length should be positive" << endl;

    m_numSegments = numSegments;
    m_length = length;

    // TEMPORARY: Add all vertices directly below m_location
    double stepSize = (double) length / numSegments;
    for (int i = 0; i < numSegments + 1; ++i)
    {
        m_vertices.append(new HairVertex(glm::vec3(location.x, location.y - stepSize * i, location.z)));

        if (i != 0) // joints are halfway between vertices
        {
            glm::vec3 _pos = (m_vertices.at(i)->position + m_vertices.at(i-1)->position)/2.0f;
            m_joints.append(new Joint(_pos));
        }
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

void Hair::paint(GLuint _program)
{

    GLfloat _data[3*m_vertices.size()];
    for (int i = 0; i < m_vertices.size(); i++){
        _data[3*i] = m_vertices.at(i)->position.x;
        _data[3*i+1] = m_vertices.at(i)->position.y;
        _data[3*i+2] = m_vertices.at(i)->position.z;
    }

    glUniform3f(glGetUniformLocation(_program, "color"), .6f, .4f, .3f);
    glUniform3fv(glGetUniformLocation(_program, "vertexData"), m_vertices.size(), _data);
    glUniform1i(glGetUniformLocation(_program, "numHairSegments"), m_vertices.size()+1);
    glUniform1i(glGetUniformLocation(_program, "numPatchHairs"), 20);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    m_patch.draw(GL_PATCHES);

}



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
    m_vertices = new HairVertex[numSegments + 1];

    // TEMPORARY: Add all vertices directly below m_location
    double stepSize = (double) numSegments / length;
    for (int i = 0; i < numSegments + 1; ++i)
    {
        m_vertices[i] = HairVertex(glm::vec3(location.x, location.y + stepSize * i, location.z));
    }
}

Hair::~Hair()
{
    delete m_vertices;
}

int Hair::getNumSegments(){ return m_numSegments;}

double Hair::getLength(){ return m_length;}

HairVertex Hair::getVertexAt(int i){ return m_vertices[i];}

HairVertex* Hair::getVertices() { return m_vertices;}




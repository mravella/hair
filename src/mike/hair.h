#ifndef HAIR_H
#define HAIR_H

#include "hairCommon.h"

class Hair
{
public:
    Hair(int numSegments, double length, glm::vec3 location);

    virtual ~Hair();

    // Returns the number of segments for the hair (numVertices - 1)
    int getNumSegments();

    // Returns the length of the hair
    double getLength();

    // Returns the location of the hair vertex at index i
    HairVertex getVertexAt(int i);

    // Returns QVector of all the vertices making up the hair
    HairVertex* getVertices();


private:
    int m_numSegments;
    double m_length;

    HairVertex *m_vertices;
};

#endif // HAIR_H

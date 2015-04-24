#ifndef TESSELLATOR_H
#define TESSELLATOR_H

#include "hairCommon.h"

class ShaderProgram;

class Tessellator
{
public:
    Tessellator();

    virtual ~Tessellator();

    void init(int numLines);

    bool setNumLines(int numLines);

    void beginTessellation();

    void endTessellation();

    void draw();

    ShaderProgram *program;

private:
    int m_numLines = 0;
    GLuint m_primitivesQuery;
    GLuint m_vaoID = 0;
    GLuint m_bufferID = 0;
};

#endif // TESSELLATOR_H

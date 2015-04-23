#include "tessellator.h"

#include "hairfeedbackshaderprogram.h"
#include "resourceloader.h"
#include "errorchecker.h"

Tessellator::Tessellator()
{
    program = new HairFeedbackShaderProgram();
}

Tessellator::~Tessellator()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_bufferID);
    glDeleteQueries(1, &m_primitivesQuery);
    safeDelete(program);
}

void Tessellator::init(int numLines)
{
    // Initialize tessellation shader program.
    program->create();

    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_bufferID);
    glGenQueries(1, &m_primitivesQuery);

    setNumLines(numLines);

    // Enable position and tangent attributes.
    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Tessellator::setNumLines(int numLines)
{
    if (numLines != m_numLines)
    {
        m_numLines = numLines;
        int bufferSize = numLines
                * 2 // vertices per line
                * 2 // attributes per vertex (position and tangent)
                * 3 // floats per attribute (x, y, z)
                * sizeof(GLfloat);

        // Re-initialize transform feedback buffer.
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }
    return false;
}

void Tessellator::beginTessellation()
{
    program->bind();
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_bufferID);

    // Keep track of the number of primitives written.
//    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_primitivesQuery); //TODO?

    glBeginTransformFeedback(GL_LINES);
}

void Tessellator::endTessellation()
{
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    program->unbind();
    glFlush();
}

void Tessellator::draw()
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_LINES, 0, m_numLines * 2);
    glBindVertexArray(0);
}

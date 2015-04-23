#include "openglshape.h"
#include <iostream>
#include <stdio.h>

void OpenGLShape::create()
{
    // Generates a buffer and a vertex array for this shape.
    glGenBuffers(1, &m_bufferID);
    glGenVertexArrays(1, &m_vaoID);

    m_created = true;
}

void OpenGLShape::destroy()
{
    // Deletes the buffer and vertex array.
    glDeleteBuffers(1, &m_bufferID);
    glDeleteVertexArrays(1, &m_vaoID);
}

bool OpenGLShape::checkIfCreated()
{
    if (!m_created) printf("Tried to use OpenGLShape before calling create().\n");
    return m_created;
}

void OpenGLShape::setVertexData(float *data, int size, int numVertices)
{
    if (!checkIfCreated()) return;

    m_numVertices = numVertices;

    // Stores vertex data in buffer.
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLShape::setAttribute(
        GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset)
{
    if (!checkIfCreated()) return;

    // Bind vertex array and buffer.
    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

    // Defines the specified vertex attribute with respect to the buffer data.
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, (void*) offset);

    // Unbind vertex array and buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLShape::draw(GLenum drawMode)
{
    if (!checkIfCreated()) return;

    glBindVertexArray(m_vaoID);
    glDrawArrays(drawMode, 0, m_numVertices);
    glBindVertexArray(0);
}

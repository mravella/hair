#ifndef OPENGLSHAPE_H
#define OPENGLSHAPE_H

#include "GL/glew.h"

class OpenGLShape
{
public:
    OpenGLShape() : m_created(false) { }

    virtual ~OpenGLShape() { }

    /** Creates a new buffer and vertex array on the GPU. */
    void create();

    /** Deletes the buffer and vertex array from the GPU. */
    void destroy();

    /** Initialize the buffer with the given vertex data. */
    void setVertexData(float *data, int size, int numVertices);

    /** Enables the specified attribute and calls glVertexAttribPointer with the given arguments. */
    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized,
                      GLsizei stride, size_t offset);

    /** Draw the initialized geometry. */
    void draw(GLenum drawMode);

private:
    bool checkIfCreated();

    bool m_created;

    GLuint m_bufferID; /// ID of the vertex buffer
    GLuint m_vaoID;    /// ID of the vertex array object (VAO)
    int m_numVertices; /// Number of vertices to be drawn.
};

#endif // OPENGLSHAPE_H

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "hairCommon.h"

class Framebuffer
{
public:
    void create();

    void bind(GLenum target = GL_FRAMEBUFFER);

    void unbind(GLenum target = GL_FRAMEBUFFER);

    void attachTextures(std::vector<GLuint> textureIDs, GLenum target = GL_FRAMEBUFFER);

private:
    GLuint m_id;
};

#endif // FRAMEBUFFER_H

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "hairCommon.h"

class Texture;

class Framebuffer
{
public:
    virtual ~Framebuffer();

    void create();

    void bind(GLenum target = GL_FRAMEBUFFER);

    void unbind(GLenum target = GL_FRAMEBUFFER);

    void attachColorTexture(GLuint textureID, GLenum target = GL_FRAMEBUFFER);

    void attachColorTextures(std::vector<GLuint> textureIDs, GLenum target = GL_FRAMEBUFFER);

    void attachDepthTexture(GLuint textureID, GLenum target = GL_FRAMEBUFFER);

    void generateColorTexture(int width, int height, GLint magFilter, GLint minFilter);

    void generateDepthTexture(int width, int height, GLint magFilter, GLint minFilter);

    void generateDepthBuffer(int width, int height);

    void resizeDepthBuffer(int width, int height);

    Texture *colorTexture = NULL;
    Texture *depthTexture = NULL;

private:
    GLuint m_id;

    GLuint m_depthBufferID = 0;
};

#endif // FRAMEBUFFER_H

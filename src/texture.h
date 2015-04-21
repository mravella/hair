#ifndef TEXTURE_H
#define TEXTURE_H

#include "hairCommon.h"

class Quad;
class TexturedQuadShaderProgram;

class Texture
{
public:
    Texture();

    virtual ~Texture();

    // Creates a texture containing the given image.
    void createColorTexture(const char *imageFile, GLint magFilter, GLint minFilter);

    // Creates a black texture with the given width and height.
    void createColorTexture(int width, int height, GLint magFilter, GLint minFilter);

    void createDepthTexture(int width, int height, GLint magFilter, GLint minFilter);

    // Resizes the texture and sets it to black.
    void resize(int width, int height);

    void bind(GLenum textureUnit);

    void unbind(GLenum textureUnit);

    // Renders a full-screen quad of the texture.
    void renderFullScreen();

    int width();
    int height();

    GLuint id;

private:
    void _create(const GLvoid * data,
            GLint internalFormat,
            int width,
            int height,
            GLenum format,
            GLenum type,
            GLint magFilter,
            GLint minFilter);

    // Parameters for glTexImage2D
    int m_width, m_height;
    GLint m_internalFormat;
    GLenum m_format;
    GLenum m_type;

    // For full-screen rendering
    Quad *m_quad;
    TexturedQuadShaderProgram *m_program;
};

#endif // TEXTURE_H

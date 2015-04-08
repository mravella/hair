#ifndef TEXTURE_H
#define TEXTURE_H

#include "hairCommon.h"

class Texture
{
public:
    // Creates a texture containing the given image.
    void create(QImage &image, GLint magFilter, GLint minFilter);

    // Creates a black texture with the given width and height.
    void createColorTexture(int width, int height, GLint magFilter, GLint minFilter);

    void createDepthTexture(int width, int height);

    void bind(unsigned int textureUnit);

    void unbind(unsigned int textureUnit);

    // Renders a full-screen quad of the texture.
    void renderFullScreen();

    GLuint id;

private:
    void _create(
            const GLvoid * data,
            GLint format,
            int width,
            int height,
            GLenum type,
            GLint magFilter,
            GLint minFilter);

};

#endif // TEXTURE_H

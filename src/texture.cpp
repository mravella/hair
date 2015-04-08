#include "texture.h"

#include "texturedquadshaderprogram.h"
#include "quad.h"

void Texture::create(QImage &image, GLint magFilter, GLint minFilter)
{
    _create(image.bits(), GL_RGBA, image.width(), image.height(), GL_UNSIGNED_BYTE,
            magFilter, minFilter);
}

void Texture::createColorTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    _create(0, GL_RGBA, width, height, GL_UNSIGNED_BYTE, magFilter, minFilter);
}

void Texture::createDepthTexture(int width, int height)
{
    _create(0, GL_DEPTH_COMPONENT, width, height, GL_FLOAT, GL_NEAREST, GL_NEAREST);
}

void Texture::_create(
        const GLvoid *data, GLint format, int width, int height, GLenum type,
        GLint magFilter, GLint minFilter)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int textureUnit)
{
    if (textureUnit > 32) {
        std::cout << "Error: Tried to bind GL_TEXTURE" << textureUnit << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
    glActiveTexture(GL_TEXTURE0);
}

void Texture::unbind(unsigned int textureUnit)
{
    if (textureUnit > 32) {
        std::cout << "Error: Tried to unbind GL_TEXTURE" << textureUnit << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

void Texture::renderFullScreen()
{
    Quad quad;
    TexturedQuadShaderProgram program;
    quad.init();
    program.create();
    program.bind();
    bind(0);
    glUniform1i(glGetUniformLocation(program.id, "tex"), 0);
    quad.draw();
    unbind(0);
    program.unbind();
}

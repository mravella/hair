#include "texture.h"

void Texture::create(QImage &image, GLint magFilter, GLint minFilter)
{
    create(image.bits(), image.width(), image.height(), magFilter, minFilter);
}

void Texture::create(int width, int height, GLint magFilter, GLint minFilter)
{
    create(0, width, height, magFilter, minFilter);
}

void Texture::create(const GLvoid *data, int width, int height, GLint magFilter, GLint minFilter)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

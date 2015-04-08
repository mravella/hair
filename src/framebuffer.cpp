#include "framebuffer.h"

void Framebuffer::create()
{
    glGenFramebuffers(1, &m_id);
}

void Framebuffer::bind(GLenum target)
{
    glBindFramebuffer(target, m_id);
}

void Framebuffer::unbind(GLenum target)
{
    glBindFramebuffer(target, 0);
}

void Framebuffer::attachTextures(std::vector<GLuint> textureIDs, GLenum target)
{
    bind(target);

    int numTextures = textureIDs.size();
    GLenum drawBuffers[numTextures];
    for (int i = 0; i < numTextures; i++)
    {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        glFramebufferTexture(target, GL_COLOR_ATTACHMENT0 + i, textureIDs[i], 0);
    }
    glDrawBuffers(numTextures, drawBuffers);

    GLenum framebufferStatus = glCheckFramebufferStatus(target);
    if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete. Status: " << framebufferStatus << std::endl;

    unbind(target);
}

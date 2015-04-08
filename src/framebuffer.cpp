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

    // Attach textures sequentially to the color attachments.
    int numTextures = textureIDs.size();
    GLenum drawBuffers[numTextures];
    for (int i = 0; i < numTextures; i++)
    {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        glFramebufferTexture(target, GL_COLOR_ATTACHMENT0 + i, textureIDs[i], 0);
    }
    glDrawBuffers(numTextures, drawBuffers);

    // Check framebuffer status.
    GLenum framebufferStatus = glCheckFramebufferStatus(target);
    if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete. Status: " << framebufferStatus << std::endl;

    unbind(target);
}

void Framebuffer::generateDepthBuffer(int width, int height)
{
    GLuint depthbuffer;
    bind();
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
    unbind();
}
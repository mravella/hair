#include "framebuffer.h"

#include "texture.h"

Framebuffer::~Framebuffer()
{
    safeDelete(colorTexture);
    safeDelete(depthTexture);
}

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

void Framebuffer::attachColorTexture(GLuint textureID, GLenum target)
{
    std::vector<GLuint> textureIDs = {textureID};
    attachColorTextures(textureIDs, target);
}

void Framebuffer::attachColorTextures(std::vector<GLuint> textureIDs, GLenum target)
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

void Framebuffer::attachDepthTexture(GLuint textureID, GLenum target)
{
    bind(target);
    glFramebufferTexture(target, GL_DEPTH_ATTACHMENT, textureID, 0);
    unbind(target);
}

void Framebuffer::generateColorTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    safeDelete(colorTexture);
    colorTexture = new Texture();
    colorTexture->createColorTexture(width, height, magFilter, minFilter);
    attachColorTexture(colorTexture->id);
}

void Framebuffer::generateDepthTexture(int width, int height, GLint magFilter, GLint minFilter)
{
    safeDelete(depthTexture);
    depthTexture = new Texture();
    depthTexture->createDepthTexture(width, height, magFilter, minFilter);
    attachDepthTexture(depthTexture->id);
}

void Framebuffer::generateDepthBuffer(int width, int height)
{
    bind();
    glGenRenderbuffers(1, &m_depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);
    unbind();
}

void Framebuffer::resizeDepthBuffer(int width, int height)
{
    bind();
    glDeleteRenderbuffers(1, &m_depthBufferID);
    generateDepthBuffer(width, height);
    unbind();
}

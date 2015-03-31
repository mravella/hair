#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "hairCommon.h"
#include <map>
#include <string>

#define MAX_HAIR_VERTICES 64

struct Uniforms {
    glm::mat4 model, view, projection;
    int numGroupHairs;
    int numHairVertices;
    glm::vec3 vertexData[MAX_HAIR_VERTICES];
    float hairGroupWidth;
    glm::vec3 color;
};

class ShaderProgram
{
public:
    ShaderProgram();

    void create();

    void bind();

    void unbind();

    // Sends all the uniforms to the shader program. Should be called after binding and before
    // drawing.
    void setUniforms();

    Uniforms uniforms;

private:
    GLuint m_id;
    std::map<std::string, int> m_uniformLocs;
};

#endif // SHADERPROGRAM_H

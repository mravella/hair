#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "hairCommon.h"
#include <map>
#include <string>

#define MAX_HAIR_VERTICES 64

struct Uniforms {
    glm::mat4 model, view, projection;

    // Number of vertices in the current guide hair.
    int numHairVertices;

    // Vertex position data for the current guide hair.
    glm::vec3 vertexData[MAX_HAIR_VERTICES];

    // Number of duplicates rendered with single-hair interpolation for each guide hair.
    int numGroupHairs;

    // Number of vertices rendered with a spline.
    int numSplineVertices;

    // Defines the width that a hair group is spread across.
    float hairGroupWidth;

    // The radius of a single hair.
    float hairRadius;

    // Exponent used for tapering the hair. The higher the exponent, the closer to the end the
    // tapering occurs.
    float taperExponent;

    // Hair color.
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

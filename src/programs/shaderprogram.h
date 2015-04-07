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
    float hairGroupSpread;

    // The radius of a single hair.
    float hairRadius;

    // Exponent used for tapering the hair. The higher the exponent, the closer to the end the
    // tapering occurs.
    float taperExponent;

    // Controls mount of noise in interpolated hairs. Should probably be in range [0, 0.1]
    float noiseAmplitude;

    // Basis vectors for the plane orthogonal to the hair's normal vector.
    glm::vec3 triangleFace[2];

    // Hair color.
    glm::vec3 color;
};

class ShaderProgram
{
public:
    ShaderProgram() { }

    virtual ~ShaderProgram() { }

    void bind();

    void unbind();

    virtual void create() = 0;

    // Sends all global uniforms to the shader program.
    virtual void setGlobalUniforms() = 0;

    // Sends all hair object-specific uniforms to the shader program.
    virtual void setPerHairObjectUniforms() = 0;

    // Sends all guide hair-specific uniforms to the shader program.
    virtual void setPerGuideHairUniforms() = 0;

    Uniforms uniforms;

protected:
    void getUniformLocations(std::vector<GLchar const *> &uniformNames);

    void setUniform1i(GLchar const *name, int value);
    void setUniform1f(GLchar const *name, float value);
    void setUniform3f(GLchar const *name, glm::vec3 &value);
    void setUniform3fv(GLchar const *name, GLsizei count, glm::vec3 *values);
    void setUniformMatrix4f(GLchar const *name, glm::mat4 &value);

    std::map<std::string, int> m_uniformLocs;

    GLuint m_id;

};

#endif // SHADERPROGRAM_H

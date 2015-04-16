#include "meshshaderprogram.h"

#include "resourceloader.h"
#include <vector>

GLuint MeshShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(":/shaders/mesh.vert", ":/shaders/mesh.frag");
}

void MeshShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("projection", uniforms.projection);
    setUniformMatrix4f("view", uniforms.view);
    setUniformMatrix4f("eyeToLight", uniforms.eyeToLight);
    setUniform1i("hairShadowMap", uniforms.hairShadowMap);
    setUniform1i("meshShadowMap", uniforms.meshShadowMap);
    setUniform1i("opacityMap", uniforms.opacityMap);
    setUniform3f("lightPosition", uniforms.lightPosition);
    setUniform1f("shadowIntensity", uniforms.shadowIntensity);
    setUniform1i("useShadows", uniforms.useShadows);
}

void MeshShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);
}

#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include <vector>

class ResourceLoader
{
public:
    ResourceLoader();
    static GLuint createBasicShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path);

    static GLuint createGeomShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * geom_file_path);

    static GLuint createTessShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path);

    static GLuint createFullShaderProgram(
            const char * vertex_file_path,
            const char * fragment_file_path,
            const char * geom_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path);

    static void initializeGlew();

private:
    static std::string readShaderFile(std::string filepath, int &additionalLines);
    static GLuint createShader(GLenum shaderType, const char *filepath);
    static GLuint createProgramFromShaders(std::vector<GLuint> &shaders);
};

#endif // SHADER_H

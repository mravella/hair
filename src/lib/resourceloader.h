#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include "hairCommon.h"
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

    static GLuint createTessFeedbackShaderProgram(
            const char * vertex_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path,
            const char ** varyings, int numVaryings);

    static GLuint createFullFeedbackShaderProgram(
            const char * vertex_file_path,
            const char * geom_file_path,
            const char * tess_control_file_path,
            const char * tess_eval_file_path,
            const char ** varyings, int numVaryings);

    static void initializeGlew();

private:
    static std::string _readShaderFile(std::string filepath, int &additionalLines);
    static GLuint _createShader(GLenum shaderType, const char *filepath);
    static GLuint _createFeedbackProgramFromShaders(std::vector<GLuint> &shaders, const GLchar **varyings, int numVaryings);
    static GLuint _createProgramFromShaders(std::vector<GLuint> &shaders);

    static void _attachShaders(GLuint program, std::vector<GLuint> &shaders);
    static void _linkProgram(GLuint program);
    static void _deleteShaders(std::vector<GLuint> &shaders);
};

#endif // SHADER_H

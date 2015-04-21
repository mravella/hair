#include "resourceloader.h"
#include "errorchecker.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "QStringList"

ResourceLoader::ResourceLoader()
{
}

GLuint ResourceLoader::createFullFeedbackShaderProgram(
        const char *vertexFilePath,
        const char *geomFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath,
        const char **varyings,
        int numVaryings)
{
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_GEOMETRY_SHADER, geomFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return _createFeedbackProgramFromShaders(shaders, varyings, numVaryings);
}

GLuint ResourceLoader::createFullShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_GEOMETRY_SHADER, geomFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };

    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createGeomShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_GEOMETRY_SHADER, geomFilePath)
    };
    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createTessShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        _createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        _createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return _createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createBasicShaderProgram(const char *vertexFilePath,const char *fragmentFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        _createShader(GL_VERTEX_SHADER, vertexFilePath),
        _createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
    };
    return _createProgramFromShaders(shaders);
}

void ResourceLoader::_attachShaders(GLuint program, std::vector<GLuint> &shaders)
{
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glAttachShader(program, shaderID);
    }
}

void ResourceLoader::_linkProgram(GLuint program)
{
    printf("Linking shaders... \n");

    // Link program.
    glLinkProgram(program);

    // Print the info log.
    GLint result;
    GLint infoLogLength;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog( std::max(infoLogLength, int(1)) );
        glGetProgramInfoLog(program, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // Exit if shader program not linked.
        if (result == GL_FALSE) exit(1);
    }
}

void ResourceLoader::_deleteShaders(std::vector<GLuint> &shaders)
{
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glDeleteShader(shaderID);
    }
}

GLuint ResourceLoader::_createProgramFromShaders(std::vector<GLuint> &shaders)
{
    GLuint programId = glCreateProgram();
    _attachShaders(programId, shaders);
    _linkProgram(programId);
    _deleteShaders(shaders);
    return programId;
}

GLuint ResourceLoader::_createFeedbackProgramFromShaders(
        std::vector<GLuint> &shaders, const GLchar **varyings, int numVaryings)
{
    GLuint programId = glCreateProgram();
    _attachShaders(programId, shaders);
    glTransformFeedbackVaryings(programId, numVaryings, varyings, GL_INTERLEAVED_ATTRIBS);
    _linkProgram(programId);
    _deleteShaders(shaders);
    return programId;
}

std::string ResourceLoader::_readShaderFile(std::string filepath, int &additionalLines)
{
    additionalLines = 0;
    std::string text;
    QFile file(QString::fromStdString(filepath));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);

        while(!stream.atEnd())
        {
            QString line = stream.readLine();

            if (line.startsWith("#include "))
            {
                QString includeFile = line.split(" ").at(1);
                includeFile = includeFile.remove( QRegExp("^[\"]*") ).remove( QRegExp("[\"]*$") );
                includeFile = ":/shaders/" + includeFile;
                int throwaway;
                line = QString::fromStdString(_readShaderFile(includeFile.toStdString(), throwaway));
                additionalLines += line.split("\n").size() - 1;
            }

            text += line.toStdString() + "\n";
        }
    } else {
        std::cout << "Could not open file: " << filepath << std::endl;
        exit(1);
    }
    return text;
}

GLuint ResourceLoader::_createShader(GLenum shaderType, const char *filepath)
{
    GLuint shaderID = glCreateShader(shaderType);

    // Compile shader code.
    printf("Compiling shader: %s\n", filepath);
    int additionalLines;
    std::string code = _readShaderFile(filepath, additionalLines);
    const char *codePtr = code.c_str();
    glShaderSource(shaderID, 1, &codePtr, NULL);
    glCompileShader(shaderID);

    // Print info log.
    GLint result;
    GLint infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // Exit if shader not compiled.
        if (result == GL_FALSE)
        {
            fprintf(stdout, "%d additional lines from included files\n", additionalLines);
            exit(1);
        }
    }

    return shaderID;
}

void ResourceLoader::initializeGlew()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // Check OpenGL errors but ignore GL_INVALID_ENUM, which is caused by glewExperimental.
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        if (err == GL_INVALID_ENUM) continue;
        printf("OpenGL error in ResourceLoader::initializeGlew(): %d\n", err);
    }}

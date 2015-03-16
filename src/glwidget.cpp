#include "glwidget.h"
#include "resourceloader.h"
#include "errorchecker.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent), m_timer(this), m_fps(60.f), m_increment(0)
{
    // Set up 60 FPS draw loop.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    m_timer.start(1000.0f / m_fps);
}

GLWidget::~GLWidget()
{
    m_triangle.destroy();
}

void GLWidget::initializeGL()
{
    ResourceLoader::initializeGlew();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Initialize shader programs.
    m_basicProgram = ResourceLoader::createBasicShaderProgram(
                ":/shaders/basic.vert", ":/shaders/basic.frag");
    m_fullProgram = ResourceLoader::createFullShaderProgram(
                ":/shaders/full.vert",
                ":/shaders/full.frag",
                ":/shaders/full.geom",
                ":/shaders/full.tcs",
                ":/shaders/full.tes");
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    // Initialize triangle.
    GLfloat triangleData[] = {-.5f, -.5f, 0.f,  // position 1
                               0.f,  0.f, 1.f,  // normal 1
                              +.5f, -.5f, 0.f,  // position 2
                               0.f,  0.f, 1.f,  // normal 2
                               0.f,  .5f, 0.f,  // position 3
                               0.f,  0.f, 1.f}; // normal 3
    m_triangle.create();
    m_triangle.setVertexData(triangleData, sizeof(triangleData), 3);
    m_triangle.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    m_triangle.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 3*sizeof(float));

    ErrorChecker::printGLErrors("end of initializeGL");
}

void GLWidget::paintGL()
{
    ErrorChecker::printGLErrors("start of paintGL");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = m_increment++ / (float) m_fps;      // Time in seconds.

    float fieldOfViewY = 0.8f;                       // Vertical field of view angle, in radians.
    float aspectRatio = (float)width() / height();   // Aspect ratio of the window.
    float nearClipPlane = 0.1f;                      // Near clipping plane.
    float farClipPlane = 100.f;                      // Far clipping plane.
    glm::vec3 eye = glm::vec3(0.f, 0.f, 4.f);        // Camera position.
    glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);     // Where camera is looking.
    glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);         // Up direction.

    // Model, view, and projection matrices
    glm::mat4 proj = glm::perspective(fieldOfViewY, aspectRatio, nearClipPlane, farClipPlane);
    glm::mat4 view = glm::lookAt(eye, center, up);
    glm::mat4 model(1.f);

    glUseProgram(m_basicProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_basicProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(m_basicProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_basicProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_basicProgram, "color"), 0.f, 1.f, 1.f);

    m_triangle.draw(GL_TRIANGLES);

    glUseProgram(0);
}

/** Repaints the canvas. Called 60 times per second. */
void GLWidget::tick()
{
    update();
}

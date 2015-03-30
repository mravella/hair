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
    m_fullProgram = ResourceLoader::createTessShaderProgram(
                ":/shaders/full.vert", ":/shaders/full.frag", //":/shaders/full.geom",
                ":/shaders/full.tcs", ":/shaders/full.tes");

    m_hairPatch.init();

    ErrorChecker::printGLErrors("end of initializeGL");
}

void GLWidget::paintGL()
{
    ErrorChecker::printGLErrors("start of paintGL");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = m_increment++ / (float) m_fps;      // Time in seconds.

    // Model, view, and projection matrices
    glm::mat4 proj = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(
                glm::vec3(0.f, 0.f, 4.f),  // eye
                glm::vec3(0.f, 0.f, 0.f),  // center
                glm::vec3(0.f, 1.f, 0.f)); // up
    glm::mat4 model(1.f);

    // Generates an array of 50 hair vertex positions, which are sent to the shaders below.
    int numVertices = 50;
    GLfloat hairData[3 * numVertices];
    HairPatch::testHairData(hairData, numVertices, time);

    glUseProgram(m_fullProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_fullProgram, "color"), .6f, .4f, .3f);
    glUniform3fv(glGetUniformLocation(m_fullProgram, "vertexData"), numVertices, hairData);
    glUniform1i(glGetUniformLocation(m_fullProgram, "numHairSegments"), numVertices + 1);
    glUniform1i(glGetUniformLocation(m_fullProgram, "numPatchHairs"), 20);

    m_hairPatch.draw();

    glUseProgram(0);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

/** Repaints the canvas. Called 60 times per second. */
void GLWidget::tick()
{
    update();
}

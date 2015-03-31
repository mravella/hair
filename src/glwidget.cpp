#include "glwidget.h"
#include "resourceloader.h"
#include "errorchecker.h"
#include "hairCommon.h"

#include "hairobject.h"
#include "simulation.h"

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

    m_testSimulation = new Simulation();
    m_hairObject = new HairObject(1, m_testSimulation);


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


    glUseProgram(m_fullProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_fullProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


    m_testSimulation->update(time);
    m_hairObject->update(time);
    m_hairObject->paint(m_fullProgram);


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

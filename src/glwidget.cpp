#include "glwidget.h"
#include "resourceloader.h"
#include "errorchecker.h"
#include "hairCommon.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include "hairobject.h"
#include "simulation.h"
#include "hairshaderprogram.h"
#include "meshshaderprogram.h"
#include "hairinterface.h"
#include "texture.h"
#include "framebuffer.h"

#define _USE_MESH_ true

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
      m_hairDensity(40),
      m_timer(this),
      m_increment(0),
      m_targetFPS(60.f)
{
    m_mesh = NULL;
    m_hairObject = NULL;
    m_testSimulation = NULL;
    m_hairProgram = new HairShaderProgram();
    m_meshProgram = new MeshShaderProgram();
    m_noiseTexture = new Texture();
    m_shadowDepthTexture = new Texture();
    m_shadowFramebuffer = new Framebuffer();
    m_hairInterface->setGLWidget(this);

    // Set up 60 FPS draw loop.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateCanvas()));
    m_timer.start(1000.0f / m_targetFPS);
}

GLWidget::~GLWidget()
{
    safeDelete(m_mesh);
    safeDelete(m_testSimulation);
    safeDelete(m_hairObject);
    safeDelete(m_hairProgram);
    safeDelete(m_meshProgram);
    safeDelete(m_noiseTexture);
}

void GLWidget::initializeGL()
{
    ResourceLoader::initializeGlew();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Initialize shader programs.
    m_hairProgram->create();
    m_meshProgram->create();

    // Initialize textures.
    m_noiseTexture->create(":/images/noise128.jpg", GL_LINEAR, GL_LINEAR);
    m_shadowDepthTexture->createDepthTexture(1024, 1024);

    // Initialize framebuffers.
    m_shadowFramebuffer->create();
    m_shadowFramebuffer->attachDepthTexture(m_shadowDepthTexture->id);
    
    // Initialize simulation.
    initSimulation();

    // Initialize global view and projection matrices.
    m_view = glm::lookAt(glm::vec3(0,0,m_zoom), glm::vec3(0), glm::vec3(0,1,0));
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    
    ErrorChecker::printGLErrors("end of initializeGL");
}

void GLWidget::paintGL()
{
    ErrorChecker::printGLErrors("start of paintGL");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = m_increment++ / (float) m_targetFPS;      // Time in seconds.

    m_testSimulation->update(time);
    m_hairObject->update(time);

    glm::mat4 model = glm::mat4(1.f);
    glm::vec3 lightPosition = glm::vec3(2, 1, 2);
    glm::mat4 lightProjection = glm::perspective(1.5f, 1.f, .1f, 100.f);
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0), glm::vec3(0,1,0));
    glm::mat4 eyeToLight = lightProjection * lightView * glm::inverse(m_view);

    // Render shadow map.
    glViewport(0, 0, m_shadowDepthTexture->width(), m_shadowDepthTexture->height());
    m_shadowFramebuffer->bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_meshProgram->bind();
        m_meshProgram->uniforms.projection = lightProjection;
        m_meshProgram->uniforms.view = lightView;
        m_meshProgram->uniforms.model = model;
        m_meshProgram->setGlobalUniforms();
        m_meshProgram->setPerObjectUniforms();
        m_mesh->draw();
        m_meshProgram->unbind();
        m_hairProgram->bind();
        m_noiseTexture->bind(GL_TEXTURE0);
        m_shadowDepthTexture->bind(GL_TEXTURE1);
        m_hairProgram->uniforms.projection = lightProjection;
        m_hairProgram->uniforms.view = lightView;
        m_hairProgram->uniforms.model = model;
        m_hairProgram->uniforms.eyeToLight = eyeToLight;
        m_hairProgram->uniforms.lightPosition = lightPosition;
        m_hairProgram->uniforms.noiseTexture = 0;
        m_hairProgram->uniforms.shadowMap = 1;
        m_hairProgram->setGlobalUniforms();
        m_hairObject->paint(m_hairProgram);
        m_shadowDepthTexture->unbind(GL_TEXTURE1);
        m_noiseTexture->unbind(GL_TEXTURE0);
        m_hairProgram->unbind();
    }
    m_shadowFramebuffer->unbind();

    // Render hair.
    glViewport(0, 0, width(), height());
    m_hairProgram->bind();
    m_noiseTexture->bind(GL_TEXTURE0);
    m_shadowDepthTexture->bind(GL_TEXTURE1);
    m_hairProgram->uniforms.projection = m_projection;
    m_hairProgram->uniforms.view = m_view;
    m_hairProgram->uniforms.model = model;
    m_hairProgram->uniforms.eyeToLight = eyeToLight;
    m_hairProgram->uniforms.lightPosition = lightPosition;
    m_hairProgram->uniforms.noiseTexture = 0;
    m_hairProgram->uniforms.shadowMap = 1;
    m_hairProgram->setGlobalUniforms();
    m_hairObject->paint(m_hairProgram);
    m_shadowDepthTexture->unbind(GL_TEXTURE1);
    m_noiseTexture->unbind(GL_TEXTURE0);
    m_hairProgram->unbind();
    
    // Render mesh.
#if _USE_MESH_
    m_meshProgram->bind();
    m_meshProgram->uniforms.projection = m_projection;
    m_meshProgram->uniforms.view = m_view;
    m_meshProgram->uniforms.model = model;
    m_meshProgram->uniforms.lightPosition = lightPosition;
    m_meshProgram->setGlobalUniforms();
    m_meshProgram->setPerObjectUniforms();
    m_mesh->draw();
    m_meshProgram->unbind();
#endif

    m_hairInterface->updateFPSLabel(m_increment);

}


void GLWidget::initSimulation()
{
    safeDelete(m_mesh);
    safeDelete(m_testSimulation);
    HairObject *_oldHairObject = m_hairObject;


#if _USE_MESH_
    m_mesh = new ObjMesh();
    m_mesh->init(":/models/head.obj");
    m_testSimulation = new Simulation(m_mesh);
    m_hairObject = new HairObject(
                m_mesh, m_hairDensity, ":/images/headHair.png", m_testSimulation, m_hairObject);
#else
    if (_oldHairObject != NULL){
        m_hairObject = new HairObject(_oldHairObject, m_testSimulation);
    } else {
        m_hairObject = new HairObject(1, m_testSimulation);
    }
#endif

    safeDelete(_oldHairObject);

    m_hairInterface->setHairObject(m_hairObject);
}


void GLWidget::resetSimulation()
{

    initSimulation();
}


void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
}

/** Repaints the canvas. Called 60 times per second. */
void GLWidget::updateCanvas()
{
    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = 10 * (event->x() - m_prevMousePos.x()) / (float) width();
    float dy = 10 * (m_prevMousePos.y() - event->y()) / (float) height();
    glm::vec3 up(0, 1, 0);
    glm::vec4 pos = m_view * glm::vec4(0, 0, 0, 1);
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(dx, up) *
            glm::rotate(dy, glm::cross(up, glm::vec3(pos.x, pos.y, pos.z))) *
            glm::translate(glm::vec3(0, 0, m_zoom)) *
            m_view;
    m_prevMousePos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_view = glm::translate(glm::vec3(0, 0, m_zoom)) * m_view;
    m_zoom -= event->delta() / 100.f;
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) * m_view;
}

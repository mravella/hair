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
#include "hairopacityshaderprogram.h"
#include "whitehairshaderprogram.h"
#include "whitemeshshaderprogram.h"
#include "hairinterface.h"
#include "meshocttree.h"
#include "texture.h"
#include "framebuffer.h"

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
      m_hairDensity(50),
      m_timer(this),
      m_increment(0),
      m_targetFPS(60.f)
{
    m_highResMesh = NULL;
    m_lowResMesh = NULL;
    m_hairObject = NULL;
    m_testSimulation = NULL;

    // Shader programs
    m_programs = {
        m_hairProgram = new HairShaderProgram(),
        m_meshProgram = new MeshShaderProgram(),
        m_hairOpacityProgram = new HairOpacityShaderProgram(),
        m_whiteHairProgram = new WhiteHairShaderProgram(),
        m_whiteMeshProgram = new WhiteMeshShaderProgram(),
    };

    // Textures
    m_textures = {
        m_noiseTexture = new Texture(),
        m_hairDepthTexture = new Texture(),
        m_meshDepthTexture = new Texture(),
        m_opacityMapTexture = new Texture(),
        m_finalTexture = new Texture(),
    };

    // Framebuffers
    m_framebuffers = {
        m_hairShadowFramebuffer = new Framebuffer(),
        m_meshShadowFramebuffer = new Framebuffer(),
        m_opacityMapFramebuffer = new Framebuffer(),
        m_finalFramebuffer = new Framebuffer(),
    };

    m_hairInterface->setGLWidget(this);

    // Set up 60 FPS draw loop.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateCanvas()));
    m_timer.start(1000.0f / m_targetFPS);
}

GLWidget::~GLWidget()
{
    for (auto program = m_programs.begin(); program != m_programs.end(); ++program)
        safeDelete(*program);
    for (auto texture = m_textures.begin(); texture != m_textures.end(); ++texture)
        safeDelete(*texture);
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        safeDelete(*framebuffer);

    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    safeDelete(m_testSimulation);
    safeDelete(m_hairObject);
}

void GLWidget::initializeGL()
{
    ResourceLoader::initializeGlew();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Initialize shader programs.
    for (auto program = m_programs.begin(); program != m_programs.end(); ++program)
        (*program)->create();

    // Initialize textures.
    int shadowMapRes = 2048;
    m_noiseTexture->createColorTexture(":/images/noise128.jpg", GL_LINEAR, GL_LINEAR);
    m_hairDepthTexture->createDepthTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_meshDepthTexture->createDepthTexture(shadowMapRes, shadowMapRes, GL_LINEAR, GL_LINEAR);
    m_opacityMapTexture->createColorTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_finalTexture->createColorTexture(2 * width(), 2 * height(), GL_LINEAR, GL_LINEAR);

    // Initialize framebuffers.
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        (*framebuffer)->create();
    m_hairShadowFramebuffer->attachDepthTexture(m_hairDepthTexture->id);
    m_meshShadowFramebuffer->attachDepthTexture(m_meshDepthTexture->id);
    m_opacityMapFramebuffer->attachColorTexture(m_opacityMapTexture->id);
    m_opacityMapFramebuffer->generateDepthBuffer(shadowMapRes, shadowMapRes);
    m_finalFramebuffer->attachColorTexture(m_finalTexture->id);
    m_finalFramebuffer->generateDepthBuffer(m_finalTexture->width(), m_finalTexture->height());
    
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

    m_increment++;
    float time = m_increment / (float) m_targetFPS;      // Time in seconds.

    if (!paused)
    {
        m_testSimulation->update(time);
        m_hairObject->update(time);
    }

    glm::mat4 model = glm::mat4(1.f);
    model = m_testSimulation->m_xform;
    glm::vec3 lightPosition = glm::vec3(1, 2, 4);
    glm::mat4 lightProjection = glm::perspective(1.3f, 1.f, .1f, 100.f);
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0), glm::vec3(0,1,0));
    glm::mat4 eyeToLight = lightProjection * lightView * glm::inverse(m_view);

    // Bind textures.
    m_noiseTexture->bind(GL_TEXTURE0);
    m_hairDepthTexture->bind(GL_TEXTURE1);
    m_opacityMapTexture->bind(GL_TEXTURE2);
    m_meshDepthTexture->bind(GL_TEXTURE3);
    m_finalTexture->bind(GL_TEXTURE4);

    ShaderProgram *program;

    if (useShadows)
    {
        // Render hair shadow map.
        glViewport(0, 0, m_hairDepthTexture->width(), m_hairDepthTexture->height());
        m_hairShadowFramebuffer->bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            program = m_whiteHairProgram;
            {
                program->bind();
                program->uniforms.noiseTexture = 0;
                program->uniforms.hairShadowMap = 1;
                program->uniforms.projection = lightProjection;
                program->uniforms.view = lightView;
                program->uniforms.model = model;
                program->uniforms.eyeToLight = eyeToLight;
                program->uniforms.lightPosition = lightPosition;
                program->setGlobalUniforms();
                m_hairObject->paint(program);
            }
        }

        // Render mesh shadow map.
        m_meshShadowFramebuffer->bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            program = m_whiteMeshProgram;
            {
                program->bind();
                program->uniforms.projection = lightProjection;
                program->uniforms.view = lightView;
                program->uniforms.model = model;
                program->setGlobalUniforms();
                program->setPerObjectUniforms();
                m_highResMesh->draw();
            }
        }
        m_meshShadowFramebuffer->unbind();

        // Render opacity map.
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        glViewport(0, 0, m_hairDepthTexture->width(), m_hairDepthTexture->height());
        m_opacityMapFramebuffer->bind();
        {
            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            program = m_hairOpacityProgram;
            {
                program->bind();
                program->uniforms.noiseTexture = 0;
                program->uniforms.hairShadowMap = 1;
                program->uniforms.projection = lightProjection;
                program->uniforms.view = lightView;
                program->uniforms.model = model;
                program->uniforms.eyeToLight = eyeToLight;
                program->uniforms.lightPosition = lightPosition;
                program->setGlobalUniforms();
                m_hairObject->paint(program);
            }
        }
        m_opacityMapFramebuffer->unbind();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

    } // if useShadows


    if (useSupersampling)
    {
        m_finalFramebuffer->bind();
        glViewport(0, 0, m_finalTexture->width(), m_finalTexture->height());
    }
    else
    {
        glViewport(0, 0, width(), height());
    }

    // Render hair.
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program = m_hairProgram;
    {
        program->bind();
        program->uniforms.noiseTexture = 0;
        program->uniforms.hairShadowMap = 1;
        program->uniforms.opacityMap = 2;
        program->uniforms.meshShadowMap = 3;
        program->uniforms.projection = m_projection;
        program->uniforms.view = m_view;
        program->uniforms.model = model;
        program->uniforms.eyeToLight = eyeToLight;
        program->uniforms.lightPosition = lightPosition;
        program->uniforms.shadowIntensity = 15;
        program->uniforms.useShadows = useShadows;
        program->setGlobalUniforms();
        m_hairObject->paint(program);
    }
    
    // Render mesh.
    program = m_meshProgram;
    {
        program->bind();
        program->uniforms.hairShadowMap = 1;
        program->uniforms.opacityMap = 2;
        program->uniforms.meshShadowMap = 3;
        program->uniforms.projection = m_projection;
        program->uniforms.view = m_view;
        program->uniforms.model = model;
        program->uniforms.lightPosition = lightPosition;
        program->uniforms.eyeToLight = eyeToLight;
        program->uniforms.shadowIntensity = 15;
        program->uniforms.useShadows = useShadows;
        program->setGlobalUniforms();
        program->setPerObjectUniforms();
        m_highResMesh->draw();
    }

    if (useSupersampling)
    {
        m_finalFramebuffer->unbind();
        glViewport(0, 0, width(), height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_finalTexture->renderFullScreen();
    }

    // Clean up.
    program->unbind();
    m_noiseTexture->unbind(GL_TEXTURE0);
    m_hairDepthTexture->unbind(GL_TEXTURE1);
    m_opacityMapTexture->unbind(GL_TEXTURE2);
    m_meshDepthTexture->unbind(GL_TEXTURE3);
    m_finalTexture->unbind(GL_TEXTURE4);

    // Update UI.
    m_hairInterface->updateFPSLabel(m_increment);

}


void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);

    m_finalTexture->resize(2*w, 2*h);
    m_finalFramebuffer->resizeDepthBuffer(2*w, 2*h);
}


void GLWidget::initSimulation()
{
    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    safeDelete(m_testSimulation);
    HairObject *_oldHairObject = m_hairObject;

    m_highResMesh = new ObjMesh();
    m_highResMesh->init(":/models/sphere.obj");

    m_lowResMesh = new ObjMesh();
    m_lowResMesh->init(":/models/sphere.obj", 1.1);

    m_testSimulation = new Simulation(m_lowResMesh);

    m_hairObject = new HairObject(
                m_highResMesh, m_hairDensity, ":/images/lower.png", m_testSimulation, m_hairObject);

    safeDelete(_oldHairObject);

    m_hairInterface->setHairObject(m_hairObject);
}


void GLWidget::resetSimulation()
{

    initSimulation();
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
    m_angleX += 10 * (event->x() - m_prevMousePos.x()) / (float) width();
    m_angleY += 10 * (event->y() - m_prevMousePos.y()) / (float) height();
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));
    m_prevMousePos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_zoom -= event->delta() / 100.f;
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));
}

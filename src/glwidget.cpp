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

#include <glm/gtx/color_space.hpp>

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
      m_hairDensity(100),
      m_timer(this),
      m_increment(0),
      m_targetFPS(60.f)
{
    m_highResMesh = NULL;
    m_lowResMesh = NULL;
    m_hairObject = NULL;
    m_testSimulation = NULL;
    
    resetTexture = NULL;

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
    
    initCamera();
    
    ErrorChecker::printGLErrors("end of initializeGL");
}

void GLWidget::paintGL()
{
    if (resetTexture != NULL){
        partialResetSim(resetTexture);
        resetTexture = NULL;
    }
    
    ErrorChecker::printGLErrors("start of paintGL");
    
    // Update simulation if not paused.
    if (!isPaused())
    {
        m_increment++;
        float time = m_increment / (float) m_targetFPS; // Time in seconds (assuming 60 FPS).
        m_testSimulation->update(time);
        m_hairObject->update(time);
    }
    
    // Update transformation matrices.
    glm::mat4 model = glm::mat4(1.f);
    model = m_testSimulation->m_xform;
    m_lightPosition = glm::vec3(2, 2, 2);
    glm::mat4 lightProjection = glm::perspective(1.3f, 1.f, .1f, 100.f);
    glm::mat4 lightView = glm::lookAt(m_lightPosition, glm::vec3(0), glm::vec3(0,1,0));
    m_eyeToLight = lightProjection * lightView * glm::inverse(m_view);
    
    // Bind textures.
    m_noiseTexture->bind(GL_TEXTURE0);
    m_hairDepthTexture->bind(GL_TEXTURE1);
    m_opacityMapTexture->bind(GL_TEXTURE2);
    m_meshDepthTexture->bind(GL_TEXTURE3);
    m_finalTexture->bind(GL_TEXTURE4);
    m_hairObject->m_blurredHairGrowthMapTexture->bind(GL_TEXTURE5);
    
    if (useShadows)
    {
        // Render hair shadow map.
        m_hairShadowFramebuffer->bind();
        glViewport(0, 0, m_hairDepthTexture->width(), m_hairDepthTexture->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawHair(m_whiteHairProgram, model, lightView, lightProjection);
        
        // Render mesh shadow map.
        m_meshShadowFramebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawMesh(m_whiteMeshProgram, model, lightView, lightProjection);
        
        // Enable additive blending for opacity map.
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        
        // Render opacity map.
        m_opacityMapFramebuffer->bind();
        glViewport(0, 0, m_hairDepthTexture->width(), m_hairDepthTexture->height());
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawHair(m_hairOpacityProgram, model, lightView, lightProjection);
        
        // Restore previous state.
        m_opacityMapFramebuffer->unbind();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
    }
    
    
    if (useSupersampling)
    {
        // Render into supersample framebuffer.
        m_finalFramebuffer->bind();
        glViewport(0, 0, m_finalTexture->width(), m_finalTexture->height());
    }
    else
    {
        // Render into default framebuffer.
        glViewport(0, 0, width(), height());
    }
    
    
    // Render scene.
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _drawHair(m_hairProgram, model, m_view, m_projection);
    _drawMesh(m_meshProgram, model, m_view, m_projection);
    
    
    if (useSupersampling)
    {
        // Render supersampled texture.
        m_finalFramebuffer->unbind();
        glViewport(0, 0, width(), height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_finalTexture->renderFullScreen();
    }
    
    
    // Clean up.
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

    forceUpdate();
}


void GLWidget::_drawHair(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    program->bind();
    program->uniforms.noiseTexture = 0;
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.lightPosition = m_lightPosition;
    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;
    program->uniforms.useShadows = useShadows;
    program->uniforms.specIntensity = m_hairObject->m_specularIntensity;
    program->uniforms.diffuseIntensity = m_hairObject->m_diffuseIntensity;
    program->setGlobalUniforms();
    m_hairObject->paint(program);
}


void GLWidget::_drawMesh(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    program->bind();
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.hairGrowthMap = 5;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.lightPosition = m_lightPosition;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;
    program->uniforms.useShadows = useShadows;
    program->uniforms.color = 2.f * glm::rgbColor(glm::vec3(m_hairObject->m_color.x*255, m_hairObject->m_color.y, m_hairObject->m_color.z)); // multiplying by 2 because it looks better...
    program->setGlobalUniforms();
    program->setPerObjectUniforms();
    m_highResMesh->draw();
}


void GLWidget::initSimulation()
{
    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    safeDelete(m_testSimulation);
    HairObject *_oldHairObject = m_hairObject;
    
    m_highResMesh = new ObjMesh();
    m_highResMesh->init(":/models/head.obj");
    m_hairInterface->setMesh(m_highResMesh);
    
    m_lowResMesh = new ObjMesh();
    m_lowResMesh->init(":/models/headLowRes.obj", 1.1);
    
    m_testSimulation = new Simulation(this, m_lowResMesh);
    
    QImage initialHairMap(":/images/headHair.jpg");
    m_hairObject = new HairObject(m_highResMesh, m_hairDensity, initialHairMap, m_testSimulation, m_hairObject);
    
    safeDelete(_oldHairObject);
    
    m_hairInterface->setHairObject(m_hairObject);
    
}

void GLWidget::initCamera(){
    
    m_angleX = 0;
    m_angleY = .33;
    m_zoom = 3;
    
    // Initialize global view and projection matrices.
//    m_view = glm::lookAt(glm::vec3(0,0,m_zoom), glm::vec3(0), glm::vec3(0,1,0));
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));
    
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    
}

void GLWidget::partialResetSim(Texture *texture){

    HairObject *_oldHairObject = m_hairObject;
        
    m_hairObject = new HairObject(
                m_highResMesh, m_hairDensity, texture->m_image, m_testSimulation);
    
    
    safeDelete(_oldHairObject);
    
    m_hairInterface->setHairObject(m_hairObject);
    
}

void GLWidget::resetSimulation()
{    
    initSimulation();
    initCamera();
}


/** Repaints the canvas. Called 60 times per second. */
void GLWidget::updateCanvas()
{
    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        m_prevMousePos = event->pos();
    }
    if (event->button() == Qt::LeftButton)
    {
        m_prevXformPos = event->pos();
        m_testSimulation->m_headMoving = true;
    }
    if (event->button() == Qt::MiddleButton)
    {
        m_prevRotPos = event->pos();
        m_testSimulation->m_headMoving = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::RightButton)
    {
        m_angleX += 10 * (event->x() - m_prevMousePos.x()) / (float) width();
        m_angleY += 10 * (event->y() - m_prevMousePos.y()) / (float) height();
        m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
                glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
                glm::rotate(m_angleX, glm::vec3(0, 1, 0));
        m_prevMousePos = event->pos();
    }
    if (event->buttons() == Qt::LeftButton)
    {
        glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        glm::mat4 inverseView = glm::inverse(m_view);
        glm::vec3 look = glm::normalize(glm::vec3(inverseView * glm::vec4(0, 0, -1.0, 1)));
        cout << "up: " << glm::to_string(up) << endl;
        cout << "look: " << glm::to_string(look) << endl;
        glm::vec3 right = glm::cross(up, look);
        QPoint delta = event->pos() - m_prevXformPos;
        glm::vec3 xform = glm::vec3();
        xform += (float) delta.x() * 0.005f * right;
        xform += (float) -delta.y() * 0.005f * up;
        if (look.z < 0.0) xform = -xform;
        m_testSimulation->updatePosition(m_hairObject, xform);
        m_prevXformPos = event->pos();
    }
    if (event->buttons() == Qt::MiddleButton)
    {
        QPoint delta = event->pos() - m_prevRotPos;
        if (fabs(delta.x()) > fabs(delta.y()))
        {
            // Rotate in up
            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
            float angle = delta.x() * 0.001f;
            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(0, 1, 0));
        }
        else
        {
            // Rotate in right
//            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
//            glm::mat4 inverseView = glm::inverse(m_view);
//            glm::vec3 look = glm::normalize(glm::vec3(inverseView * glm::vec4(0, 0, 0, 1)));
//            glm::vec3 right = glm::cross(up, look);
            float angle = delta.y() * 0.001f;
            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(1,0,0));
        }
    }

    forceUpdate();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_testSimulation->m_headMoving = false;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_zoom -= event->delta() / 100.f;
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));

    forceUpdate();
}

void GLWidget::pause()
{
    if (!m_paused)
    {
        m_paused = true;
        m_timer.stop();
    }
}

void GLWidget::unpause()
{
    if (m_paused)
    {
        m_paused = false;
        m_timer.start();
    }
}

bool GLWidget::isPaused()
{
    return m_paused;
}

void GLWidget::forceUpdate()
{
    if (isPaused())
    {
        update();
    }
}

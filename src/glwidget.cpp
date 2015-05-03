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
#include "hairdepthpeelprogram.h"
#include "meshdepthpeelprogram.h"
#include "hairinterface.h"
#include "meshocttree.h"
#include "texture.h"
#include "framebuffer.h"

#include <glm/gtx/color_space.hpp>

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
<<<<<<< HEAD
      m_hairDensity(250),
=======
      m_hairDensity(100),
      m_maxHairLength(.45),
>>>>>>> 01092fde9e0245c27b8256649704430d74acc753
      m_timer(this),
      m_increment(0),
      m_targetFPS(60.f)
{
    m_highResMesh = NULL;
    m_lowResMesh = NULL;
    m_hairObject = NULL;
    m_testSimulation = NULL;
    
    resetFromSceneEditorGrowthTexture = NULL;
    resetFromSceneEditorGroomingTexture = NULL;
    
    m_noiseTexture = new Texture();
    
    // Shader programs
    m_programs = {
        m_hairProgram = new HairShaderProgram(),
        m_meshProgram = new MeshShaderProgram(),
        m_hairOpacityProgram = new HairOpacityShaderProgram(),
        m_whiteHairProgram = new WhiteHairShaderProgram(),
        m_whiteMeshProgram = new WhiteMeshShaderProgram(),
        m_hairDepthPeelProgram = new HairDepthPeelShaderProgram(),
        m_meshDepthPeelProgram = new MeshDepthPeelShaderProgram(),
    };
    
    // Framebuffers
    m_framebuffers = {
        m_hairShadowFramebuffer = new Framebuffer(),
        m_meshShadowFramebuffer = new Framebuffer(),
        m_opacityMapFramebuffer = new Framebuffer(),
        m_finalFramebuffer = new Framebuffer(),
        m_depthPeel0Framebuffer = new Framebuffer(),
        m_depthPeel1Framebuffer = new Framebuffer(),
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
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        safeDelete(*framebuffer);
    
    safeDelete(m_noiseTexture);
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
    m_noiseTexture->createColorTexture(":/images/noise128.jpg", GL_LINEAR, GL_LINEAR);
    
    // Initialize framebuffers.
    int shadowMapRes = 2048;
    glm::vec2 finalSize = glm::vec2(2 * width(), 2 * height());
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        (*framebuffer)->create();
    m_hairShadowFramebuffer->generateDepthTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_meshShadowFramebuffer->generateDepthTexture(shadowMapRes, shadowMapRes, GL_LINEAR, GL_LINEAR);
    m_opacityMapFramebuffer->generateColorTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_opacityMapFramebuffer->generateDepthBuffer(shadowMapRes, shadowMapRes);
    m_finalFramebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_finalFramebuffer->generateDepthBuffer(finalSize.x, finalSize.y);
    m_depthPeel0Framebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_depthPeel0Framebuffer->generateDepthTexture(finalSize.x, finalSize.y, GL_NEAREST, GL_NEAREST);
    m_depthPeel1Framebuffer->generateColorTexture(finalSize.x, finalSize.y, GL_LINEAR, GL_LINEAR);
    m_depthPeel1Framebuffer->generateDepthBuffer(finalSize.x, finalSize.y);
    
    // Initialize simulation.
    initSimulation();
    
    initCamera();
    
    ErrorChecker::printGLErrors("end of initializeGL");
}

void GLWidget::paintGL()
{
    ErrorChecker::printGLErrors("start of paintGL");
    
    if (resetFromSceneEditorGroomingTexture != NULL && resetFromSceneEditorGrowthTexture != NULL){
        applySceneEditor(resetFromSceneEditorGrowthTexture, resetFromSceneEditorGroomingTexture);
        resetFromSceneEditorGroomingTexture = NULL;
        resetFromSceneEditorGrowthTexture = NULL;
    }
    
    m_clock.restart();
    
    _resizeDepthPeelFramebuffers();
    
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
    m_hairShadowFramebuffer->depthTexture->bind(GL_TEXTURE1);
    m_opacityMapFramebuffer->colorTexture->bind(GL_TEXTURE2);
    m_meshShadowFramebuffer->depthTexture->bind(GL_TEXTURE3);
    m_finalFramebuffer->colorTexture->bind(GL_TEXTURE4);
    m_hairObject->m_blurredHairGrowthMapTexture->bind(GL_TEXTURE5);
    m_depthPeel0Framebuffer->depthTexture->bind(GL_TEXTURE6);
    m_depthPeel0Framebuffer->colorTexture->bind(GL_TEXTURE7);
    m_depthPeel1Framebuffer->colorTexture->bind(GL_TEXTURE8);
    
    if (useShadows)
    {
        // Render hair shadow map.
        m_hairShadowFramebuffer->bind();
        glViewport(0, 0, m_hairShadowFramebuffer->depthTexture->width(), m_hairShadowFramebuffer->depthTexture->height());
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
        glViewport(0, 0, m_hairShadowFramebuffer->depthTexture->width(), m_hairShadowFramebuffer->depthTexture->height());
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawHair(m_hairOpacityProgram, model, lightView, lightProjection);
        
        // Restore previous state.
        m_opacityMapFramebuffer->unbind();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
    }
    
    if (useTransparency)
    {
        glViewport(0, 0, m_depthPeel0Framebuffer->colorTexture->width(), m_depthPeel0Framebuffer->colorTexture->height());
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        
        // Draw first (front-most) depth peeling layer.
        m_depthPeel0Framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _drawHair(m_hairProgram, model, m_view, m_projection);
        _drawMesh(m_meshProgram, model, m_view, m_projection);
        
        // Draw second depth peeling layer.
        m_depthPeel1Framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      
        _drawHair(m_hairDepthPeelProgram, model, m_view, m_projection);
        _drawMesh(m_meshDepthPeelProgram, model, m_view, m_projection);
        
        // Render farthest layer to screen.
        m_depthPeel1Framebuffer->unbind();
        glViewport(0, 0, width(), height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        m_depthPeel1Framebuffer->colorTexture->renderFullScreen();
        
        // Blend closer layers on top.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_depthPeel0Framebuffer->colorTexture->renderFullScreen();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
    
    else
    {
        if (useSupersampling)
        {
            // Render into supersample framebuffer.
            m_finalFramebuffer->bind();
            glViewport(0, 0, m_finalFramebuffer->colorTexture->width(), m_finalFramebuffer->colorTexture->height());
        }
        else
        {
            // Render into default framebuffer...
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
            m_finalFramebuffer->colorTexture->renderFullScreen();
        }
    }
    
    
    // Clean up.
    m_noiseTexture->unbind(GL_TEXTURE0);
    m_hairShadowFramebuffer->depthTexture->unbind(GL_TEXTURE1);
    m_opacityMapFramebuffer->colorTexture->unbind(GL_TEXTURE2);
    m_meshShadowFramebuffer->depthTexture->unbind(GL_TEXTURE3);
    m_finalFramebuffer->colorTexture->unbind(GL_TEXTURE4);
    m_hairObject->m_blurredHairGrowthMapTexture->unbind(GL_TEXTURE5);
    m_depthPeel0Framebuffer->depthTexture->bind(GL_TEXTURE6);
    m_depthPeel0Framebuffer->colorTexture->bind(GL_TEXTURE7);
    m_depthPeel1Framebuffer->colorTexture->bind(GL_TEXTURE8);
    
    // Update UI.
    m_hairInterface->updateFPSLabel(m_increment);
    if (m_paused || m_pausedLastFrame)
    {
        m_hairInterface->updateFPSLabelPaused(1000.0 / m_clock.elapsed());
    }
    m_pausedLastFrame = isPaused();
}


void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    
    m_finalFramebuffer->colorTexture->resize(2*w, 2*h);
    m_finalFramebuffer->resizeDepthBuffer(2*w, 2*h);
    
    forceUpdate();
}


void GLWidget::_resizeDepthPeelFramebuffers()
{
    int w = useSupersampling ? 2 * width() : width();
    int h = useSupersampling ? 2 * height() : height();
    
    Texture *colorTexture0 = m_depthPeel0Framebuffer->colorTexture;
    if (colorTexture0->width() != w || colorTexture0->height() != h)
    {
        m_depthPeel0Framebuffer->colorTexture->resize(w, h);
        m_depthPeel0Framebuffer->depthTexture->resize(w, h);
        m_depthPeel1Framebuffer->colorTexture->resize(w, h);
        m_depthPeel1Framebuffer->resizeDepthBuffer(w, h);
    }
}

void GLWidget::_drawHair(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    program->bind();
    program->uniforms.noiseTexture = 0;
    program->uniforms.hairShadowMap = 1;
    program->uniforms.opacityMap = 2;
    program->uniforms.meshShadowMap = 3;
    program->uniforms.depthPeelMap = 6;
    program->uniforms.projection = projection;
    program->uniforms.view = view;
    program->uniforms.model = model;
    program->uniforms.eyeToLight = m_eyeToLight;
    program->uniforms.lightPosition = m_lightPosition;
    program->uniforms.shadowIntensity = m_hairObject->m_shadowIntensity;
    program->uniforms.useShadows = useShadows;
    program->uniforms.specIntensity = m_hairObject->m_specularIntensity;
    program->uniforms.diffuseIntensity = m_hairObject->m_diffuseIntensity;
    program->uniforms.opacity = 1.f - m_hairObject->m_transparency;
    if (m_hairObject->m_useHairColorVariation){
        program->uniforms.maxColorVariation = m_hairObject->m_hairColorVariation;
    } else {
        program->uniforms.maxColorVariation = 0;
    }
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
    HairObject *_oldHairObject = m_hairObject;
    
    m_highResMesh = new ObjMesh();
    m_highResMesh->init(":/models/head.obj");
    m_hairInterface->setMesh(m_highResMesh);
    
    m_lowResMesh = new ObjMesh();
    m_lowResMesh->init(":/models/headLowRes.obj", 1.1);
    
    Simulation *_oldSim = m_testSimulation;
    m_testSimulation = new Simulation(this, m_lowResMesh, _oldSim);
    
    if (_oldHairObject == NULL){
        QImage initialGrowthMap(":/images/headHair.jpg");
        QImage initialGroomingMap(initialGrowthMap.width(), initialGrowthMap.height(), initialGrowthMap.format());
        initialGroomingMap.fill(QColor(128, 128, 255));
        m_hairObject = new HairObject(m_highResMesh, m_hairDensity, m_maxHairLength, initialGrowthMap, initialGroomingMap, m_testSimulation, _oldHairObject);
    } else {
        m_hairObject = new HairObject(m_highResMesh, m_hairDensity, m_maxHairLength, _oldHairObject->m_hairGrowthMap, _oldHairObject->m_hairGroomingMap, m_testSimulation, _oldHairObject);
    }
    
    safeDelete(_oldSim);
    safeDelete(_oldHairObject);
    
    m_hairInterface->setHairObject(m_hairObject);
    
}

void GLWidget::initCamera(){
    
    m_angleX = 0;
    m_angleY = .33;
    m_zoom = 3;
    
    // Initialize global view and projection matrices.
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
            glm::rotate(m_angleY, glm::vec3(1, 0, 0)) *
            glm::rotate(m_angleX, glm::vec3(0, 1, 0));
    
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    
}

void GLWidget::applySceneEditor(Texture *_hairGrowthTexture, Texture *_hairGroomingTexture){
    
    HairObject *_oldHairObject = m_hairObject;
    
    m_hairObject = new HairObject(
                m_highResMesh, m_hairDensity, m_maxHairLength, _hairGrowthTexture->m_image, _hairGroomingTexture->m_image, m_testSimulation, _oldHairObject);
    
    
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

        glm::vec3 p0 = glm::vec3(m_prevRotPos.x() / (float)width(), 1.f - m_prevRotPos.y() / (float)height(), 0.f);
        glm::vec3 p1 = glm::vec3(event->pos().x() / (float)width(), 1.f - event->pos().y() / (float)height(), 0.f);

        p0 = 2.f * p0 - 1.f;
        p1 = 2.f * p1 - 1.f;

        float angle = 5 * glm::length(p1 - p0);

        glm::mat4 clipToObject = glm::inverse(m_projection * m_view * m_testSimulation->m_xform);
        p0 = glm::vec3(glm::normalize( clipToObject * glm::vec4(p0, 0.f) ));
        p1 = glm::vec3(glm::normalize( clipToObject * glm::vec4(p1, 0.f) ));

        glm::vec3 axis = glm::cross(p0, p1);

        m_testSimulation->updateRotation(m_hairObject, angle, axis);

        m_prevRotPos = event->pos();


        //        QPoint delta = event->pos() - m_prevRotPos;
        //        if (fabs(delta.x()) > fabs(delta.y()))
        //        {
        //            // Rotate in up
        //            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        //            float angle = delta.x() * 0.001f;
        //            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(0, 1, 0));
        //        }
        //        else
        //        {
        //            // Rotate in right
        ////            glm::vec3 up = glm::normalize(glm::vec3(m_view[2][1], m_view[2][2], m_view[2][3]));
        ////            glm::mat4 inverseView = glm::inverse(m_view);
        ////            glm::vec3 look = glm::normalize(glm::vec3(inverseView * glm::vec4(0, 0, 0, 1)));
        ////            glm::vec3 right = glm::cross(up, look);
        //            float angle = delta.y() * 0.001f;
        //            m_testSimulation->updateRotation(m_hairObject, angle, glm::vec3(1,0,0));
        //        }
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

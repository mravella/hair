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
#include "hairkbufferprogram.h"
#include "hairinterface.h"
#include "meshocttree.h"
#include "texture.h"
#include "framebuffer.h"

#include "quad.h"

GLWidget::GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent)
    : QGLWidget(format, parent),
      m_hairInterface(hairInterface),
      m_hairDensity(40),
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
        m_hairKBufferProgram = new HairKBufferProgram(),
    };

    // Textures
    m_textures = {
        m_noiseTexture = new Texture(),
        m_hairDepthTexture = new Texture(),
        m_meshDepthTexture = new Texture(),
        m_opacityMapTexture = new Texture(),
    };

    // K-buffer texture
    m_kbufferTextures = {
        m_hairK1 = new Texture(),
        m_hairK2 = new Texture(),
    };

    // Framebuffers
    m_framebuffers = {
        m_hairShadowFramebuffer = new Framebuffer(),
        m_meshShadowFramebuffer = new Framebuffer(),
        m_opacityMapFramebuffer = new Framebuffer(),
        m_hairKFramebuffer = new Framebuffer(),
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
    for (auto texture = m_kbufferTextures.begin(); texture != m_kbufferTextures.end(); ++texture)
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
    m_noiseTexture->create(":/images/noise128.jpg", GL_LINEAR, GL_LINEAR);
    m_hairDepthTexture->createDepthTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_meshDepthTexture->createDepthTexture(shadowMapRes, shadowMapRes, GL_LINEAR, GL_LINEAR);
    m_opacityMapTexture->createColorTexture(shadowMapRes, shadowMapRes, GL_NEAREST, GL_NEAREST);
    m_hairK1->createColorTexture(width(), height(), GL_NEAREST, GL_NEAREST);
    m_hairK2->createColorTexture(width(), height(), GL_NEAREST, GL_NEAREST);

    // Initialize framebuffers.
    for (auto framebuffer = m_framebuffers.begin(); framebuffer != m_framebuffers.end(); ++framebuffer)
        (*framebuffer)->create();
    m_hairShadowFramebuffer->attachDepthTexture(m_hairDepthTexture->id);
    m_meshShadowFramebuffer->attachDepthTexture(m_meshDepthTexture->id);
    std::vector<GLuint> textures { m_opacityMapTexture->id };
    m_opacityMapFramebuffer->attachColorTextures(textures);
    m_opacityMapFramebuffer->generateDepthBuffer(shadowMapRes, shadowMapRes);
    textures = {m_hairK1->id, m_hairK2->id};
    m_hairKFramebuffer->attachColorTextures(textures);
    
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

    m_hairK1->bind(GL_TEXTURE4);
    m_hairK2->bind(GL_TEXTURE5);

    ShaderProgram *program;

#if 1
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

    // Render hair.
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width(), height());
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
//        m_hairObject->paint(program);
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
//        m_highResMesh->draw();
    }

    m_hairKFramebuffer->bind();
    glViewport(0, 0, width(), height());
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    {
        program = m_hairKBufferProgram;
        {
            program->bind();
            program->uniforms.noiseTexture = 0;
            program->uniforms.projection = m_projection;
            program->uniforms.view = m_view;
            program->uniforms.model = model;
            glUniform1i(glGetUniformLocation(program->id, "tex1"), 4);
            glUniform1i(glGetUniformLocation(program->id, "tex2"), 5);
            program->setGlobalUniforms();
            m_hairObject->paint(program);
        }
    }
    m_hairKFramebuffer->unbind();

    Quad q;
    q.init();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    GLuint p = ResourceLoader::createBasicShaderProgram(
                ":/shaders/texturedquad.vert", ":/shaders/renderkbuffer.frag");
    glUseProgram(p);
    glUniform1i(glGetUniformLocation(p, "tex1"), 4);
    glUniform1i(glGetUniformLocation(p, "tex2"), 5);

    q.draw();

    glUseProgram(0);

#else

    // render kbuffer thing

    GLfloat data[] = {
        -.5, -.5, 0,
        1, 0, 0,
         .5, -.5, 0,
        1, 0, 0,
          0,  .5, 0,
        1, 0, 0,

        -.4, -.5, -.2,
        0, 1, 0,
         .6, -.5, -.2,
        0, 1, 0,
         .1,  .5, -.2,
        0, 1, 0,

        -.3, -.5, -.4,
        0, 0, 1,
         .7, -.5, -.4,
        0, 0, 1,
         .2,  .5, -.4,
        0, 0, 1,
    };

    OpenGLShape shape1;
    shape1.create();
    shape1.setVertexData(data, sizeof(data), 3);
    shape1.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    shape1.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    OpenGLShape shape2;
    shape2.create();
    shape2.setVertexData(data, sizeof(data), 3);
    shape2.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 18 * sizeof(GLfloat));
    shape2.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 21 * sizeof(GLfloat));

    OpenGLShape shape3;
    shape3.create();
    shape3.setVertexData(data, sizeof(data), 3);
    shape3.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 36 * sizeof(GLfloat));
    shape3.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 39 * sizeof(GLfloat));


    model = glm::mat4(1.f);
    int res = 1024;

    Texture t1;
    t1.createColorTexture(res, res, GL_NEAREST, GL_NEAREST);
    t1.bind(GL_TEXTURE5);

    Texture t2;
    t2.createColorTexture(res, res, GL_NEAREST, GL_NEAREST);
    t2.bind(GL_TEXTURE6);

    Framebuffer f;
    f.create();
    std::vector<GLuint> textures = {t1.id, t2.id};
    f.attachColorTextures(textures);
    glDisable(GL_DEPTH_TEST);

    f.bind();

    glViewport(0, 0, res, res);
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint programID = ResourceLoader::createBasicShaderProgram(
                ":/shaders/test.vert", ":/shaders/kbuffer.frag");
    glUseProgram(programID);

    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
    glUniform1i(glGetUniformLocation(programID, "tex1"), 5);
    glUniform1i(glGetUniformLocation(programID, "tex2"), 6);

    shape1.draw(GL_TRIANGLES);
    shape2.draw(GL_TRIANGLES);
    shape3.draw(GL_TRIANGLES);

    glUseProgram(0);
    t1.unbind(GL_TEXTURE5);
    t2.unbind(GL_TEXTURE6);
    f.unbind();

    glViewport(0, 0, width(), height());

    if (useShadows)
        t1.renderFullScreen();
    else
        t2.renderFullScreen();

#endif

    // Clean up.
    program->unbind();
    m_meshDepthTexture->unbind(GL_TEXTURE3);
    m_opacityMapTexture->unbind(GL_TEXTURE2);
    m_hairDepthTexture->unbind(GL_TEXTURE1);
    m_noiseTexture->unbind(GL_TEXTURE0);

    // Update UI.
    m_hairInterface->updateFPSLabel(m_increment);

}


void GLWidget::initSimulation()
{
    safeDelete(m_highResMesh);
    safeDelete(m_lowResMesh);
    safeDelete(m_testSimulation);
    HairObject *_oldHairObject = m_hairObject;

    m_highResMesh = new ObjMesh();
    m_highResMesh->init(":/models/head.obj");

    m_lowResMesh = new ObjMesh();
    m_lowResMesh->init(":/models/headLowRes.obj", 1.1);

    m_testSimulation = new Simulation(m_lowResMesh);

    m_hairObject = new HairObject(
                m_highResMesh, m_hairDensity, ":/images/headHair.jpg", m_testSimulation, m_hairObject);

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

    for (auto texture = m_kbufferTextures.begin(); texture != m_kbufferTextures.end(); ++texture)
        safeDelete(*texture);
    m_kbufferTextures = {
        m_hairK1 = new Texture(),
        m_hairK2 = new Texture(),
    };
    m_hairK1->createColorTexture(w, h, GL_NEAREST, GL_NEAREST);
    m_hairK2->createColorTexture(w, h, GL_NEAREST, GL_NEAREST);
    std::vector<GLuint> kbufferTextures = {m_hairK1->id, m_hairK2->id};
    m_hairKFramebuffer->attachColorTextures(kbufferTextures);
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

#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "hairCommon.h"

class ObjMesh;
class HairObject;
class Simulation;
class ShaderProgram;
class HairInterface;
class Texture;
class Framebuffer;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent = 0);
    ~GLWidget();

    void resetSimulation();

    bool useShadows = true;
    bool useSupersampling = true;
    bool useFrictionSim = true;
    
    bool paused = false;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void initSimulation();

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void updateCanvas();

private:
    void _drawHair(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void _drawMesh(ShaderProgram *program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    HairInterface *m_hairInterface;

    ObjMesh *m_highResMesh, *m_lowResMesh;
    HairObject *m_hairObject;
    Simulation *m_testSimulation;

    std::vector<ShaderProgram*> m_programs;
    ShaderProgram *m_hairProgram,
                  *m_meshProgram,
                  *m_hairOpacityProgram,
                  *m_whiteMeshProgram,
                  *m_whiteHairProgram;

    std::vector<Texture*> m_textures;
    Texture *m_noiseTexture,
            *m_hairDepthTexture,
            *m_meshDepthTexture,
            *m_opacityMapTexture,
            *m_finalTexture;

    std::vector<Framebuffer*> m_framebuffers;
    Framebuffer *m_hairShadowFramebuffer,
                *m_meshShadowFramebuffer,
                *m_opacityMapFramebuffer,
                *m_finalFramebuffer;

    // Camera parameters
    glm::mat4 m_projection, m_view;
    float m_zoom = 5, m_angleX = 0, m_angleY = 0;
    QPoint m_prevMousePos;
    QPoint m_prevXformPos;

    // Light parameters
    glm::vec3 m_lightPosition;
    glm::mat4 m_eyeToLight;


    float m_hairDensity;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    int m_increment; /** Incremented on every call to paintGL. */
    float m_targetFPS;
};

#endif // GLWIDGET_H

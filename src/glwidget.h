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

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void initSimulation();

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void updateCanvas();

private:
    HairInterface *m_hairInterface;

    ObjMesh *m_highResMesh, *m_lowResMesh;
    HairObject *m_hairObject;
    Simulation *m_testSimulation;

    ShaderProgram *m_hairProgram, *m_meshProgram, *m_opacityMapProgram;
    Texture *m_noiseTexture, *m_shadowDepthTexture, *m_opacityMapTexture;
    Framebuffer *m_shadowFramebuffer, *m_opacityMapFramebuffer;

    // Camera parameters
    glm::mat4 m_projection, m_view;
    float m_zoom = 6, m_angleX = 0, m_angleY = 0;
    QPoint m_prevMousePos;

    float m_hairDensity;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    int m_increment; /** Incremented on every call to paintGL. */
    float m_targetFPS;
};

#endif // GLWIDGET_H

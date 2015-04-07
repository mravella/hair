#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>

class ObjMesh;
class HairObject;
class Simulation;
class ShaderProgram;
class HairInterface;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, HairInterface *hairInterface, QWidget *parent = 0);
    ~GLWidget();

    void resetSimulation();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
        
    void initSimulation();

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void updateCanvas();

private:
    HairInterface *m_hairInterface;

    ObjMesh *m_mesh;
    HairObject *m_hairObject;
    Simulation *m_testSimulation;

    GLuint m_meshProgramID;
    ShaderProgram *m_hairProgram, *m_meshProgram;

    float m_hairDensity;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    int m_increment; /** Incremented on every call to paintGL. */
    float m_targetFPS;
};

#endif // GLWIDGET_H

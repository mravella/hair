#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "sphere.h"
#include "hairpatch.h"
#include "shaderprogram.h"

class HairObject;
class Simulation;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    void setFPSLabel(QLabel *label);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void tick();

private:
    HairObject *m_hairObject;
    Simulation *m_testSimulation;

    ShaderProgram m_program;

    QLabel *m_fpsLabel;

    QTime m_clock;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    float m_targetFPS;
    int m_increment; /** Incremented on every call to paintGL. */
};

#endif // GLWIDGET_H

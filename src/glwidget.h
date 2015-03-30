#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <glu.h>
#include "sphere.h"
#include "hairpatch.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void tick();

private:
    HairPatch m_hairPatch;

    /** ID for the basic shader program (just vertex and fragmnet shader). */
    GLuint m_basicProgram;

    /** ID for the full shader program (vertex, tessellation, geometry, and fragmnet shaders). */
    GLuint m_fullProgram;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    float m_fps;
    int m_increment; /** Incremented on every call to paintGL. */
};

#endif // GLWIDGET_H

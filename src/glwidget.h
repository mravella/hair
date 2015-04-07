#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>

namespace Ui {
    class MainWindow;
}


class ObjMesh;
class HairObject;
class Simulation;
class ShaderProgram;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    void setUI(Ui::MainWindow *ui);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    
    void syncUI();
    
    void initSimulation();

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void tick();
    void resetSimulation();
    void setHairsPerPatch(int);
    void setSplineVertices(int);
    void setHairColorR(int);
    void setHairColorG(int);
    void setHairColorB(int);
        

private:
    ObjMesh *m_mesh;
    HairObject *m_hairObject;
    Simulation *m_testSimulation;

    GLuint m_meshProgramID;
    ShaderProgram *m_hairProgram;

    float m_hairDensity;

    Ui::MainWindow *m_ui;

    QTime m_clock;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    float m_targetFPS;
    int m_increment; /** Incremented on every call to paintGL. */
};

#endif // GLWIDGET_H

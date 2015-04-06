#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "sphere.h"
#include "hairpatch.h"
#include "shaderprogram.h"

namespace Ui {
    class MainWindow;
}


class ObjMesh;
class HairObject;
class Simulation;


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    void setFPSLabel(QLabel *label);
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
    ShaderProgram m_hairProgram;

    Ui::MainWindow *m_ui;
    QLabel *m_fpsLabel;

    QTime m_clock;

    QTimer m_timer; /** Timer calls tick() 60 times per second. */
    float m_targetFPS;
    int m_increment; /** Incremented on every call to paintGL. */
};

#endif // GLWIDGET_H

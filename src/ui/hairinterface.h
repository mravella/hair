#ifndef HAIRINTERFACE_H
#define HAIRINTERFACE_H

#include <QObject>
#include <QTime>

namespace Ui {
    class MainWindow;
}
class HairObject;
class GLWidget;

class HairInterface : public QObject
{
    Q_OBJECT

public:
    HairInterface(Ui::MainWindow *ui);
    virtual ~HairInterface() { }

    void setGLWidget(GLWidget *glWidget);
    void setHairObject(HairObject *hairObject);
    void updateFPSLabel(int totalNumFrames);
    void updateStatsLabel();

public slots:
    void resetSimulation();
    void setHairsPerPatch(int);
    void setSplineVertices(int);
    void setHairRadius(int);
    void setHairColorR(int);
    void setHairColorG(int);
    void setHairColorB(int);
    void setShadows(bool);

private:
    void connectUserInputs();

    Ui::MainWindow *m_ui;
    HairObject *m_hairObject;
    GLWidget *m_glWidget;
    QTime m_clock;
};

#endif // HAIRINTERFACE_H

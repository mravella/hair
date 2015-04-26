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
    void showHideGroupSim();
    void showHideGroupTess();
    void showHideGroupRender();
    
    void resetSimulation();
    
    void inputHairsPerPatchText(QString text);
    void setHairsPerPatch(int);
    
    void inputSplineVerticesText(QString text);
    void setSplineVertices(int numVertices);
    
    void inputHairRadiusText(QString text);
    void setHairRadius(int value);
    
    void inputNoiseAmpText(QString text);
    void setNoiseAmp(int value);
    
    void inputHairColorRText(QString text);
    void setHairColorR(int value);
    void inputHairColorGText(QString text);
    void setHairColorG(int value);
    void inputHairColorBText(QString text);
    void setHairColorB(int value);

    void setShadows(bool);
    void setSupersampling(bool);
    void setFrictionSim(bool);
    void togglePaused();

private:
    void connectUserInputs();

    Ui::MainWindow *m_ui;
    HairObject *m_hairObject;
    GLWidget *m_glWidget;
    QTime m_clock;
};

#endif // HAIRINTERFACE_H
